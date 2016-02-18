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

#include <boost/test/unit_test.hpp>

#include <array>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(StreamSuite)

BOOST_AUTO_TEST_CASE(StringBuilderCase)
{
  StringBuilder<7> sb;
  BOOST_CHECK(sb.empty());
  sb << "foo";
  BOOST_CHECK_EQUAL(sb.size(), 3UL);
  BOOST_CHECK_EQUAL(sb.str(), "foo");
  sb << ',' << "bar";
  BOOST_CHECK_EQUAL(sb.size(), 7UL);
  BOOST_CHECK_EQUAL(sb.str(), "foo,bar");

  sb.reset();
  BOOST_CHECK(sb.empty());
  sb << 12345678;
  BOOST_CHECK_EQUAL(sb.size(), 7UL);
  BOOST_CHECK_EQUAL(sb.str(), "1234567");
  BOOST_CHECK(!sb);

  sb.reset();
  BOOST_CHECK(sb);
  BOOST_CHECK_EQUAL((sb << "test").str(), "test");
}

BOOST_AUTO_TEST_CASE(OStreamJoinerCase)
{
  array<string, 3> arr{{"foo", "bar", "baz"}};
  stringstream ss;
  copy(arr.begin(), arr.end(), OStreamJoiner(ss, ','));
  BOOST_CHECK_EQUAL(ss.str(), "foo,bar,baz");
}

BOOST_AUTO_TEST_SUITE_END()
