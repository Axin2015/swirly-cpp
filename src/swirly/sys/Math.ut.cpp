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
#include "Math.hpp"

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(MathSuite)

BOOST_AUTO_TEST_CASE(NextPow2Case)
{
    BOOST_TEST(nextPow2(1U) == 1U);
    BOOST_TEST(nextPow2(2U) == 2U);
    BOOST_TEST(nextPow2(3U) == 4U);
    BOOST_TEST(nextPow2(4U) == 4U);
    BOOST_TEST(nextPow2(5U) == 8U);
    BOOST_TEST(nextPow2(6U) == 8U);
    BOOST_TEST(nextPow2(7U) == 8U);
    BOOST_TEST(nextPow2(8U) == 8U);

    BOOST_TEST(nextPow2(1UL) == 1UL);
    BOOST_TEST(nextPow2(2UL) == 2UL);
    BOOST_TEST(nextPow2(3UL) == 4UL);
    BOOST_TEST(nextPow2(4UL) == 4UL);
    BOOST_TEST(nextPow2(5UL) == 8UL);
    BOOST_TEST(nextPow2(6UL) == 8UL);
    BOOST_TEST(nextPow2(7UL) == 8UL);
    BOOST_TEST(nextPow2(8UL) == 8UL);
}

BOOST_AUTO_TEST_SUITE_END()
