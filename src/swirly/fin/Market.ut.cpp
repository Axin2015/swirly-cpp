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
#include <swirly/util/String.hpp>

#include <boost/test/unit_test.hpp>

using namespace swirly;

BOOST_AUTO_TEST_SUITE(MarketSuite)

BOOST_AUTO_TEST_CASE(MarketToStringCase)
{
    Market market{1_id64, "EURUSD"sv, ymd_to_jd(2014, 3, 14), 0x01};

    BOOST_TEST(to_string(market) == //
               "{\"id\":1"
               ",\"instr\":\"EURUSD\""
               ",\"settl_date\":20140314"
               ",\"state\":1"
               ",\"last_lots\":null"
               ",\"last_ticks\":null"
               ",\"last_time\":null"
               ",\"bid_ticks\":[null,null,null]"
               ",\"bid_lots\":[null,null,null]"
               ",\"bid_count\":[null,null,null]"
               ",\"offer_ticks\":[null,null,null]"
               ",\"offer_lots\":[null,null,null]"
               ",\"offer_count\":[null,null,null]"
               "}");
}

BOOST_AUTO_TEST_CASE(MarketToStringNullCase)
{
    Market market{1_id64, "EURUSD"sv, 0_jd, 0x01};

    BOOST_TEST(to_string(market) == //
               "{\"id\":1"
               ",\"instr\":\"EURUSD\""
               ",\"settl_date\":null"
               ",\"state\":1"
               ",\"last_lots\":null"
               ",\"last_ticks\":null"
               ",\"last_time\":null"
               ",\"bid_ticks\":[null,null,null]"
               ",\"bid_lots\":[null,null,null]"
               ",\"bid_count\":[null,null,null]"
               ",\"offer_ticks\":[null,null,null]"
               ",\"offer_lots\":[null,null,null]"
               ",\"offer_count\":[null,null,null]"
               "}");
}

BOOST_AUTO_TEST_SUITE_END()
