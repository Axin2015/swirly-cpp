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
#include <swirly/elm/Posn.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(PosnSuite)

BOOST_AUTO_TEST_CASE(TraderPosnSetCase)
{
    TraderPosnSet s;

    PosnPtr posn1{s.emplace("MARAYL", "EURUSD", 1, 0, 0, 0, 0)};
    BOOST_CHECK_EQUAL(posn1->refs(), 2);
    BOOST_CHECK_EQUAL(posn1->contr(), "EURUSD");
    BOOST_CHECK_EQUAL(posn1->settlDay(), 1);
    BOOST_CHECK(s.find("EURUSD", 1) != s.end());

    // Duplicate.
    PosnPtr posn2{s.emplace("MARAYL", "EURUSD", 1, 0, 0, 0, 0)};
    BOOST_CHECK_EQUAL(posn2->refs(), 3);
    BOOST_CHECK_EQUAL(posn2, posn1);

    // Replace.
    PosnPtr posn3{s.emplaceOrReplace("MARAYL", "EURUSD", 1, 0, 0, 0, 0)};
    BOOST_CHECK_EQUAL(posn3->refs(), 2);
    BOOST_CHECK_NE(posn3, posn1);
    BOOST_CHECK_EQUAL(posn3->contr(), "EURUSD");
    BOOST_CHECK_EQUAL(posn3->settlDay(), 1);
}

BOOST_AUTO_TEST_SUITE_END()
