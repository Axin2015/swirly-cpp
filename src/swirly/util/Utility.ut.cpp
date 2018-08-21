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

BOOST_AUTO_TEST_CASE(Stoi16Case)
{
    BOOST_TEST(ston<int16_t>(""sv) == 0);
    BOOST_TEST(ston<int16_t>("1"sv) == 1);
    BOOST_TEST(ston<int16_t>("123"sv) == 123);
    BOOST_TEST(ston<int16_t>(" "sv) == 0);
    BOOST_TEST(ston<int16_t>("x"sv) == 0);
    BOOST_TEST(ston<int16_t>("x1 "sv) == 0);
    BOOST_TEST(ston<int16_t>("x123 "sv) == 0);
    BOOST_TEST(ston<int16_t>("32767"sv) == 0x7fff);
    BOOST_TEST(ston<int16_t>("-32768"sv) == -0x8000);
}

BOOST_AUTO_TEST_CASE(Stoi32Case)
{
    BOOST_TEST(ston<int32_t>(""sv) == 0);
    BOOST_TEST(ston<int32_t>("1"sv) == 1);
    BOOST_TEST(ston<int32_t>("123"sv) == 123);
    BOOST_TEST(ston<int32_t>(" "sv) == 0);
    BOOST_TEST(ston<int32_t>("x"sv) == 0);
    BOOST_TEST(ston<int32_t>("x1 "sv) == 0);
    BOOST_TEST(ston<int32_t>("x123 "sv) == 0);
    BOOST_TEST(ston<int32_t>("2147483647"sv) == 0x7fffffff);
    BOOST_TEST(ston<int32_t>("-2147483648"sv) == -0x80000000);
}

BOOST_AUTO_TEST_CASE(Stoi32NegSignCase)
{
    BOOST_TEST(ston<int32_t>("-"sv) == 0);
    BOOST_TEST(ston<int32_t>("-1"sv) == -1);
    BOOST_TEST(ston<int32_t>("-123"sv) == -123);
    BOOST_TEST(ston<int32_t>("-x"sv) == 0);
    BOOST_TEST(ston<int32_t>("x-1 "sv) == 0);
    BOOST_TEST(ston<int32_t>("x-123 "sv) == 0);
}

BOOST_AUTO_TEST_CASE(Stoi64Case)
{
    BOOST_TEST(ston<int64_t>(""sv) == 0L);
    BOOST_TEST(ston<int64_t>("1"sv) == 1L);
    BOOST_TEST(ston<int64_t>("123"sv) == 123L);
    BOOST_TEST(ston<int64_t>(" "sv) == 0L);
    BOOST_TEST(ston<int64_t>("x"sv) == 0L);
    BOOST_TEST(ston<int64_t>("x1 "sv) == 0L);
    BOOST_TEST(ston<int64_t>("x123 "sv) == 0L);
    BOOST_TEST(ston<int64_t>("9223372036854775807"sv) == 0x7fffffffffffffffLL);
    BOOST_TEST(ston<int64_t>("-9223372036854775808"sv) == -0x8000000000000000LL);
}

BOOST_AUTO_TEST_CASE(Stou16Case)
{
    BOOST_TEST(ston<uint16_t>(""sv) == 0);
    BOOST_TEST(ston<uint16_t>("1"sv) == 1);
    BOOST_TEST(ston<uint16_t>("123"sv) == 123);
    BOOST_TEST(ston<uint16_t>(" "sv) == 0);
    BOOST_TEST(ston<uint16_t>("x"sv) == 0);
    BOOST_TEST(ston<uint16_t>("x1 "sv) == 0);
    BOOST_TEST(ston<uint16_t>("x123 "sv) == 0);
    BOOST_TEST(ston<uint16_t>("65535"sv) == 0xffff);
}

BOOST_AUTO_TEST_CASE(Stou32Case)
{
    BOOST_TEST(ston<uint32_t>(""sv) == 0U);
    BOOST_TEST(ston<uint32_t>("1"sv) == 1U);
    BOOST_TEST(ston<uint32_t>("123"sv) == 123U);
    BOOST_TEST(ston<uint32_t>(" "sv) == 0U);
    BOOST_TEST(ston<uint32_t>("x"sv) == 0U);
    BOOST_TEST(ston<uint32_t>("x1 "sv) == 0U);
    BOOST_TEST(ston<uint32_t>("x123 "sv) == 0U);
    BOOST_TEST(ston<uint32_t>("4294967295"sv) == 0xffffffffU);
}

BOOST_AUTO_TEST_CASE(Stou64Case)
{
    BOOST_TEST(ston<uint64_t>(""sv) == 0UL);
    BOOST_TEST(ston<uint64_t>("1"sv) == 1UL);
    BOOST_TEST(ston<uint64_t>("123"sv) == 123UL);
    BOOST_TEST(ston<uint64_t>(" "sv) == 0UL);
    BOOST_TEST(ston<uint64_t>("x"sv) == 0UL);
    BOOST_TEST(ston<uint64_t>("x1 "sv) == 0UL);
    BOOST_TEST(ston<uint64_t>("x123 "sv) == 0UL);
    BOOST_TEST(ston<uint64_t>("18446744073709551615"sv) == 0xffffffffffffffffULL);
}

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
