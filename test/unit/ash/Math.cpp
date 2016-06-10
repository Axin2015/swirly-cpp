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
#include <swirly/ash/Math.hpp>

#include <swirly/tea/Test.hpp>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(VarAccum)
{
  VarAccum v;
  v.append(1345, 1301, 1368, 1322, 1310, 1370, 1318, 1350, 1303, 1299);
  SWIRLY_CHECK(test::isSame(v.size(), 10));
  SWIRLY_CHECK(test::isSame(v.mean(), 1328.6));
  SWIRLY_CHECK(test::isSame(var(v), 754.2666667));
  SWIRLY_CHECK(test::isSame(varp(v), 678.84));
  SWIRLY_CHECK(test::isSame(stdev(v), 27.4639157));
  SWIRLY_CHECK(test::isSame(stdevp(v), 26.0545581));
  SWIRLY_CHECK(test::isSame(v.min(), 1299));
  SWIRLY_CHECK(test::isSame(v.max(), 1370));
}

SWIRLY_TEST_CASE(NextPow2)
{
  SWIRLY_CHECK(nextPow2(1U) == 1U);
  SWIRLY_CHECK(nextPow2(2U) == 2U);
  SWIRLY_CHECK(nextPow2(3U) == 4U);
  SWIRLY_CHECK(nextPow2(4U) == 4U);
  SWIRLY_CHECK(nextPow2(5U) == 8U);
  SWIRLY_CHECK(nextPow2(6U) == 8U);
  SWIRLY_CHECK(nextPow2(7U) == 8U);
  SWIRLY_CHECK(nextPow2(8U) == 8U);

  SWIRLY_CHECK(nextPow2(1UL) == 1UL);
  SWIRLY_CHECK(nextPow2(2UL) == 2UL);
  SWIRLY_CHECK(nextPow2(3UL) == 4UL);
  SWIRLY_CHECK(nextPow2(4UL) == 4UL);
  SWIRLY_CHECK(nextPow2(5UL) == 8UL);
  SWIRLY_CHECK(nextPow2(6UL) == 8UL);
  SWIRLY_CHECK(nextPow2(7UL) == 8UL);
  SWIRLY_CHECK(nextPow2(8UL) == 8UL);
}
