/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#ifndef SWIRLY_ASH_TIME_HPP
#define SWIRLY_ASH_TIME_HPP

#include <swirly/util/Defs.hpp>

#include <chrono>
#include <iosfwd>

namespace swirly {

using namespace std::literals::chrono_literals;

struct SWIRLY_API UnixClock {
  using duration = std::chrono::milliseconds;
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

/**
 * Milliseconds since epoch to time.
 */
constexpr Time msToTime(std::int64_t ms) noexcept
{
  using std::chrono::milliseconds;
  return Time{milliseconds{ms}};
}

/**
 * Time to milliseconds since epoch.
 */
constexpr std::int64_t timeToMs(Time time) noexcept
{
  using std::chrono::milliseconds;
  const milliseconds ms{time.time_since_epoch()};
  return ms.count();
}

SWIRLY_API std::ostream& operator<<(std::ostream& os, Time time);

} // swirly

#endif // SWIRLY_ASH_TIME_HPP
