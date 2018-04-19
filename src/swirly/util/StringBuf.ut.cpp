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

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {
constexpr string_view Name = "string_view"sv;
} // namespace

BOOST_AUTO_TEST_SUITE(StringBufSuite)

BOOST_AUTO_TEST_CASE(StringBufEmptyCase)
{
    StringBuf<3> sb;
    BOOST_TEST(sb.empty());
    BOOST_TEST(sb.size() == 0U);
    BOOST_TEST(sb.compare(""sv) == 0);
    BOOST_TEST(sb == StringBuf<3>{""sv});
}

BOOST_AUTO_TEST_CASE(StringBufNonEmptyCase)
{
    StringBuf<3> sb{"Foo"sv};
    BOOST_TEST(!sb.empty());
    BOOST_TEST(sb.size() == 3U);
    BOOST_TEST(sb.compare("Foo"sv) == 0);
    BOOST_TEST(sb == StringBuf<3>{"Foo"sv});
}

BOOST_AUTO_TEST_CASE(StringBufUpperBoundCase)
{
    StringBuf<3> sb{"Foox"sv};
    BOOST_TEST(sb.size() == 3U);
    BOOST_TEST(sb == "Foo"sv);

    sb = "Barx"sv;
    BOOST_TEST(sb.size() == 3U);
    BOOST_TEST(sb == "Bar"sv);
}

BOOST_AUTO_TEST_CASE(StringBufClearCase)
{
    StringBuf<3> sb{"Foo"sv};
    sb.clear();
    BOOST_TEST(sb.empty());
}

BOOST_AUTO_TEST_CASE(StringBufAssignCase)
{
    StringBuf<3> sb;
    sb = "Foo"sv;
    BOOST_TEST(sb.size() == 3U);
    BOOST_TEST(sb == "Foo"sv);

    sb.clear();
    sb = "Foox"sv;
    BOOST_TEST(sb.size() == 3U);
    BOOST_TEST(sb == "Foo"sv);
}

BOOST_AUTO_TEST_CASE(StringBufAppendCase)
{
    StringBuf<6> sb;
    sb = "Foo"sv;
    sb += "Bar"sv;
    BOOST_TEST(sb.size() == 6U);
    BOOST_TEST(sb == "FooBar"sv);

    sb = "Foo"sv;
    sb += "Barx"sv;
    BOOST_TEST(sb.size() == 6U);
    BOOST_TEST(sb == "FooBar"sv);
}

BOOST_AUTO_TEST_SUITE_END()
