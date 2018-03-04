/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
#include "StringBuf.hpp"

#include <swirly/unit/Test.hpp>

using namespace std;
using namespace swirly;

namespace {
constexpr string_view Name = "string_view"sv;
} // namespace

SWIRLY_TEST_CASE(StringBufEmpty)
{
    StringBuf<3> sb;
    SWIRLY_CHECK(sb.empty());
    SWIRLY_CHECK(sb.size() == 0UL);
    SWIRLY_CHECK(sb.compare(""sv) == 0);
    SWIRLY_CHECK(sb == StringBuf<3>{""sv});
}

SWIRLY_TEST_CASE(StringBufNonEmpty)
{
    StringBuf<3> sb{"Foo"sv};
    SWIRLY_CHECK(!sb.empty());
    SWIRLY_CHECK(sb.size() == 3UL);
    SWIRLY_CHECK(sb.compare("Foo"sv) == 0);
    SWIRLY_CHECK(sb == StringBuf<3>{"Foo"sv});
}

SWIRLY_TEST_CASE(StringBufUpperBound)
{
    StringBuf<3> sb{"Foox"sv};
    SWIRLY_CHECK(sb.size() == 3UL);
    SWIRLY_CHECK(sb == "Foo"sv);

    sb = "Barx"sv;
    SWIRLY_CHECK(sb.size() == 3UL);
    SWIRLY_CHECK(sb == "Bar"sv);
}

SWIRLY_TEST_CASE(StringBufClear)
{
    StringBuf<3> sb{"Foo"sv};
    sb.clear();
    SWIRLY_CHECK(sb.empty());
}

SWIRLY_TEST_CASE(StringBufAssign)
{
    StringBuf<3> sb;
    sb = "Foo"sv;
    SWIRLY_CHECK(sb.size() == 3UL);
    SWIRLY_CHECK(sb == "Foo"sv);

    sb.clear();
    sb = "Foox"sv;
    SWIRLY_CHECK(sb.size() == 3UL);
    SWIRLY_CHECK(sb == "Foo"sv);
}

SWIRLY_TEST_CASE(StringBufAppend)
{
    StringBuf<6> sb;
    sb = "Foo"sv;
    sb += "Bar"sv;
    SWIRLY_CHECK(sb.size() == 6UL);
    SWIRLY_CHECK(sb == "FooBar"sv);

    sb = "Foo"sv;
    sb += "Barx"sv;
    SWIRLY_CHECK(sb.size() == 6UL);
    SWIRLY_CHECK(sb == "FooBar"sv);
}
