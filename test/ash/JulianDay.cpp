/*
 * Swirly Order-Book and Matching-Engine.
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
#include <swirly/ash/JulianDay.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(JulianDaySuite)

BOOST_AUTO_TEST_CASE(YmdToIsoCase)
{
  BOOST_CHECK_EQUAL(20140314_dt, ymdToIso(2014, 2, 14));
}

BOOST_AUTO_TEST_CASE(YmdToJdCase)
{
  BOOST_CHECK_EQUAL(2456731_jd, ymdToJd(2014, 2, 14));
  // AD 1978 January 1, 0h UT is JD 2443509.5 and AD 1978 July 21, 15h UT, is JD 2443711.125.
  BOOST_CHECK_EQUAL(2443510_jd, ymdToJd(1978, 0, 1));
  BOOST_CHECK_EQUAL(2443711_jd, ymdToJd(1978, 6, 21));
}

BOOST_AUTO_TEST_CASE(JdToMillisCase)
{
  BOOST_CHECK_EQUAL(1394798400000_ms, jdToMs(ymdToJd(2014, 2, 14)));
}

BOOST_AUTO_TEST_SUITE_END()
