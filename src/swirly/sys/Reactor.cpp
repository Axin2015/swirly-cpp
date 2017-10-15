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

void Reactor::dispatch(long modcnt, Event* events, int size) const
{
    for (int i{0}; i < size; ++i) {
        auto& event = events[i];
        const auto& data = data_[event.data.fd];
        // Skip this socket if it was modified after the call to wait().
        if (data.modcnt > modcnt) {
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
        eh->ioEvent(event.data.fd, event.events);
    }
}

} // namespace swirly
