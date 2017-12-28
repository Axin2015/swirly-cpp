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
#ifndef SWIRLY_SYS_EVENTQUEUE_HPP
#define SWIRLY_SYS_EVENTQUEUE_HPP

#include <swirly/sys/Event.hpp>

#include <mutex>
#include <vector>

namespace swirly {

class SWIRLY_API EventQueue {
    using Buffer = std::vector<Event>;

  public:
    explicit EventQueue(std::size_t capacity = 1024);
    ~EventQueue() noexcept;

    // Copy.
    EventQueue(const EventQueue&) = delete;
    EventQueue& operator=(const EventQueue&) = delete;

    // Move.
    EventQueue(EventQueue&&) = delete;
    EventQueue& operator=(EventQueue&&) = delete;

    Event pop();

    /**
     * Returns number of pending events after push.
     */
    std::size_t push(const Event& ev)
    {
        std::lock_guard<std::mutex> lock{mutex_};
        wrbuf_.push_back(ev);
        return wrbuf_.size();
    }

    /**
     * Returns number of pending events after push.
     */
    std::size_t push(Event&& ev)
    {
        std::lock_guard<std::mutex> lock{mutex_};
        wrbuf_.push_back(std::move(ev));
        return wrbuf_.size();
    }

  private:
    Buffer::iterator it_;
    std::mutex mutex_;
    Buffer rdbuf_, wrbuf_;
};

} // namespace swirly

#endif // SWIRLY_SYS_EVENTQUEUE_HPP
