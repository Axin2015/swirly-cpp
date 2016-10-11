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
#include <swirly/ash/Date.hpp>

#include <swirly/tea/Test.hpp>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(YmdToIso)
{
  SWIRLY_CHECK(20140314_ymd == ymdToIso(2014, 2, 14));
}

SWIRLY_TEST_CASE(YmdToJd)
{
  SWIRLY_CHECK(2456731_jd == ymdToJd(2014, 2, 14));
  // AD 1978 January 1, 0h UT is JD 2443509.5 and AD 1978 July 21, 15h UT, is JD 2443711.125.
  SWIRLY_CHECK(2443510_jd == ymdToJd(1978, 0, 1));
  SWIRLY_CHECK(2443711_jd == ymdToJd(1978, 6, 21));
}

SWIRLY_TEST_CASE(JdToTime)
{
  SWIRLY_CHECK(1394798400000ms == jdToTime(ymdToJd(2014, 2, 14)).time_since_epoch());
}
