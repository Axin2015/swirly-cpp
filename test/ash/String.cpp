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
  String<3> sb;
  BOOST_CHECK(sb.empty());
  BOOST_CHECK_EQUAL(sb.size(), 0UL);
  BOOST_CHECK_EQUAL(sb.compare(""), 0);
  BOOST_CHECK_EQUAL(sb, String<3>{""});
}

BOOST_AUTO_TEST_CASE(NonEmptyCase)
{
  String<3> sb{"Foo"};
  BOOST_CHECK(!sb.empty());
  BOOST_CHECK_EQUAL(sb.size(), 3UL);
  BOOST_CHECK_EQUAL(sb.compare("Foo"), 0);
  BOOST_CHECK_EQUAL(sb, String<3>{"Foo"});
}

BOOST_AUTO_TEST_CASE(UpperBoundCase)
{
  String<3> sb{"Foox"};
  BOOST_CHECK_EQUAL(sb.size(), 3UL);
  BOOST_CHECK_EQUAL(sb, "Foo");

  sb = "Barx";
  BOOST_CHECK_EQUAL(sb.size(), 3UL);
  BOOST_CHECK_EQUAL(sb, "Bar");
}

BOOST_AUTO_TEST_CASE(ClearCase)
{
  String<3> sb{"Foo"};
  sb.clear();
  BOOST_CHECK(sb.empty());
}

BOOST_AUTO_TEST_CASE(AssignCase)
{
  String<3> sb;
  sb = "Foo"_sv;
  BOOST_CHECK_EQUAL(sb.size(), 3UL);
  BOOST_CHECK_EQUAL(sb, "Foo");

  sb.clear();
  sb = "Foox"_sv;
  BOOST_CHECK_EQUAL(sb.size(), 3UL);
  BOOST_CHECK_EQUAL(sb, "Foo");
}

BOOST_AUTO_TEST_CASE(ToStringCase)
{
  BOOST_CHECK_EQUAL(toString("foo"_sv), "foo"s);
  BOOST_CHECK_EQUAL(stoi(toString(12345)), 12345);
  BOOST_CHECK_EQUAL(stod(toString(12345.67)), 12345.67);
}

BOOST_AUTO_TEST_CASE(StoulCase)
{
  BOOST_CHECK_EQUAL(stoul(""_sv), 0UL);
  BOOST_CHECK_EQUAL(stoul("1"_sv), 1UL);
  BOOST_CHECK_EQUAL(stoul("123"_sv), 123UL);
  BOOST_CHECK_EQUAL(stoul(" "_sv), 0UL);
  BOOST_CHECK_EQUAL(stoul(" 1 "_sv), 1UL);
  BOOST_CHECK_EQUAL(stoul(" 123 "_sv), 123UL);
  BOOST_CHECK_EQUAL(stoul("x"_sv), 0UL);
  BOOST_CHECK_EQUAL(stoul(" 1x"_sv), 1UL);
  BOOST_CHECK_EQUAL(stoul(" 123x"_sv), 123UL);
  BOOST_CHECK_EQUAL(stoul("x1 "_sv), 0UL);
  BOOST_CHECK_EQUAL(stoul("x123 "_sv), 0UL);
}

BOOST_AUTO_TEST_SUITE_END()
