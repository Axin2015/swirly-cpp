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
#include "EpollReactor.hpp"

#include <swirly/sys/TimerFd.hpp>

#include <swirly/util/Log.hpp>

namespace swirly {
inline namespace sys {
using namespace std;
namespace {

constexpr size_t MaxEvents{16};

} // namespace

EpollReactor::EpollReactor(size_t size_hint)
: mux_{size_hint}
{
    const auto efd = efd_.fd();
    data_.resize(max<size_t>(efd + 1, size_hint));
    mux_.subscribe(efd, 0, EventIn);
}

EpollReactor::~EpollReactor()
{
    mux_.unsubscribe(efd_.fd());
}

void EpollReactor::do_interrupt() noexcept
{
    // Best effort.
    std::error_code ec;
    efd_.write(1, ec);
}

Reactor::Handle EpollReactor::do_subscribe(int fd, unsigned events, IoSlot slot)
{
    assert(fd >= 0);
    assert(slot);
    if (fd >= static_cast<int>(data_.size())) {
        data_.resize(fd + 1);
    }
    auto& ref = data_[fd];
    mux_.subscribe(fd, ++ref.sid, events);
    ref.events = events;
    ref.slot = slot;
    return {*this, fd, ref.sid};
}

void EpollReactor::do_unsubscribe(int fd, int sid) noexcept
{
    auto& ref = data_[fd];
    if (ref.sid == sid) {
        mux_.unsubscribe(fd);
        ref.events = 0;
        ref.slot.reset();
    }
}

void EpollReactor::do_set_events(int fd, int sid, unsigned events, IoSlot slot)
{
    auto& ref = data_[fd];
    if (ref.sid == sid) {
        if (ref.events != events) {
            mux_.set_events(fd, sid, events);
            ref.events = events;
        }
        ref.slot = slot;
    }
}

void EpollReactor::do_set_events(int fd, int sid, unsigned events)
{
    auto& ref = data_[fd];
    if (ref.sid == sid && ref.events != events) {
        mux_.set_events(fd, sid, events);
        ref.events = events;
    }
}

Timer EpollReactor::do_timer(WallTime expiry, Duration interval, Priority priority, TimerSlot slot)
{
    return tqs_[static_cast<size_t>(priority)].insert(expiry, interval, slot);
}

Timer EpollReactor::do_timer(WallTime expiry, Priority priority, TimerSlot slot)
{
    return tqs_[static_cast<size_t>(priority)].insert(expiry, slot);
}

int EpollReactor::do_poll(WallTime now, Duration timeout)
{
    enum { High = 0, Low = 1 };
    using namespace chrono;

    // If timeout is zero then the wait_until time should also be zero to signify no wait.
    WallTime wait_until{};
    if (!is_zero(timeout)) {
        const WallTime next = next_expiry(timeout == NoTimeout ? WallClock::max() : now + timeout);
        if (next > now) {
            wait_until = next;
        }
    }

    Event buf[MaxEvents];
    error_code ec;
    int ret;
    if (wait_until == WallClock::max()) {
        // Block indefinitely.
        ret = mux_.wait(buf, MaxEvents, ec);
    } else {
        // The wait function will not block if time is zero.
        ret = mux_.wait(buf, MaxEvents, wait_until, ec);
    }
    if (ret < 0) {
        if (ec.value() != EINTR) {
            throw system_error{ec};
        }
        return 0;
    }
    // If the muxer call was a blocking call, then acquire the current time.
    if (!is_zero(wait_until)) {
        now = WallClock::now();
    }
    const auto n = tqs_[High].dispatch(now) + dispatch(now, buf, ret);
    // Low priority timers are only dispatched during empty cycles.
    return n == 0 ? tqs_[Low].dispatch(now) : n;
}

WallTime EpollReactor::next_expiry(WallTime next) const
{
    enum { High = 0, Low = 1 };
    using namespace chrono;
    {
        auto& tq = tqs_[High];
        if (!tq.empty()) {
            // Duration until next expiry. Mitigate scheduler latency by preempting the
            // high-priority timer and busy-waiting for 200us ahead of timer expiry.
            next = min(next, tq.front().expiry() - 200us);
        }
    }
    {
        auto& tq = tqs_[Low];
        if (!tq.empty()) {
            // Duration until next expiry.
            next = min(next, tq.front().expiry());
        }
    }
    return next;
}

int EpollReactor::dispatch(WallTime now, Event* buf, int size)
{
    int n{0};
    for (int i{0}; i < size; ++i) {

        auto& ev = buf[i];
        const auto fd = mux_.fd(ev);
        if (fd == efd_.fd()) {
            SWIRLY_INFO << "reactor interrupted";
            efd_.read();
            continue;
        }
        const auto& ref = data_[fd];
        if (!ref.slot) {
            // Ignore timerfd.
            continue;
        }

        const auto sid = mux_.sid(ev);
        // Skip this socket if it was modified after the call to wait().
        if (ref.sid > sid) {
            continue;
        }
        // Apply the interest events to filter-out any events that the user may have removed from
        // the events since the call to wait() was made. This would typically happen via a reentrant
        // call into the reactor from an event-handler. N.B. EventErr and EventHup are always
        // reported if they occur, regardless of whether they are specified in events.
        const auto events = mux_.events(ev) & (ref.events | EventErr | EventHup);
        if (!events) {
            continue;
        }

        try {
            ref.slot(now, fd, events);
        } catch (const std::exception& e) {
            SWIRLY_ERROR << "error handling io event: " << e.what();
        }
        ++n;
    }
    return n;
}

} // namespace sys
} // namespace swirly
