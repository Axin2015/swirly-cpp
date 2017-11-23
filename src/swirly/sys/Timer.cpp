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
#include "Timer.hpp"

#include "Log.hpp"

#include <algorithm>

namespace swirly {

using namespace std;

Timer TimerQueue::set(Time expiry, Duration interval, const AsyncHandlerPtr& handler)
{
    const auto id = ++maxId_;
    timers_.emplace_back(id, expiry, interval, handler);
    push_heap(timers_.begin(), timers_.end());
    return Timer{{this, id}};
}

bool TimerQueue::reset(long id, Duration interval)
{
    auto it = find_if(timers_.begin(), timers_.end(), [id](const auto& t) { return t.id == id; });
    if (it == timers_.end()) {
        // No matching timer.
        return false;
    }
    it->interval = interval;
    return true;
}

void TimerQueue::cancel(long id)
{
    auto it = find_if(timers_.begin(), timers_.end(), [id](const auto& t) { return t.id == id; });
    if (it != timers_.end()) {
        // Remove timer.
        pop_heap(it, timers_.end());
        timers_.pop_back();
    }
}

bool TimerQueue::expire(Time now)
{
    if (timers_.empty() || timers_.front().expiry > now) {
        return false;
    }

    // Save timer-entry.
    auto timer = timers_.front();
    try {
        timer.handler->timer(timer.id, now);
    } catch (const std::exception& e) {
        using namespace std::string_literals;
        SWIRLY_ERROR("error handling timer event: "s + e.what());
    }

    // If timer was not cancelled during callback.
    if (timers_.front().id == timer.id) {

        // User may have reset interval during callback.
        timer.interval = timers_.front().interval;

        // Remove timer.
        pop_heap(timers_.begin(), timers_.end());
        timers_.pop_back();

        // If the timer should be restarted.
        if (timer.interval.count() > 0) {
            // Add interval to expiry, while ensuring that next expiry is always in the future.
            timer.expiry = std::max(timer.expiry + timer.interval, now + Nanos{1});
            // Reschedule timer.
            timers_.push_back(timer);
            std::push_heap(timers_.begin(), timers_.end());
        }
    }
    return true;
}

} // namespace swirly
