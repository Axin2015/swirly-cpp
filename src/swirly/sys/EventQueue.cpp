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
#include "EventQueue.hpp"

namespace swirly {

EventQueue::EventQueue(std::size_t capacity)
{
    rdbuf_.reserve(capacity);
    wrbuf_.reserve(capacity);
    it_ = rdbuf_.begin();
}

EventQueue::~EventQueue() noexcept = default;

Event EventQueue::pop()
{
    if (it_ == rdbuf_.end()) {
        {
            std::lock_guard<std::mutex> lock{mutex_};
            rdbuf_.swap(wrbuf_);
        }
        it_ = rdbuf_.begin();
    }
    Event ev;
    if (it_ != rdbuf_.end()) {
        ev = *it_++;
    }
    return ev;
}

} // namespace swirly
