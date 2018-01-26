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

#include <swirly/unit/Test.hpp>

#include <array>
#include <sstream>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(StringStream)
{
    StringStream<7> ss;
    SWIRLY_CHECK(ss.empty());
    ss << "foo";
    SWIRLY_CHECK(ss.size() == 3UL);
    SWIRLY_CHECK(ss.str() == "foo");
    ss << ',' << "bar";
    SWIRLY_CHECK(ss.size() == 7UL);
    SWIRLY_CHECK(ss.str() == "foo,bar");

    ss.reset();
    SWIRLY_CHECK(ss.empty());
    ss << 12345678;
    SWIRLY_CHECK(ss.size() == 7UL);
    SWIRLY_CHECK(ss.str() == "1234567");
    SWIRLY_CHECK(!ss);

    ss.reset();
    SWIRLY_CHECK(ss);
    SWIRLY_CHECK((ss << "test").str() == "test");
}

SWIRLY_TEST_CASE(OStreamJoiner)
{
    array<string, 3> arr{{"foo", "bar", "baz"}};
    stringstream ss;
    copy(arr.begin(), arr.end(), OStreamJoiner(ss, ','));
    SWIRLY_CHECK(ss.str() == "foo,bar,baz");
}
