/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#include <swirly/ws/Page.hpp>

#include <swirly/util/String.hpp>

#include <swirly/unit/Test.hpp>

#include <tuple>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(ParseQueryNone)
{
    auto page = parseQuery(""_sv);
    SWIRLY_CHECK(page.offset == 0);
    SWIRLY_CHECK(!page.limit);
}

SWIRLY_TEST_CASE(ParseQueryOffset)
{
    auto page = parseQuery("offset=1"_sv);
    SWIRLY_CHECK(page.offset == 1);
    SWIRLY_CHECK(!page.limit);
}

SWIRLY_TEST_CASE(ParseQueryLimit)
{
    auto page = parseQuery("limit=2"_sv);
    SWIRLY_CHECK(page.offset == 0);
    SWIRLY_CHECK(page.limit);
    SWIRLY_CHECK(*page.limit == 2);
}

SWIRLY_TEST_CASE(ParseQueryOffsetAndLimit)
{
    auto page = parseQuery("offset=1&limit=2"_sv);
    SWIRLY_CHECK(page.offset == 1);
    SWIRLY_CHECK(page.limit);
    SWIRLY_CHECK(*page.limit == 2);
}

SWIRLY_TEST_CASE(ParseQueryLimitAndOffset)
{
    auto page = parseQuery("limit=2&offset=1"_sv);
    SWIRLY_CHECK(page.offset == 1);
    SWIRLY_CHECK(page.limit);
    SWIRLY_CHECK(*page.limit == 2);
}

SWIRLY_TEST_CASE(ParseQueryOther)
{
    auto page = parseQuery("foo=1&bar=2"_sv);
    SWIRLY_CHECK(page.offset == 0);
    SWIRLY_CHECK(!page.limit);
}
