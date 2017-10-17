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

namespace swirly {

EventHandler::~EventHandler() noexcept = default;

int Reactor::poll() const
{
    Event events[16];
    const auto size = mux_.wait(events, 16);
    dispatch(events, size);
    return size;
}

int Reactor::poll(std::chrono::milliseconds timeout) const
{
    Event events[16];
    const auto size = mux_.wait(events, 16, timeout);
    dispatch(events, size);
    return size;
}

void Reactor::insert(int fd, EventMask mask, const EventHandlerPtr& eh)
{
    assert(fd >= 0);
    assert(eh);
    if (fd >= static_cast<int>(data_.size())) {
        data_.resize(fd + 1);
    }
    auto& data = data_[fd];
    mux_.insert(++data.sid, fd, mask);
    data.mask = mask;
    data.eh = eh;
}

void Reactor::update(int fd, EventMask mask)
{
    if (data_[fd].mask != mask) {
        mux_.update(fd, mask);
        data_[fd].mask = mask;
    }
}

void Reactor::erase(int fd)
{
    mux_.erase(fd);
    auto& data = data_[fd];
    data.mask = 0;
    data.eh.reset();
}

void Reactor::dispatch(Event* events, int size) const
{
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
        EventHandlerPtr eh{data.eh};
        eh->ioEvent(fd, event.events);
    }
}

} // namespace swirly
