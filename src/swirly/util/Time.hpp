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
#ifndef SWIRLY_UTIL_TIME_HPP
#define SWIRLY_UTIL_TIME_HPP

#include <swirly/util/TypeTraits.hpp>

#include <boost/io/ios_state.hpp>

#include <chrono>
#include <iomanip>
#include <iosfwd>

#include <sys/time.h>

namespace swirly {
using namespace std::literals::chrono_literals;
inline namespace util {

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

using Duration = UnixClock::duration;
using Time = UnixClock::time_point;
using Seconds = std::chrono::seconds;
using Millis = std::chrono::milliseconds;
using Micros = std::chrono::microseconds;
using Nanos = std::chrono::nanoseconds;

SWIRLY_API Time get_time() noexcept;
SWIRLY_API std::ostream& operator<<(std::ostream& os, Time time);

inline UnixClock::time_point UnixClock::now() noexcept
{
    return get_time();
}

constexpr bool is_zero(Time time) noexcept
{
    return time == Time{};
}

template <typename RepT, typename PeriodT>
constexpr Time to_time(std::chrono::duration<RepT, PeriodT> d) noexcept
{
    using namespace std::chrono;
    return Time{duration_cast<Duration>(d)};
}

constexpr Time to_time(timeval tv) noexcept
{
    using namespace std::chrono;
    return to_time(seconds{tv.tv_sec} + microseconds{tv.tv_usec});
}

constexpr Time to_time(timespec ts) noexcept
{
    using namespace std::chrono;
    return to_time(seconds{ts.tv_sec} + nanoseconds{ts.tv_nsec});
}

template <typename DurationT>
constexpr DurationT time_since_epoch(Time time) noexcept
{
    using namespace std::chrono;
    const Duration d{time.time_since_epoch()};
    return duration_cast<DurationT>(d);
}

constexpr std::int64_t ms_since_epoch(Time time) noexcept
{
    using namespace std::chrono;
    return time_since_epoch<milliseconds>(time).count();
}

constexpr std::int64_t us_since_epoch(Time time) noexcept
{
    using namespace std::chrono;
    return time_since_epoch<microseconds>(time).count();
}

constexpr std::int64_t ns_since_epoch(Time time) noexcept
{
    using namespace std::chrono;
    const nanoseconds ns{time.time_since_epoch()};
    return ns.count();
}

template <typename DurationT>
struct PutTime {
    Time time;
    const char* fmt;
};

template <typename DurationT = Seconds>
auto put_time(Time time, const char* fmt)
{
    return PutTime<DurationT>{time, fmt};
}

template <typename DurationT>
std::ostream& operator<<(std::ostream& os, PutTime<DurationT> pt)
{
    const auto t = UnixClock::to_time_t(pt.time);
    struct tm gmt;
    os << std::put_time(gmtime_r(&t, &gmt), pt.fmt);

    if constexpr (std::is_same_v<DurationT, Nanos>) {
        const auto ns = ns_since_epoch(pt.time);
        boost::io::ios_fill_saver ifs{os};
        boost::io::ios_width_saver iws{os};
        os << '.' << std::setfill('0') << std::setw(9) << (ns % 1'000'000'000L);
    } else if constexpr (std::is_same_v<DurationT, Micros>) {
        const auto us = us_since_epoch(pt.time);
        boost::io::ios_fill_saver ifs{os};
        boost::io::ios_width_saver iws{os};
        os << '.' << std::setfill('0') << std::setw(6) << (us % 1'000'000L);
    } else if constexpr (std::is_same_v<DurationT, Millis>) {
        const auto ms = ms_since_epoch(pt.time);
        boost::io::ios_fill_saver ifs{os};
        boost::io::ios_width_saver iws{os};
        os << '.' << std::setfill('0') << std::setw(3) << (ms % 1'000L);
    } else if constexpr (std::is_same_v<DurationT, Seconds>) {
    } else {
        static_assert(DependentFalse<DurationT>::value);
    }
    return os;
}

constexpr bool is_zero(timeval tv) noexcept
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

constexpr bool is_zero(timespec ts) noexcept
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

template <>
struct TypeTraits<Time> {
    static auto from_string(std::string_view sv) noexcept { return to_time(Millis{stoi64(sv)}); }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s});
    }
};

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_TIME_HPP
