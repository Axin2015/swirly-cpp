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

#include <swirly/unit/Test.hpp>

using namespace swirly;

SWIRLY_TEST_CASE(MarketToString)
{
    Market market{1_id64, "EURUSD"_sv, ymdToJd(2014, 2, 14), 0x01};

    SWIRLY_CHECK(toString(market) == //
                 "{\"id\":1"
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

SWIRLY_TEST_CASE(MarketToStringNull)
{
    Market market{1_id64, "EURUSD"_sv, 0_jd, 0x01};

    SWIRLY_CHECK(toString(market) == //
                 "{\"id\":1"
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
