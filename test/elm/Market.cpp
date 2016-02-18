/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#include <swirly/elm/Market.hpp>

#include <swirly/ash/JulianDay.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

static_assert(sizeof(Market) <= 3 * 64, "crossed cache-line boundary");

BOOST_AUTO_TEST_SUITE(MarketSuite)

BOOST_AUTO_TEST_CASE(JsonCase)
{
  Market market{"EURUSD.MAR14"_sv,    "EURUSD March 14"_sv, "EURUSD"_sv,
                ymdToJd(2014, 2, 14), ymdToJd(2014, 2, 12), 0x01};

  BOOST_CHECK_EQUAL(toString(market), //
                    "{\"mnem\":\"EURUSD.MAR14\""
                    ",\"display\":\"EURUSD March 14\""
                    ",\"contr\":\"EURUSD\""
                    ",\"settlDate\":20140314"
                    ",\"expiryDate\":20140312"
                    ",\"state\":1"
                    "}");
}

BOOST_AUTO_TEST_CASE(JsonNullCase)
{
  Market market{"EURUSD.MAR14"_sv, "EURUSD March 14"_sv, "EURUSD"_sv, 0_jd, 0_jd, 0x01};

  BOOST_CHECK_EQUAL(toString(market), //
                    "{\"mnem\":\"EURUSD.MAR14\""
                    ",\"display\":\"EURUSD March 14\""
                    ",\"contr\":\"EURUSD\""
                    ",\"settlDate\":null"
                    ",\"expiryDate\":null"
                    ",\"state\":1"
                    "}");
}

BOOST_AUTO_TEST_SUITE_END()
