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

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace utf = boost::unit_test;

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

BOOST_AUTO_TEST_CASE(VarAccumCase, *utf::tolerance(0.0000001))
{
    VarAccum v;
    v.append(1345, 1301, 1368, 1322, 1310, 1370, 1318, 1350, 1303, 1299);
    BOOST_TEST(v.size() == 10U);
    BOOST_TEST(v.mean() == 1328.6);
    BOOST_TEST(var(v) == 754.2666667);
    BOOST_TEST(varp(v) == 678.84);
    BOOST_TEST(stdev(v) == 27.4639157);
    BOOST_TEST(stdevp(v) == 26.0545581);
    BOOST_TEST(v.min() == 1299);
    BOOST_TEST(v.max() == 1370);
}

BOOST_AUTO_TEST_CASE(CeilCase)
{
    BOOST_TEST(ceil(1U, 3U) == 1U);
    BOOST_TEST(ceil(2U, 3U) == 1U);
    BOOST_TEST(ceil(3U, 3U) == 1U);
    BOOST_TEST(ceil(4U, 3U) == 2U);
    BOOST_TEST(ceil(5U, 3U) == 2U);
    BOOST_TEST(ceil(6U, 3U) == 2U);
}

BOOST_AUTO_TEST_SUITE_END()
