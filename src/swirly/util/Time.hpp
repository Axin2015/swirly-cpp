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
#include <swirly/util/Types.hpp>

#include <boost/io/ios_state.hpp>

#include <chrono>
#include <iomanip>
#include <iosfwd>

#include <sys/time.h>

namespace swirly {
using namespace std::literals::chrono_literals;
using namespace std::literals::string_view_literals;
inline namespace util {

using Seconds = std::chrono::seconds;
using Millis = std::chrono::milliseconds;
using Micros = std::chrono::microseconds;
using Nanos = std::chrono::nanoseconds;
using Duration = Nanos;

SWIRLY_API Nanos get_time(clockid_t clock_id) noexcept;

struct MonoClock {
    using duration = Duration;
    using period = Duration::period;
    using rep = Duration::rep;
    using time_point = std::chrono::time_point<MonoClock, Duration>;

    static constexpr int clock_id{CLOCK_MONOTONIC};
    static constexpr bool is_steady{true};

    static constexpr time_point max() noexcept
    {
        using namespace std::chrono;
        using FromPoint = std::chrono::time_point<MonoClock, seconds>;
        constexpr seconds secs{std::numeric_limits<int>::max()};
        return time_point_cast<Duration>(FromPoint{secs});
    }

    static time_point now() noexcept { return time_point{get_time(clock_id)}; }
};

struct WallClock {
    using duration = Duration;
    using period = Duration::period;
    using rep = Duration::rep;
    using time_point = std::chrono::time_point<WallClock, Duration>;

    static constexpr int clock_id{CLOCK_REALTIME};
    static constexpr bool is_steady{false};

    static constexpr time_point max() noexcept
    {
        using namespace std::chrono;
        using FromPoint = std::chrono::time_point<WallClock, seconds>;
        constexpr seconds secs{std::numeric_limits<int>::max()};
        return time_point_cast<Duration>(FromPoint{secs});
    }

    static time_point now() noexcept { return time_point{get_time(clock_id)}; }

    static constexpr std::time_t to_time_t(const time_point& tp) noexcept
    {
        using namespace std::chrono;
        return duration_cast<seconds>(tp.time_since_epoch()).count();
    }

    static constexpr time_point from_time_t(std::time_t t) noexcept
    {
        using namespace std::chrono;
        using FromPoint = std::chrono::time_point<WallClock, seconds>;
        return time_point_cast<Duration>(FromPoint{seconds{t}});
    }
};

using MonoTime = MonoClock::time_point;
using WallTime = WallClock::time_point;

SWIRLY_API std::ostream& operator<<(std::ostream& os, WallTime t);

template <typename RepT, typename PeriodT>
constexpr bool is_zero(std::chrono::duration<RepT, PeriodT> d) noexcept
{
    return d == decltype(d){};
}

template <typename ClockT>
constexpr bool is_zero(std::chrono::time_point<ClockT, Duration> t) noexcept
{
    return t == decltype(t){};
}

template <typename ClockT, typename DurationT>
constexpr DurationT time_since_epoch(std::chrono::time_point<ClockT, Duration> t) noexcept
{
    using namespace std::chrono;
    const Duration d{t.time_since_epoch()};
    return duration_cast<DurationT>(d);
}

template <typename ClockT>
constexpr std::int64_t ms_since_epoch(std::chrono::time_point<ClockT, Duration> t) noexcept
{
    using namespace std::chrono;
    return time_since_epoch<ClockT, milliseconds>(t).count();
}

template <typename ClockT>
constexpr std::int64_t us_since_epoch(std::chrono::time_point<ClockT, Duration> t) noexcept
{
    using namespace std::chrono;
    return time_since_epoch<ClockT, microseconds>(t).count();
}

template <typename ClockT>
constexpr std::int64_t ns_since_epoch(std::chrono::time_point<ClockT, Duration> t) noexcept
{
    using namespace std::chrono;
    const nanoseconds ns{t.time_since_epoch()};
    return ns.count();
}

template <typename ClockT, typename RepT, typename PeriodT>
constexpr auto to_time(std::chrono::duration<RepT, PeriodT> d) noexcept
{
    using namespace std::chrono;
    return std::chrono::time_point<ClockT, Duration>{duration_cast<Duration>(d)};
}

template <typename ClockT>
constexpr auto to_time(timeval tv) noexcept
{
    using namespace std::chrono;
    return to_time<ClockT>(seconds{tv.tv_sec} + microseconds{tv.tv_usec});
}

template <typename ClockT>
constexpr auto to_time(timespec ts) noexcept
{
    using namespace std::chrono;
    return to_time<ClockT>(seconds{ts.tv_sec} + nanoseconds{ts.tv_nsec});
}

template <typename RepT, typename PeriodT>
constexpr timeval to_timeval(std::chrono::duration<RepT, PeriodT> d) noexcept
{
    using namespace std::chrono;
    const auto us = duration_cast<microseconds>(d).count();
    return {static_cast<time_t>(us / 1'000'000L), static_cast<suseconds_t>(us % 1'000'000L)};
}

template <typename ClockT>
constexpr timeval to_timeval(std::chrono::time_point<ClockT, Duration> t) noexcept
{
    using namespace std::chrono;
    return to_timeval(time_since_epoch<ClockT, microseconds>(t));
}

template <typename RepT, typename PeriodT>
constexpr timespec to_timespec(std::chrono::duration<RepT, PeriodT> d) noexcept
{
    using namespace std::chrono;
    const auto ns = duration_cast<nanoseconds>(d).count();
    return {static_cast<time_t>(ns / 1'000'000'000L), static_cast<long>(ns % 1'000'000'000L)};
}

template <typename ClockT>
constexpr timespec to_timespec(std::chrono::time_point<ClockT, Duration> t) noexcept
{
    using namespace std::chrono;
    return to_timespec(time_since_epoch<ClockT, nanoseconds>(t));
}

template <typename DurationT>
struct PutTime {
    WallTime time;
    const char* fmt;
};

template <typename DurationT = Seconds>
auto put_time(WallTime t, const char* fmt)
{
    return PutTime<DurationT>{t, fmt};
}

template <typename DurationT>
std::ostream& operator<<(std::ostream& os, PutTime<DurationT> pt)
{
    const auto t = WallClock::to_time_t(pt.time);
    struct tm gmt;
    os << std::put_time(gmtime_r(&t, &gmt), pt.fmt);

    if constexpr (std::is_same_v<DurationT, Nanos>) {
        const auto ns = ns_since_epoch<WallClock>(pt.time);
        boost::io::ios_fill_saver ifs{os};
        boost::io::ios_width_saver iws{os};
        os << '.' << std::setfill('0') << std::setw(9) << (ns % 1'000'000'000L);
    } else if constexpr (std::is_same_v<DurationT, Micros>) {
        const auto us = us_since_epoch<WallClock>(pt.time);
        boost::io::ios_fill_saver ifs{os};
        boost::io::ios_width_saver iws{os};
        os << '.' << std::setfill('0') << std::setw(6) << (us % 1'000'000L);
    } else if constexpr (std::is_same_v<DurationT, Millis>) {
        const auto ms = ms_since_epoch<WallClock>(pt.time);
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

template <typename RepT, typename PeriodT>
struct TypeTraits<std::chrono::duration<RepT, PeriodT>> {
    static auto from_string(std::string_view sv) noexcept
    {
        using namespace std::chrono;
        using Duration = duration<RepT, PeriodT>;
        using Rep = typename Duration::rep;
        return Duration{TypeTraits<Rep>::from_string(sv)};
    }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s});
    }
};

template <>
struct TypeTraits<WallTime> {
    static auto from_string(std::string_view sv) noexcept
    {
        return to_time<WallClock>(TypeTraits<Millis>::from_string(sv));
    }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s});
    }
};

