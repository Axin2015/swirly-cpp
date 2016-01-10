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
#include <swirly/elm/Level.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(LevelSuite)

BOOST_AUTO_TEST_CASE(LevelSetCase)
{
    const Order order{"MARAYL", "EURUSD", "EURUSD", 0, 0, "", 0, State::NEW, Side::BUY,
            10, 12345, 0, 0, 0, 0, 0, 0, false, 0, 0};

    LevelSet s;

    Level& level1{s.emplace(order)};
    BOOST_CHECK_EQUAL(level1.key(), -12345);
    BOOST_CHECK(s.find(Side::BUY, 12345) != s.end());

    // Duplicate.
    Level& level2{s.emplace(order)};
    BOOST_CHECK_EQUAL(&level2, &level1);

    // Replace.
    Level& level3{s.emplaceOrReplace(order)};
    BOOST_CHECK_NE(&level3, &level1);
    BOOST_CHECK_EQUAL(level3.key(), -12345);
}

BOOST_AUTO_TEST_SUITE_END()
