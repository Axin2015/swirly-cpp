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
#ifndef SWIRLY_DISRUPT_TYPES_HPP
#define SWIRLY_DISRUPT_TYPES_HPP

#include <iostream>
#include <memory>

namespace swirly {

class EventHandler;
using EventHandlerPtr = std::shared_ptr<EventHandler>;

class LongEvent {
    friend std::ostream& operator<<(std::ostream& os, const LongEvent& event)
    {
        return os << "LongEvent{" << event.value_ << '}';
    }

  public:
    void set(long value) noexcept { value_ = value; }

  private:
    long value_{};
};

using EventT = LongEvent;

} // namespace swirly

#endif // SWIRLY_DISRUPT_TYPES_HPP
