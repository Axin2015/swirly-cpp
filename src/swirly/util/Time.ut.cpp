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
#include "Time.hpp"

#include <boost/test/unit_test.hpp>

namespace std::chrono {
template <typename RepT, typename PeriodT>
ostream& operator<<(ostream& os, duration<RepT, PeriodT> d)
{
    return os << d.count();
}
} // namespace std::chrono

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(TimeSuite)

BOOST_AUTO_TEST_CASE(TimeParseNanosCase)
{
    BOOST_TEST(parse_nanos("") == 0ns);
    BOOST_TEST(parse_nanos("0") == 0ns);
    BOOST_TEST(parse_nanos("1") == 100ms);

    BOOST_TEST(parse_nanos("000000001") == 1ns);
    BOOST_TEST(parse_nanos("000000012") == 12ns);
    BOOST_TEST(parse_nanos("000000123") == 123ns);

    BOOST_TEST(parse_nanos("000001") == 1us);
    BOOST_TEST(parse_nanos("000012") == 12us);
    BOOST_TEST(parse_nanos("000123") == 123us);

    BOOST_TEST(parse_nanos("001") == 1ms);
    BOOST_TEST(parse_nanos("012") == 12ms);
    BOOST_TEST(parse_nanos("123") == 123ms);

    BOOST_TEST(parse_nanos("123456789999") == 123456789ns);
}

BOOST_AUTO_TEST_CASE(TimeParseTimeOnlyCase)
{
    BOOST_TEST(!parse_time_only(""sv).valid);
    BOOST_TEST(!parse_time_only("12:34:5"sv).valid);
    BOOST_TEST(!parse_time_only("x2:34:56"sv).valid);
    BOOST_TEST(!parse_time_only("1x:34:56"sv).valid);
    BOOST_TEST(!parse_time_only("12x34:56"sv).valid);
    BOOST_TEST(!parse_time_only("12:x4:56"sv).valid);
    BOOST_TEST(!parse_time_only("12:3x:56"sv).valid);
    BOOST_TEST(!parse_time_only("12:34x56"sv).valid);
    BOOST_TEST(!parse_time_only("12:34:x6"sv).valid);
    BOOST_TEST(!parse_time_only("12:34:5x"sv).valid);

    BOOST_TEST(parse_time_only("00:00:00"sv).value == 0ns);
    BOOST_TEST(parse_time_only("12:00:00"sv).value == 12h);
    BOOST_TEST(parse_time_only("00:34:00"sv).value == 34min);
    BOOST_TEST(parse_time_only("00:00:56"sv).value == 56s);

    BOOST_TEST(parse_time_only("00:00:56."sv).value == 56s);
    BOOST_TEST(parse_time_only("00:00:00.789"sv).value == 789ms);
    BOOST_TEST(parse_time_only("00:00:00.000789"sv).value == 789us);
    BOOST_TEST(parse_time_only("00:00:00.000000789"sv).value == 789ns);
}

BOOST_AUTO_TEST_SUITE_END()
