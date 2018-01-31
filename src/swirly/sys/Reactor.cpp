/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#include "Reactor.hpp"

#include "EventFile.hpp"
#include "Log.hpp"

#include <algorithm>
#include <unordered_map>

namespace swirly {
using namespace std;
namespace {

constexpr size_t MaxEvents{16};

class EventDispatcher {
    enum : int { Unlocked = -1 };
  public:
    EventDispatcher() = default;
    ~EventDispatcher() noexcept = default;

    // Copy.
    EventDispatcher(const EventDispatcher&) = default;
    EventDispatcher& operator=(const EventDispatcher&) = default;

    // Move.
    EventDispatcher(EventDispatcher&&) = default;
    EventDispatcher& operator=(EventDispatcher&&) = default;

    int dispatch(MsgQueue& mq, Time now) noexcept
    {
        struct Lock {
            explicit Lock(int& state)
              : state_(state)
            {
                state = 0;
            }
            ~Lock() noexcept { state_ = Unlocked; }
            int& state_;
        } lock{state_};

        int n{0};
        while (mq.fetch([this, now](const MsgEvent& ev) noexcept { dispatch(ev, now); })) {
            ++n;
        }
        if (state_ > 0) {
            handlers_.erase(remove_if(handlers_.begin(), handlers_.end(), [](const auto& ptr) { return !ptr; }), handlers_.end());
        }
        return n;
    }
    void subscribe(const EventHandlerPtr& handler)
    {
        handlers_.push_back(handler);
    }
    void unsubscribe(const EventHandler& handler)
    {
        const auto pred = [&handler](const auto& ptr) { return ptr.get() == &handler; };
        const auto it = find_if(handlers_.begin(), handlers_.end(), pred);
        if (it != handlers_.end()) {
            if (state_ == Unlocked) {
                handlers_.erase(it);
            } else {
                it->reset();
                ++state_;
            }
        }
    }

  private:
    void dispatch(const MsgEvent& ev, Time now) noexcept
    {
        const auto n = handlers_.size();
        for (size_t i{0}; i < n; ++i) {
            const auto handler = handlers_[i];
            if (handler) {
                try {
                    if (ev.type != Signal) {
                        handler->onEvent(ev, now);
                    } else {
                        handler->onSignal(data<int>(ev), now);
                    }
                } catch (const std::exception& e) {
                    using namespace string_literals;
                    SWIRLY_ERROR("error handling event: "s + e.what());
                }
            }
        }

    }
    vector<EventHandlerPtr> handlers_;
    int state_{Unlocked};
};

struct Data {
    int sid{};
    FileEvents mask{};
    EventHandlerPtr handler;
};

atomic<vector<Reactor>*> reactors_{nullptr};

} // namespace

struct Reactor::Impl {

    explicit Impl(size_t sizeHint)
      : mux{sizeHint}
      , mq{ef, 1 << 14}
    {
        const auto fd = mq.fd();
        data.resize(max<size_t>(fd + 1, sizeHint));

        auto& ref = data[fd];
        mux.subscribe(fd, 0, In);
        ref.mask = In;
        ref.handler = {};
    }

    Muxer mux;
    vector<Data> data;
    EventDispatcher ed;
    EventFile ef;
    MsgQueue mq;
    TimerQueue tq;
};

Reactor::Reactor(size_t sizeHint)
  : impl_{make_unique<Impl>(sizeHint)}
{
}

Reactor::~Reactor() noexcept = default;

// Move.
Reactor::Reactor(Reactor&&) = default;
Reactor& Reactor::operator=(Reactor&&) = default;

EventToken Reactor::subscribe(const EventHandlerPtr& handler)
{
    impl_->ed.subscribe(handler);
    return EventToken{{this, handler.get()}};
}

FileToken Reactor::subscribe(int fd, FileEvents mask, const EventHandlerPtr& handler)
{
    assert(fd >= 0);
    assert(handler);
    if (fd >= static_cast<int>(impl_->data.size())) {
        impl_->data.resize(fd + 1);
    }
    auto& ref = impl_->data[fd];
    impl_->mux.subscribe(fd, ++ref.sid, mask);
    ref.mask = mask;
    ref.handler = handler;
    return FileToken{{this, fd}};
}

void Reactor::unsubscribe(const EventHandler& handler) noexcept
{
    impl_->ed.unsubscribe(handler);
}

void Reactor::unsubscribe(int fd) noexcept
{
    impl_->mux.unsubscribe(fd);
    auto& ref = impl_->data[fd];
    ref.mask = 0;
    ref.handler.reset();
}

void Reactor::setMask(int fd, FileEvents mask)
{
    auto& ref = impl_->data[fd];
    if (ref.mask != mask) {
        impl_->mux.setMask(fd, ref.sid, mask);
        impl_->data[fd].mask = mask;
    }
}

Timer Reactor::setTimer(Time expiry, Duration interval, const EventHandlerPtr& handler)
{
    return impl_->tq.insert(expiry, interval, handler);
}

Timer Reactor::setTimer(Time expiry, const EventHandlerPtr& handler)
{
    return impl_->tq.insert(expiry, handler);
}

int Reactor::poll(chrono::milliseconds timeout)
{
    using namespace chrono;

    if (!impl_->tq.empty()) {
        // Millis until next expiry.
        const auto expiry
            = duration_cast<milliseconds>(impl_->tq.front().expiry() - UnixClock::now());
        if (expiry < timeout) {
            timeout = max(expiry, 0ms);
        }
    }
    FileEvent buf[MaxEvents];
    error_code ec;
    int ret;
    if (timeout > 0ms && impl_->mq.park()) {
        ret = impl_->mux.wait(buf, MaxEvents, timeout, ec);
        impl_->mq.unpark();
    } else {
        ret = impl_->mux.wait(buf, MaxEvents, 0ms, ec);
    }
    if (ret < 0) {
        if (ec.value() != EINTR) {
            throw system_error{ec};
        }
        return 0;
    }
    const auto now = UnixClock::now();
    return impl_->ed.dispatch(impl_->mq, now) + impl_->tq.dispatch(now) + dispatch(buf, ret, now);
}

MsgQueue& Reactor::mq() noexcept
{
    return impl_->mq;
}

int Reactor::dispatch(FileEvent* buf, int size, Time now)
{
    int n{0};
    for (int i{0}; i < size; ++i) {

        auto& ev = buf[i];
        const auto fd = static_cast<int>(ev.data.u64 & 0xffffffff);
        if (fd == impl_->mq.fd()) {
            impl_->mq.flush();
            continue;
        }

        const auto sid = static_cast<int>(ev.data.u64 >> 32);

        const auto& ref = impl_->data[fd];
        // Skip this socket if it was modified after the call to wait().
        if (ref.sid > sid) {
            continue;
        }
        // Apply the interest mask to filter-out any events that the user may have removed from the
        // mask since the call to wait() was made. This would typically happen via a reentrant call
        // into the reactor from an event-handler.
        ev.events &= ref.mask;
        if (!(ev.events)) {
            continue;
        }

        EventHandlerPtr eh{ref.handler};
        try {
            eh->onReady(fd, ev.events, now);
        } catch (const std::exception& e) {
            using namespace string_literals;
            SWIRLY_ERROR("error handling io event: "s + e.what());
        }
        ++n;
    }
    return n;
}

std::vector<Reactor>& getReactors() noexcept
{
    auto* rs = reactors_.load(memory_order_acquire);
    assert(rs);
    return *rs;
}

void setReactors(vector<Reactor>& rs) noexcept
{
    return reactors_.store(&rs, memory_order_release);
}

} // namespace swirly
