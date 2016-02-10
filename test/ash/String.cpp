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
#include <swirly/ash/String.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {
constexpr string_view name = "string_view"_sv;
} // anonymous

BOOST_AUTO_TEST_SUITE(StringSuite)

BOOST_AUTO_TEST_CASE(EmptyCase)
{
  StringBuf<3> sb;
  BOOST_CHECK(sb.empty());
  BOOST_CHECK_EQUAL(sb.size(), 0UL);
  BOOST_CHECK_EQUAL(sb.compare(""), 0);
  BOOST_CHECK_EQUAL(sb, StringBuf<3>{""});
}

BOOST_AUTO_TEST_CASE(NonEmptyCase)
{
  StringBuf<3> sb{"Foo"};
  BOOST_CHECK(!sb.empty());
  BOOST_CHECK_EQUAL(sb.size(), 3UL);
  BOOST_CHECK_EQUAL(sb.compare("Foo"), 0);
  BOOST_CHECK_EQUAL(sb, StringBuf<3>{"Foo"});
}

BOOST_AUTO_TEST_CASE(UpperBoundCase)
{
  StringBuf<3> sb{"Foox"};
  BOOST_CHECK_EQUAL(sb.size(), 3UL);
  BOOST_CHECK_EQUAL(sb, "Foo");

  sb = "Barx";
  BOOST_CHECK_EQUAL(sb.size(), 3UL);
  BOOST_CHECK_EQUAL(sb, "Bar");
}

BOOST_AUTO_TEST_SUITE_END()
