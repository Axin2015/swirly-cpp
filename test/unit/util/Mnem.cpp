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
#include <swirly/util/Mnem.hpp>

#include <swirly/unit/Test.hpp>

#include <swirly/util/String.hpp>

using namespace std;
using namespace swirly;

static_assert(Mnem{}.empty(), "default constructor not constexpr");
static_assert(Mnem{Mnem{}}.empty(), "copy constructor not constexpr");

SWIRLY_TEST_CASE(MnemEmpty)
{
  Mnem mnem;
  SWIRLY_CHECK(mnem.empty());
  SWIRLY_CHECK(mnem.size() == 0UL);
  SWIRLY_CHECK(mnem.compare(""_sv) == 0);
  SWIRLY_CHECK(mnem == Mnem{""_sv});
}

SWIRLY_TEST_CASE(MnemNonEmpty)
{
  Mnem mnem{"Foo"_sv};
  SWIRLY_CHECK(!mnem.empty());
  SWIRLY_CHECK(mnem.size() == 3UL);
  SWIRLY_CHECK(mnem.compare("Foo"_sv) == 0);
  SWIRLY_CHECK(mnem == Mnem{"Foo"_sv});
}

SWIRLY_TEST_CASE(MnemUpperBound)
{
  Mnem mnem{"0123456789ABCDEFx"_sv};
  SWIRLY_CHECK(mnem.size() == MaxMnem);
  SWIRLY_CHECK(mnem == "0123456789ABCDEF"_sv);

  mnem = "0123456789abcdefx"_sv;
  SWIRLY_CHECK(mnem.size() == MaxMnem);
  SWIRLY_CHECK(mnem == "0123456789abcdef"_sv);
}

SWIRLY_TEST_CASE(MnemIndex)
{
  Mnem mnem{"Bar"_sv};
  SWIRLY_CHECK(mnem[0] == 'B');
  SWIRLY_CHECK(mnem[1] == 'a');
  SWIRLY_CHECK(mnem[2] == 'r');
  SWIRLY_CHECK(mnem.front() == 'B');
  SWIRLY_CHECK(mnem.back() == 'r');
}

SWIRLY_TEST_CASE(MnemClear)
{
  Mnem mnem{"Foo"_sv};
  mnem.clear();
  SWIRLY_CHECK(mnem.empty());
}
