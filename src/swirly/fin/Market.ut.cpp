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
#include "Market.hpp"

#include <swirly/util/Date.hpp>

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

using namespace swirly;

BOOST_AUTO_TEST_SUITE(MarketSuite)

BOOST_AUTO_TEST_CASE(MarketToStringCase)
{
    Market market{1_id64, "SWIRLY"sv, "EURUSD"sv, ymdToJd(2014, 3, 14), 0x01};

    BOOST_TEST(toString(market) == //
               "{\"id\":1"
               ",\"broker\":\"SWIRLY\""
               ",\"instr\":\"EURUSD\""
               ",\"settlDate\":20140314"
               ",\"state\":1"
               ",\"lastLots\":null"
               ",\"lastTicks\":null"
               ",\"lastTime\":null"
               ",\"bidTicks\":[null,null,null]"
               ",\"bidLots\":[null,null,null]"
               ",\"bidCount\":[null,null,null]"
               ",\"offerTicks\":[null,null,null]"
               ",\"offerLots\":[null,null,null]"
               ",\"offerCount\":[null,null,null]"
               "}");
}

BOOST_AUTO_TEST_CASE(MarketToStringNullCase)
{
    Market market{1_id64, "SWIRLY"sv, "EURUSD"sv, 0_jd, 0x01};

    BOOST_TEST(toString(market) == //
               "{\"id\":1"
               ",\"broker\":\"SWIRLY\""
               ",\"instr\":\"EURUSD\""
               ",\"settlDate\":null"
               ",\"state\":1"
               ",\"lastLots\":null"
               ",\"lastTicks\":null"
               ",\"lastTime\":null"
               ",\"bidTicks\":[null,null,null]"
               ",\"bidLots\":[null,null,null]"
               ",\"bidCount\":[null,null,null]"
               ",\"offerTicks\":[null,null,null]"
               ",\"offerLots\":[null,null,null]"
               ",\"offerCount\":[null,null,null]"
               "}");
}

BOOST_AUTO_TEST_SUITE_END()
