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
#include "Utility.hpp"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(UtilitySuite)

BOOST_AUTO_TEST_CASE(HexDigitsCase)
{
    BOOST_TEST(hex_digits(0x1) == 1);
    BOOST_TEST(hex_digits(0xf) == 1);
    BOOST_TEST(hex_digits(0x10) == 2);
    BOOST_TEST(hex_digits(0xff) == 2);

    BOOST_TEST(hex_digits(0xcdef) == 4);
    BOOST_TEST(hex_digits(0x10000) == 5);
    BOOST_TEST(hex_digits(0x89abcdef) == 8);
    BOOST_TEST(hex_digits(0x100000000) == 9);
    BOOST_TEST(hex_digits(0x567890abcdef) == 12);
    BOOST_TEST(hex_digits(0x1000000000000) == 13);
    BOOST_TEST(hex_digits(0x1234567890abcdef) == 16);
}

BOOST_AUTO_TEST_SUITE_END()
