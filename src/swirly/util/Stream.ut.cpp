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
#include "Stream.hpp"

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

#include <array>
#include <sstream>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(StreamSuite)

BOOST_AUTO_TEST_CASE(StaticStreamCase)
{
    StaticStream<7> ss;
    BOOST_TEST(ss.empty());
    ss << "foo"sv;
    BOOST_TEST(ss.size() == 3U);
    BOOST_TEST(ss.str() == "foo");
    ss << ',' << "bar"sv;
    BOOST_TEST(ss.size() == 7U);
    BOOST_TEST(ss.str() == "foo,bar");

    ss.reset();
    BOOST_TEST(ss.empty());
    ss << 12345678;
    BOOST_TEST(ss.size() == 7U);
    BOOST_TEST(ss.str() == "1234567");
    BOOST_TEST(!ss);

    ss.reset();
    BOOST_TEST(!!ss);
    BOOST_TEST((ss << "test"sv).str() == "test");
}

BOOST_AUTO_TEST_CASE(OStreamJoinerCase)
{
    array<string, 3> arr{{"foo", "bar", "baz"}};
    stringstream ss;
    copy(arr.begin(), arr.end(), OStreamJoiner{ss, ','});
    BOOST_TEST(ss.str() == "foo,bar,baz");
}

BOOST_AUTO_TEST_SUITE_END()
