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

struct Data {
    int sid{};
    FileEvents mask{};
    EventHandlerPtr handler;
};

} // namespace

struct Reactor::Impl {

    explicit Impl(size_t sizeHint)
    : mux{sizeHint}
    {
        const auto fd = ef.fd();
        data.resize(max<size_t>(fd + 1, sizeHint));
        mux.subscribe(fd, 0, In);

        auto& ref = data[fd];
        ref.sid = 0;
        ref.mask = In;
        ref.handler = {};
    }
    ~Impl() noexcept { mux.unsubscribe(ef.fd()); }

    Muxer mux;
    vector<Data> data;
    EventFile ef;
    TimerQueue tq;
    std::atomic<bool> closed{false};
};

Reactor::Reactor(size_t sizeHint)
: impl_{make_unique<Impl>(sizeHint)}
{
}

Reactor::~Reactor() noexcept = default;

// Move.
Reactor::Reactor(Reactor&&) = default;
Reactor& Reactor::operator=(Reactor&&) = default;

bool Reactor::closed() const noexcept
{
    return impl_->closed.load(std::memory_order_acquire);
}

void Reactor::close() noexcept
{
    impl_->closed.store(true, std::memory_order_release);
    impl_->ef.notify();
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
    int n{0};
    for (int i{0}; i < size; ++i) {

        auto& ev = buf[i];
        const auto fd = static_cast<int>(ev.data.u64 & 0xffffffff);
        if (fd == impl_->ef.fd()) {
            SWIRLY_INFO("reactor wakeup"sv);
            impl_->ef.flush();
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

} // namespace swirly
