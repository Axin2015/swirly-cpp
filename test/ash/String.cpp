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

#include <swirly/tea/Test.hpp>

using namespace std;
using namespace swirly;

namespace {
constexpr string_view name = "string_view"_sv;
} // anonymous

SWIRLY_TEST_CASE(StringEmpty)
{
  String<3> sb;
  SWIRLY_CHECK(sb.empty());
  SWIRLY_CHECK(sb.size() == 0UL);
  SWIRLY_CHECK(sb.compare("") == 0);
  SWIRLY_CHECK(sb == String<3>{""});
}

SWIRLY_TEST_CASE(StringNonEmpty)
{
  String<3> sb{"Foo"};
  SWIRLY_CHECK(!sb.empty());
  SWIRLY_CHECK(sb.size() == 3UL);
  SWIRLY_CHECK(sb.compare("Foo") == 0);
  SWIRLY_CHECK(sb == String<3>{"Foo"});
}

SWIRLY_TEST_CASE(StringUpperBound)
{
  String<3> sb{"Foox"};
  SWIRLY_CHECK(sb.size() == 3UL);
  SWIRLY_CHECK(sb == "Foo");

  sb = "Barx";
  SWIRLY_CHECK(sb.size() == 3UL);
  SWIRLY_CHECK(sb == "Bar");
}

SWIRLY_TEST_CASE(StringClear)
{
  String<3> sb{"Foo"};
  sb.clear();
  SWIRLY_CHECK(sb.empty());
}

SWIRLY_TEST_CASE(StringAssign)
{
  String<3> sb;
  sb = "Foo"_sv;
  SWIRLY_CHECK(sb.size() == 3UL);
  SWIRLY_CHECK(sb == "Foo");

  sb.clear();
  sb = "Foox"_sv;
  SWIRLY_CHECK(sb.size() == 3UL);
  SWIRLY_CHECK(sb == "Foo");
}

SWIRLY_TEST_CASE(StringToString)
{
  SWIRLY_CHECK(toString("foo"_sv) == "foo"s);
  SWIRLY_CHECK(stoi(toString(12345)) == 12345);
  SWIRLY_CHECK(stod(toString(12345.67)) == 12345.67);
}

SWIRLY_TEST_CASE(Stou64)
{
  SWIRLY_CHECK(stou64(""_sv) == 0UL);
  SWIRLY_CHECK(stou64("1"_sv) == 1UL);
  SWIRLY_CHECK(stou64("123"_sv) == 123UL);
  SWIRLY_CHECK(stou64(" "_sv) == 0UL);
  SWIRLY_CHECK(stou64(" 1 "_sv) == 1UL);
  SWIRLY_CHECK(stou64(" 123 "_sv) == 123UL);
  SWIRLY_CHECK(stou64("x"_sv) == 0UL);
  SWIRLY_CHECK(stou64(" 1x"_sv) == 1UL);
  SWIRLY_CHECK(stou64(" 123x"_sv) == 123UL);
  SWIRLY_CHECK(stou64("x1 "_sv) == 0UL);
  SWIRLY_CHECK(stou64("x123 "_sv) == 0UL);
  SWIRLY_CHECK(stou64("18446744073709551615"_sv) == 18446744073709551615ULL);
}