/**
 * Parse fractional nanoseconds.
 * @param sv String of decimal digits.
 * @return nanoseconds.
 */
constexpr auto parse_nanos(std::string_view sv) noexcept
{
    // clang-format off
    constexpr int c[] = {
                  0,
        100'000'000,
         10'000'000,
          1'000'000,
            100'000,
             10'000,
              1'000,
                100,
                 10,
                  1
    };
    // clang-format on

    // Truncate to ensure that we process no more than 9 decimal places.
    sv = sv.substr(0, 9);
    auto it = sv.begin(), end = sv.end();

    int ns{0};
    if (isdigit(*it)) {
        ns = *it++ - '0';
        while (it != end && isdigit(*it)) {
            ns *= 10;
            ns += *it++ - '0';
        }
    }
    return Nanos{ns * c[it - sv.begin()]};
}
static_assert(parse_nanos("000000001") == 1ns);

/**
 * Time-only represented in UTC (Universal Time Coordinated, also known as "GMT") in either HH:MM:SS
 * (whole seconds) or HH:MM:SS.sss (milliseconds) format, colons, and period required. This
 * special-purpose field is paired with UTCDateOnly to form a proper UTCTimestamp for
 * bandwidth-sensitive messages. Valid values: HH = 00-23, MM = 00-59, SS = 00-5960 (60 only if UTC
 * leap second) (without milliseconds). HH = 00-23, MM = 00-59, SS = 00-5960 (60 only if UTC leap
 * second), sss=000-999 (indicating milliseconds).
 */
constexpr Result<Nanos> parse_time_only(std::string_view sv) noexcept
{
    using namespace std::chrono;

    // clang-format off
    if (sv.size() < 8
        || !isdigit(sv[0])
        || !isdigit(sv[1])
        || sv[2] != ':'
        || !isdigit(sv[3])
        || !isdigit(sv[4])
        || sv[5] != ':'
        || !isdigit(sv[6])
        || !isdigit(sv[7])) {
        // Invalid format.
        return {};
    }
    // clang-format on
    const hours h{(sv[0] - '0') * 10 + sv[1] - '0'};
    const minutes m{(sv[3] - '0') * 10 + sv[4] - '0'};
    const seconds s{(sv[6] - '0') * 10 + sv[7] - '0'};
    Nanos ns{h + m + s};
    if (sv.size() > 8) {
        if (sv[8] != '.') {
            // Invalid delimiter.
            return {ns, false};
        }
        ns += parse_nanos(sv.substr(9));
    }
    return {ns, true};
}
static_assert(parse_time_only("12:00:00"sv).value == 12h);

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_TIME_HPP
