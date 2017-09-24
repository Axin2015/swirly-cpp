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
#ifndef SWIRLY_UTIL_TIME_HPP
#define SWIRLY_UTIL_TIME_HPP

#include <swirly/Defs.hpp>

#include <chrono>
#include <iosfwd>

#include <sys/time.h>

namespace swirly {

using namespace std::literals::chrono_literals;

struct SWIRLY_API UnixClock {
    using duration = std::chrono::nanoseconds;
    using rep = duration::rep;
    using period = duration::period;
    using time_point = std::chrono::time_point<UnixClock, duration>;

    static constexpr bool is_steady = false;

    static time_point now() noexcept;

    static std::time_t to_time_t(const time_point& tp) noexcept
    {
        using namespace std::chrono;
        return duration_cast<seconds>(tp.time_since_epoch()).count();
    }

    static time_point from_time_t(std::time_t t) noexcept
    {
        using namespace std::chrono;
        using FromPoint = std::chrono::time_point<UnixClock, seconds>;
        return time_point_cast<UnixClock::duration>(FromPoint{seconds{t}});
    }
};

using Time = UnixClock::time_point;
using Millis = std::chrono::milliseconds;
using Micros = std::chrono::microseconds;
using Nanos = std::chrono::nanoseconds;

constexpr bool isZero(Time time) noexcept
{
    return time == Time{};
}

template <typename RepT, typename PeriodT>
constexpr Time toTime(std::chrono::duration<RepT, PeriodT> d) noexcept
{
    using namespace std::chrono;
    return Time{duration_cast<UnixClock::duration>(d)};
}

constexpr Time toTime(timeval tv) noexcept
{
    using namespace std::chrono;
    return toTime(seconds{tv.tv_sec} + microseconds{tv.tv_usec});
}

constexpr Time toTime(timespec ts) noexcept
{
    using namespace std::chrono;
    return toTime(seconds{ts.tv_sec} + nanoseconds{ts.tv_nsec});
}

template <typename DurationT>
constexpr DurationT timeSinceEpoch(Time time) noexcept
{
    using namespace std::chrono;
    const UnixClock::duration d{time.time_since_epoch()};
    return duration_cast<DurationT>(d);
}

constexpr std::int64_t msSinceEpoch(Time time) noexcept
{
    using namespace std::chrono;
    return timeSinceEpoch<milliseconds>(time).count();
}

constexpr std::int64_t usSinceEpoch(Time time) noexcept
{
    using namespace std::chrono;
    return timeSinceEpoch<microseconds>(time).count();
}

constexpr std::int64_t nsSinceEpoch(Time time) noexcept
{
    using namespace std::chrono;
    const nanoseconds ns{time.time_since_epoch()};
    return ns.count();
}

/**
 * Format time with microsecond precision.
 */
SWIRLY_API void formatTime(Time time, std::ostream& os);

SWIRLY_API std::string formatTime(Time time);

SWIRLY_API std::ostream& operator<<(std::ostream& os, Time time);

constexpr bool isZero(timeval tv) noexcept
{
    return tv.tv_sec == 0 && tv.tv_usec == 0;
}

inline void clear(timeval& tv) noexcept
{
    tv.tv_sec = tv.tv_usec = 0;
}

inline timeval operator+(timeval lhs, timeval rhs) noexcept
{
    timeval tv;
    tv.tv_sec = lhs.tv_sec + rhs.tv_sec;
    tv.tv_usec = lhs.tv_usec + rhs.tv_usec;
    if (tv.tv_usec >= 1'000'000) {
        ++tv.tv_sec;
        tv.tv_usec -= 1'000'000;
    }
    return tv;
}

inline timeval operator-(timeval lhs, timeval rhs) noexcept
{
    timeval tv;
    tv.tv_sec = lhs.tv_sec - rhs.tv_sec;
    tv.tv_usec = lhs.tv_usec - rhs.tv_usec;
    if (tv.tv_usec < 0) {
        --tv.tv_sec;
        tv.tv_usec += 1'000'000;
    }
    return tv;
}

constexpr bool isZero(timespec ts) noexcept
{
    return ts.tv_sec == 0 && ts.tv_nsec == 0;
}

inline void clear(timespec& ts) noexcept
{
    ts.tv_sec = ts.tv_nsec = 0;
}

inline timespec operator+(timespec lhs, timespec rhs) noexcept
{
    timespec ts;
    ts.tv_sec = lhs.tv_sec + rhs.tv_sec;
    ts.tv_nsec = lhs.tv_nsec + rhs.tv_nsec;
    if (ts.tv_nsec >= 1'000'000'000) {
        ++ts.tv_sec;
        ts.tv_nsec -= 1'000'000'000;
    }
    return ts;
}

inline timespec operator-(timespec lhs, timespec rhs) noexcept
{
    timespec ts;
    ts.tv_sec = lhs.tv_sec - rhs.tv_sec;
    ts.tv_nsec = lhs.tv_nsec - rhs.tv_nsec;
    if (ts.tv_nsec < 0) {
        --ts.tv_sec;
        ts.tv_nsec += 1'000'000'000;
    }
    return ts;
}

} // swirly

#endif // SWIRLY_UTIL_TIME_HPP
