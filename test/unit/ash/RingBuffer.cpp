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
#include <swirly/ash/RingBuffer.hpp>

#include <swirly/tea/Test.hpp>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(RingBuffer)
{
  using IntRingBuffer = RingBuffer<int>;

  IntRingBuffer rb{4};
  SWIRLY_CHECK(rb.empty());
  SWIRLY_CHECK(!rb.full());
  SWIRLY_CHECK(rb.size() == 0);

  rb.write([](int& val) { val = 1; });
  SWIRLY_CHECK(!rb.empty());
  SWIRLY_CHECK(!rb.full());
  SWIRLY_CHECK(rb.size() == 1);

  rb.clear();
  SWIRLY_CHECK(rb.empty());
  SWIRLY_CHECK(!rb.full());
  SWIRLY_CHECK(rb.size() == 0);

  rb.write(1);
  rb.write(2);
  rb.write(3);
  rb.write(4);
  SWIRLY_CHECK(!rb.empty());
  SWIRLY_CHECK(rb.full());
  SWIRLY_CHECK(rb.size() == 4);

  rb.write(5);
  SWIRLY_CHECK(!rb.empty());
  SWIRLY_CHECK(rb.full());
  SWIRLY_CHECK(rb.size() == 4);

  int val;
  rb.read([&val](const int& ref) { val = ref; });
  SWIRLY_CHECK(2);
  SWIRLY_CHECK(!rb.empty());
  SWIRLY_CHECK(!rb.full());
  SWIRLY_CHECK(rb.size() == 3);

  rb.read(val);
  SWIRLY_CHECK(val == 3);
  rb.read(val);
  SWIRLY_CHECK(val == 4);
  rb.read(val);
  SWIRLY_CHECK(val == 5);

  SWIRLY_CHECK(rb.empty());
  SWIRLY_CHECK(!rb.full());
  SWIRLY_CHECK(rb.size() == 0);
}
