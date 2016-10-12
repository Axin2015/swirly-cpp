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
#include <swirly/ash/Time.hpp>

#include <iostream>

#include <sys/time.h>

namespace swirly {

UnixClock::time_point UnixClock::now() noexcept
{
  using std::chrono::milliseconds;
  timeval tv;
  gettimeofday(&tv, nullptr);
  std::int64_t ms{tv.tv_sec * 1000L};
  ms += (tv.tv_usec + 500L) / 1000L;
  return time_point{milliseconds{ms}};
}

std::ostream& operator<<(std::ostream& os, Time time)
{
  return os << timeToMs(time);
}

} // swirly
