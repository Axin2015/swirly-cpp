/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
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
#include <swirly/ash/Array.hpp>

#include <boost/test/unit_test.hpp>

#include <numeric> // accumulate()

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(ArraySuite)

BOOST_AUTO_TEST_CASE(ArrayViewCase)
{
    BOOST_CHECK(!ArrayView<int>{}.data());
    BOOST_CHECK(ArrayView<int>{}.empty());
    BOOST_CHECK_EQUAL(ArrayView<int>{}.size(), 0UL);

    const int arr[] = { 101, 202, 303 };
    ArrayView<int> av{arr};

    BOOST_CHECK(av.data());
    BOOST_CHECK(!av.empty());
    BOOST_CHECK_EQUAL(av.size(), 3UL);

    BOOST_CHECK_EQUAL(av[0], arr[0]);
    BOOST_CHECK_EQUAL(av[1], arr[1]);
    BOOST_CHECK_EQUAL(av[2], arr[2]);

    BOOST_CHECK_EQUAL(av.front(), arr[0]);
    BOOST_CHECK_EQUAL(av.back(), arr[2]);

    BOOST_CHECK(equal(av.begin(), av.end(), arr));
    int rev[] = { 303, 202, 101 };

    BOOST_CHECK(equal(av.rbegin(), av.rend(), rev));

    BOOST_CHECK_EQUAL(makeArrayView(arr, 2).size(), 2UL);
    BOOST_CHECK_EQUAL(makeArrayView(arr).size(), 3UL);
}

BOOST_AUTO_TEST_SUITE_END()
