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

Token Reactor::attach(int fd, FileEvents mask, const ActorPtr& actor)
{
    assert(fd >= 0);
    assert(actor);
    if (fd >= static_cast<int>(data_.size())) {
        data_.resize(fd + 1);
    }
    auto& data = data_[fd];
    mux_.attach(++data.sid, fd, mask);
    data.mask = mask;
    data.actor = actor;
    return Token{{this, fd}};
}

void Reactor::mask(int fd, FileEvents mask)
{
    if (data_[fd].mask != mask) {
        mux_.setMask(fd, mask);
        data_[fd].mask = mask;
    }
}

void Reactor::detach(int fd) noexcept
{
    mux_.detach(fd);
    auto& data = data_[fd];
    data.mask = 0;
    data.actor.reset();
}

Timer Reactor::timer(Time expiry, Duration interval, const ActorPtr& actor)
{
    return tq_.insert(expiry, interval, actor);
}

Timer Reactor::timer(Time expiry, const ActorPtr& actor)
{
    return tq_.insert(expiry, actor);
}

int Reactor::poll(chrono::milliseconds timeout)
{
    using namespace chrono;

    if (!tq_.empty()) {
        // Millis until next expiry.
        const auto expiry = duration_cast<milliseconds>(tq_.front().expiry() - UnixClock::now());
        if (expiry < timeout) {
            timeout = max(expiry, 0ms);
        }
    }
    FileEvent buf[16];
    error_code ec;
    const auto ret = mux_.wait(buf, 16, timeout, ec);
    if (ret < 0) {
        if (ec.value() != EINTR) {
            throw system_error{ec};
        }
        return 0;
    }
    const auto now = UnixClock::now();
    return tq_.dispatch(now) + dispatch(buf, ret, now);
}

int Reactor::dispatch(FileEvent* buf, int size, Time now)
{
    for (int i{0}; i < size; ++i) {

        auto& ev = buf[i];
        const auto fd = static_cast<int>(ev.data.u64 & 0xffffffff);
        const auto sid = static_cast<int>(ev.data.u64 >> 32);

        const auto& data = data_[fd];
        // Skip this socket if it was modified after the call to wait().
        if (data.sid > sid) {
            continue;
        }
        // Apply the interest mask to filter-out any events that the user may have removed from the
        // mask since the call to wait() was made. This would typically happen via a reentrant call
        // into the reactor from an actor.
        ev.events &= data.mask;
        if (!(ev.events)) {
            continue;
        }
        ActorPtr actor{data.actor};
        try {
            actor->ready(fd, ev.events, now);
        } catch (const std::exception& e) {
            using namespace std::string_literals;
            SWIRLY_ERROR("error handling io event: "s + e.what());
        }
    }
    return size;
}

} // namespace swirly
