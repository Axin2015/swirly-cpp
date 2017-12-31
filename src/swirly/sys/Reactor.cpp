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
#include "EventQueue.hpp"
#include "Log.hpp"

#include <algorithm>
#include <unordered_map>

namespace swirly {
using namespace std;
namespace {

constexpr size_t MaxEvents{16};

class EventDispatcher {
  public:
    EventDispatcher() = default;
    ~EventDispatcher() noexcept = default;

    // Copy.
    EventDispatcher(const EventDispatcher&) = default;
    EventDispatcher& operator=(const EventDispatcher&) = default;

    // Move.
    EventDispatcher(EventDispatcher&&) = default;
    EventDispatcher& operator=(EventDispatcher&&) = default;

    void dispatch(const Event& ev)
    {
        const auto addr = ev.to();
        const auto it = map_.find(addr);
        if (it == map_.end()) {
            return;
        }
        auto& v = it->second;

        locked_ = addr;
        struct Finally {
            explicit Finally(Address& locked)
              : locked_(locked)
            {
            }
            ~Finally() noexcept { locked_ = Address::None; }
            Address& locked_;
        } finally{locked_};

        unsubs_ = 0;

        const auto n = v.size();
        for (size_t i{0}; i < n; ++i) {
            const auto actor = v[i];
            if (actor) {
                try {
                    if (addr != Address::Signal) {
                        actor->onEvent(ev);
                    } else {
                        actor->onSignal(ev.type());
                    }
                } catch (const std::exception& e) {
                    using namespace string_literals;
                    SWIRLY_ERROR("error handling event: "s + e.what());
                }
            }
        }

        if (unsubs_ > 0) {
            v.erase(remove_if(v.begin(), v.end(), [](const auto& ptr) { return !ptr; }), v.end());
        }
    }
    void subscribe(Address addr, const ActorPtr& actor)
    {
        auto& v = map_[addr];
        if (find(v.begin(), v.end(), actor) != v.end()) {
            throw logic_error{"already subscribed"};
        }
        v.push_back(actor);
    }
    void unsubscribe(Address addr, const Actor& actor)
    {
        auto& v = map_[addr];
        const auto pred = [&actor](const auto& ptr) { return ptr.get() == &actor; };
        if (addr != locked_) {
            v.erase(remove_if(v.begin(), v.end(), pred), v.end());
        } else {
            const auto it = find_if(v.begin(), v.end(), pred);
            if (it != v.end()) {
                it->reset();
                ++unsubs_;
            }
        }
    }

  private:
    unordered_map<Address, vector<ActorPtr>> map_;
    Address locked_{Address::None};
    int unsubs_{};
};

atomic<vector<Reactor>*> reactors_{nullptr};

} // namespace

struct Reactor::Impl {

    explicit Impl(size_t sizeHint)
      : mux{sizeHint}
    {
        const auto fd = ef.waitfd();
        data.resize(max<size_t>(fd + 1, sizeHint));

        auto& ref = data[fd];
        mux.subscribe(0, fd, In);
        ref.mask = In;
        ref.actor = {};
    }

    Muxer mux;
    vector<Data> data;
    EventDispatcher ed;
    EventFile ef;
    EventQueue eq;
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

void Reactor::postEvent(const Event& ev)
{
    if (impl_->eq.push(ev) == 1) {
        impl_->ef.notify();
    }
}

void Reactor::postEvent(Event&& ev)
{
    if (impl_->eq.push(move(ev)) == 1) {
        impl_->ef.notify();
    }
}

FileToken Reactor::subscribe(int fd, FileEvents mask, const ActorPtr& actor)
{
    assert(fd >= 0);
    assert(actor);
    if (fd >= static_cast<int>(impl_->data.size())) {
        impl_->data.resize(fd + 1);
    }
    auto& ref = impl_->data[fd];
    impl_->mux.subscribe(++ref.sid, fd, mask);
    ref.mask = mask;
    ref.actor = actor;
    return FileToken{{this, fd}};
}

EventToken Reactor::subscribe(Address addr, const ActorPtr& actor)
{
    impl_->ed.subscribe(addr, actor);
    return EventToken{{this, addr, actor.get()}};
}

void Reactor::unsubscribe(int fd) noexcept
{
    impl_->mux.unsubscribe(fd);
    auto& ref = impl_->data[fd];
    ref.mask = 0;
    ref.actor.reset();
}

void Reactor::unsubscribe(Address addr, const Actor& actor) noexcept
{
    impl_->ed.unsubscribe(addr, actor);
}

void Reactor::setMask(int fd, FileEvents mask)
{
    if (impl_->data[fd].mask != mask) {
        impl_->mux.setMask(fd, mask);
        impl_->data[fd].mask = mask;
    }
}

Timer Reactor::setTimer(Time expiry, Duration interval, const ActorPtr& actor)
{
    return impl_->tq.insert(expiry, interval, actor);
}

Timer Reactor::setTimer(Time expiry, const ActorPtr& actor)
{
    return impl_->tq.insert(expiry, actor);
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
    const auto ret = impl_->mux.wait(buf, MaxEvents, timeout, ec);
    if (ret < 0) {
        if (ec.value() != EINTR) {
            throw system_error{ec};
        }
        return 0;
    }
    const auto now = UnixClock::now();
    return impl_->tq.dispatch(now) + dispatch(buf, ret, now);
}

int Reactor::dispatch(FileEvent* buf, int size, Time now)
{
    int n{};
    for (int i{0}; i < size; ++i) {

        auto& ev = buf[i];
        const auto fd = static_cast<int>(ev.data.u64 & 0xffffffff);
        if (fd == impl_->ef.waitfd()) {
            while (auto event = impl_->eq.pop()) {
                impl_->ed.dispatch(event);
                ++n;
            }
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
        // into the reactor from an actor.
        ev.events &= ref.mask;
        if (!(ev.events)) {
            continue;
        }

        ActorPtr actor{ref.actor};
        try {
            actor->onReady(fd, ev.events, now);
        } catch (const std::exception& e) {
            using namespace string_literals;
            SWIRLY_ERROR("error handling io event: "s + e.what());
        }
        ++n;
    }
    return n;
}

void setReactors(vector<Reactor>& rs) noexcept
{
    return reactors_.store(&rs, memory_order_release);
}

void postEvent(const Event& ev)
{
    auto* rs = reactors_.load(memory_order_acquire);
    assert(rs);
    for_each(rs->begin(), rs->end(), [&ev](auto& r) { r.postEvent(ev); });
}

} // namespace swirly
