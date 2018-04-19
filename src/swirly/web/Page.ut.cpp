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
#include "Page.hpp"

#include <swirly/util/String.hpp>

#include <boost/test/unit_test.hpp>

#include <tuple>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(PageSuite)

BOOST_AUTO_TEST_CASE(ParseQueryNoneCase)
{
    auto page = parseQuery(""sv);
    BOOST_TEST(page.offset == 0U);
    BOOST_TEST(!page.limit);
}

BOOST_AUTO_TEST_CASE(ParseQueryOffsetCase)
{
    auto page = parseQuery("offset=1"sv);
    BOOST_TEST(page.offset == 1U);
    BOOST_TEST(!page.limit);
}

BOOST_AUTO_TEST_CASE(ParseQueryLimitCase)
{
    auto page = parseQuery("limit=2"sv);
    BOOST_TEST(page.offset == 0U);
    BOOST_TEST(!!page.limit);
    BOOST_TEST(*page.limit == 2U);
}

BOOST_AUTO_TEST_CASE(ParseQueryOffsetAndLimitCase)
{
    auto page = parseQuery("offset=1&limit=2"sv);
    BOOST_TEST(page.offset == 1U);
    BOOST_TEST(!!page.limit);
    BOOST_TEST(*page.limit == 2U);
}

BOOST_AUTO_TEST_CASE(ParseQueryLimitAndOffsetCase)
{
    auto page = parseQuery("limit=2&offset=1"sv);
    BOOST_TEST(page.offset == 1U);
    BOOST_TEST(!!page.limit);
    BOOST_TEST(*page.limit == 2U);
}

BOOST_AUTO_TEST_CASE(ParseQueryOtherCase)
{
    auto page = parseQuery("foo=1&bar=2"sv);
    BOOST_TEST(page.offset == 0U);
    BOOST_TEST(!page.limit);
}

BOOST_AUTO_TEST_SUITE_END()
