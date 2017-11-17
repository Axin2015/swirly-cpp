/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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

#include "Log.hpp"

namespace swirly {

using namespace std;

void Reactor::attach(int fd, EventMask mask, const AsyncHandlerPtr& handler)
{
    assert(fd >= 0);
    assert(handler);
    if (fd >= static_cast<int>(data_.size())) {
        data_.resize(fd + 1);
    }
    auto& data = data_[fd];
    mux_.attach(++data.sid, fd, mask);
    data.mask = mask;
    data.handler = handler;
}

void Reactor::setMask(int fd, EventMask mask)
{
    if (data_[fd].mask != mask) {
        mux_.setMask(fd, mask);
        data_[fd].mask = mask;
    }
}

void Reactor::detach(int fd)
{
    mux_.detach(fd);
    auto& data = data_[fd];
    data.mask = 0;
    data.handler.reset();
}

Timer Reactor::setTimer(Time expiry, Duration interval, const AsyncHandlerPtr& handler)
{
    return tq_.set(expiry, interval, handler);
}

Timer Reactor::setTimer(Time expiry, const AsyncHandlerPtr& handler)
{
    return tq_.set(expiry, handler);
}

bool Reactor::resetTimer(long id, Duration interval)
{
    return tq_.reset(id, interval);
}

bool Reactor::resetTimer(Timer::Id id, Duration interval)
{
    return tq_.reset(id, interval);
}

void Reactor::cancelTimer(long id)
{
    tq_.cancel(id);
}

void Reactor::cancelTimer(Timer::Id id)
{
    tq_.cancel(id);
}

int Reactor::poll(chrono::milliseconds timeout)
{
    using namespace chrono;

    if (!tq_.empty()) {
        // Millis until next expiry.
        const auto expiry = duration_cast<milliseconds>(tq_.front().expiry - UnixClock::now());
        if (expiry < timeout) {
            timeout = max(expiry, 0ms);
        }
    }
    Event events[16];
    const auto size = mux_.wait(events, 16, timeout);
    dispatch(events, size, UnixClock::now());
    return size;
}

void Reactor::dispatch(Event* events, int size, Time now)
{
    // Dispatch timer notifications.
    while (tq_.expire(now))
        ;

    for (int i{0}; i < size; ++i) {

        auto& event = events[i];
        const auto fd = static_cast<int>(event.data.u64 & 0xffffffff);
        const auto sid = static_cast<int>(event.data.u64 >> 32);

        const auto& data = data_[fd];
        // Skip this socket if it was modified after the call to wait().
        if (data.sid > sid) {
            continue;
        }
        // Apply the interest mask to filter-out any events that the user may have removed from
        // the mask since the call to wait() was made. This would typically happen via a
        // reentrant call into the reactor from an event handler.
        event.events &= data.mask;
        if (!(event.events)) {
            continue;
        }
        AsyncHandlerPtr handler{data.handler};
        try {
            handler->event(fd, event.events, now);
        } catch (const std::exception& e) {
            using namespace std::string_literals;
            SWIRLY_ERROR("error handling io event: "s + e.what());
        }
    }
}

} // namespace swirly
