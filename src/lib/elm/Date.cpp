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
#include <swirly/elm/Date.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#pragma GCC diagnostic pop

namespace lt = boost::local_time;
namespace pt = boost::posix_time;

using namespace std;

namespace swirly {

namespace {

// http://www.di-mgt.com.au/wclock/tz.html

// America/New_York.
const lt::time_zone_ptr newYork{new lt::posix_time_zone{"EST-5EDT,M3.2.0/2,M11.1.0/2"}};

// Roll at 5pm.
constexpr int RollHour{17};

inline pt::ptime millisToPtime(Millis ms)
{
  return pt::from_time_t(unbox(ms) / 1000L) + pt::milliseconds(unbox(ms) % 1000L);
}

} // anonymous

Jday getBusDay(Millis ms)
{
  lt::local_date_time ldt{millisToPtime(ms), newYork};
  // Add 7 hours to 17.00 will roll the date.
  ldt += pt::hours(24 - RollHour);
  return box<Jday>(ldt.local_time().date().julian_day());
}

} // swirly
