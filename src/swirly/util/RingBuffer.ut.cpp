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
#include "RingBuffer.hpp"

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(RingBufferSuite)

BOOST_AUTO_TEST_CASE(RingBufferCase)
{
    using IntRingBuffer = RingBuffer<int>;

    IntRingBuffer rb{4};
    BOOST_TEST(rb.empty());
    BOOST_TEST(!rb.full());
    BOOST_TEST(rb.size() == 0);

    rb.post([](int& val) { val = 1; });
    BOOST_TEST(!rb.empty());
    BOOST_TEST(!rb.full());
    BOOST_TEST(rb.size() == 1);

    rb.clear();
    BOOST_TEST(rb.empty());
    BOOST_TEST(!rb.full());
    BOOST_TEST(rb.size() == 0);

    rb.push(1);
    rb.push(2);
    rb.push(3);
    rb.push(4);
    BOOST_TEST(!rb.empty());
    BOOST_TEST(rb.full());
    BOOST_TEST(rb.size() == 4);

    rb.push(5);
    BOOST_TEST(!rb.empty());
    BOOST_TEST(rb.full());
    BOOST_TEST(rb.size() == 4);

    int val;
    rb.fetch([&val](const int& ref) { val = ref; });
    BOOST_TEST(2);
    BOOST_TEST(!rb.empty());
    BOOST_TEST(!rb.full());
    BOOST_TEST(rb.size() == 3);

    BOOST_TEST(rb.front() == 3);
    rb.pop();
    BOOST_TEST(rb.front() == 4);
    rb.pop();
    BOOST_TEST(rb.front() == 5);
    rb.pop();

    BOOST_TEST(rb.empty());
    BOOST_TEST(!rb.full());
    BOOST_TEST(rb.size() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
