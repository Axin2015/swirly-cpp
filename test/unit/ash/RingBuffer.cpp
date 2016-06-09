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

#include <string>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(RingBuffer)
{
  using StringRingBuffer = RingBuffer<string>;

  StringRingBuffer srb{4};
  SWIRLY_CHECK(srb.empty());
  SWIRLY_CHECK(!srb.full());
  SWIRLY_CHECK(srb.size() == 0);

  srb.write([](string& s) { s = "first"s; });
  SWIRLY_CHECK(!srb.empty());
  SWIRLY_CHECK(!srb.full());
  SWIRLY_CHECK(srb.size() == 1);

  srb.clear();
  SWIRLY_CHECK(srb.empty());
  SWIRLY_CHECK(!srb.full());
  SWIRLY_CHECK(srb.size() == 0);

  srb.write("first"s);
  srb.write("second"s);
  srb.write("third"s);
  srb.write("fourth"s);
  SWIRLY_CHECK(!srb.empty());
  SWIRLY_CHECK(srb.full());
  SWIRLY_CHECK(srb.size() == 4);

  srb.write("fifth"s);
  SWIRLY_CHECK(!srb.empty());
  SWIRLY_CHECK(srb.full());
  SWIRLY_CHECK(srb.size() == 4);

  string s;
  srb.read([&s](const string& ref) { s = ref; });
  SWIRLY_CHECK(s == "second");
  SWIRLY_CHECK(!srb.empty());
  SWIRLY_CHECK(!srb.full());
  SWIRLY_CHECK(srb.size() == 3);

  srb.read(s);
  SWIRLY_CHECK(s == "third");
  srb.read(s);
  SWIRLY_CHECK(s == "fourth");
  srb.read(s);
  SWIRLY_CHECK(s == "fifth");

  SWIRLY_CHECK(srb.empty());
  SWIRLY_CHECK(!srb.full());
  SWIRLY_CHECK(srb.size() == 0);
}
