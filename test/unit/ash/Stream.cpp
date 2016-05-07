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
#include <swirly/ash/Stream.hpp>

#include <swirly/tea/Test.hpp>

#include <array>
#include <sstream>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(StringBuilder)
{
  StringBuilder<7> sb;
  SWIRLY_CHECK(sb.empty());
  sb << "foo";
  SWIRLY_CHECK(sb.size() == 3UL);
  SWIRLY_CHECK(sb.str() == "foo");
  sb << ',' << "bar";
  SWIRLY_CHECK(sb.size() == 7UL);
  SWIRLY_CHECK(sb.str() == "foo,bar");

  sb.reset();
  SWIRLY_CHECK(sb.empty());
  sb << 12345678;
  SWIRLY_CHECK(sb.size() == 7UL);
  SWIRLY_CHECK(sb.str() == "1234567");
  SWIRLY_CHECK(!sb);

  sb.reset();
  SWIRLY_CHECK(sb);
  SWIRLY_CHECK((sb << "test").str() == "test");
}

SWIRLY_TEST_CASE(OStreamJoiner)
{
  array<string, 3> arr{{"foo", "bar", "baz"}};
  stringstream ss;
  copy(arr.begin(), arr.end(), OStreamJoiner(ss, ','));
  SWIRLY_CHECK(ss.str() == "foo,bar,baz");
}
