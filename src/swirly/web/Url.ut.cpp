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
#include "Url.hpp"

#include <swirly/util/String.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(UrlSuite)

BOOST_AUTO_TEST_CASE(UrlCase)
{
    Url url{"http://www.swirlycloud.com:8080/accnt/execs?offset=3&limit5"s};
    BOOST_TEST(url.schema() == "http"s);
    BOOST_TEST(url.host() == "www.swirlycloud.com"s);
    BOOST_TEST(url.port() == "8080"s);
    BOOST_TEST(url.path() == "/accnt/execs"s);
    BOOST_TEST(url.query() == "offset=3&limit5"s);
    BOOST_TEST(url.fragment().empty());
    BOOST_TEST(url.userInfo().empty());
}

BOOST_AUTO_TEST_CASE(UrlViewCase)
{
    const auto sv = "http://www.swirlycloud.com:8080/accnt/execs?offset=3&limit5"sv;
    UrlView url{sv};
    BOOST_TEST(url.schema() == "http"sv);
    BOOST_TEST(url.host() == "www.swirlycloud.com"sv);
    BOOST_TEST(url.port() == "8080"sv);
    BOOST_TEST(url.path() == "/accnt/execs"sv);
    BOOST_TEST(url.query() == "offset=3&limit5"sv);
    BOOST_TEST(url.fragment().empty());
    BOOST_TEST(url.userInfo().empty());
}

BOOST_AUTO_TEST_SUITE_END()
