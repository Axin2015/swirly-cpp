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
#ifndef SWIRLY_SYS_TIMER_HPP
#define SWIRLY_SYS_TIMER_HPP

#include <swirly/sys/AsyncHandler.hpp>
#include <swirly/sys/Handle.hpp>
#include <swirly/sys/Time.hpp>

#include <swirly/Config.h>

#include <vector>

namespace swirly {

class TimerQueue;

struct TimerPolicy {
    struct Id {
        TimerQueue* queue{nullptr};
        long value{-1};
    };
    static constexpr Id invalid() noexcept { return {}; }
    static void close(Id id) noexcept;
};

inline bool operator==(TimerPolicy::Id lhs, TimerPolicy::Id rhs) noexcept
{
    assert(lhs.queue == rhs.queue || !lhs.queue || !rhs.queue);
    return lhs.value == rhs.value;
}

inline bool operator!=(TimerPolicy::Id lhs, TimerPolicy::Id rhs) noexcept
{
    assert(lhs.queue == rhs.queue || !lhs.queue || !rhs.queue);
    return lhs.value != rhs.value;
}

using Timer = Handle<TimerPolicy>;

struct TimerEntry {
    TimerEntry(long id, Time expiry, Duration interval, const AsyncHandlerPtr& handler)
        : id{id}, expiry{expiry}, interval{interval}, handler{handler}
    {
    }
    long id;
    Time expiry;
    Duration interval;
    AsyncHandlerPtr handler;
};

inline bool operator<(const TimerEntry& lhs, const TimerEntry& rhs) noexcept
{
    return lhs.expiry > rhs.expiry;
}

class SWIRLY_API TimerQueue {
    friend struct TimerPolicy;

  public:
    TimerQueue() = default;

    // Copy.
    TimerQueue(const TimerQueue&) = delete;
    TimerQueue& operator=(const TimerQueue&) = delete;

    // Move.
    TimerQueue(TimerQueue&&) = default;
    TimerQueue& operator=(TimerQueue&&) = default;

    const TimerEntry& front() const { return timers_.front(); }
    bool empty() const { return timers_.empty(); }

    Timer set(Time expiry, Duration interval, const AsyncHandlerPtr& handler);
    Timer set(Time expiry, const AsyncHandlerPtr& handler)
    {
        return set(expiry, Duration::zero(), handler);
    }

    // Resetting the interval will not reschedule any pending timer.
    bool reset(long id, Duration interval);
    bool reset(Timer::Id id, Duration interval)
    {
        assert(id.queue == this);
        return reset(id.value, interval);
    }

    void cancel(long id);
    void cancel(Timer::Id id)
    {
        assert(id.queue == this);
        cancel(id.value);
    }

    bool expire(Time now);

  private:
    // Heap of timers ordered by expiry time.
    std::vector<TimerEntry> timers_;
    long maxId_{};
};

inline void TimerPolicy::close(Id id) noexcept
{
    id.queue->cancel(id.value);
}

} // namespace swirly

#endif // SWIRLY_SYS_TIMER_HPP
