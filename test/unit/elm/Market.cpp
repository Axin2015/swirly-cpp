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

#include <swirly/ash/Date.hpp>

#include <swirly/tea/Test.hpp>

#include <iostream>
using namespace std;
using namespace swirly;

static_assert(sizeof(Market) <= 4 * 64, "crossed cache-line boundary");

SWIRLY_TEST_CASE(toMarketId)
{
  const auto id = toMarketId(171_id32, 2492719_jd);
  SWIRLY_CHECK(id == 0xabcdef_id64);
}

SWIRLY_TEST_CASE(MarketToString)
{
  Market market{1_id64, "EURUSD"_sv, ymdToJd(2014, 2, 14), 0x01};

  cout << toString(market) << endl;
  SWIRLY_CHECK(toString(market) == //
               "{\"contr\":\"EURUSD\""
               ",\"settlDate\":20140314"
               ",\"state\":1"
               ",\"lastLots\":null"
               ",\"lastTicks\":null"
               ",\"lastTime\":null"
               ",\"bidTicks\":[null,null,null]"
               ",\"bidResd\":[null,null,null]"
               ",\"bidCount\":[null,null,null]"
               ",\"offerTicks\":[null,null,null]"
               ",\"offerResd\":[null,null,null]"
               ",\"offerCount\":[null,null,null]"
               "}");
}

SWIRLY_TEST_CASE(MarketToStringNull)
{
  Market market{1_id64, "EURUSD"_sv, 0_jd, 0x01};

  cout << toString(market) << endl;
  SWIRLY_CHECK(toString(market) == //
               "{\"contr\":\"EURUSD\""
               ",\"settlDate\":null"
               ",\"state\":1"
               ",\"lastLots\":null"
               ",\"lastTicks\":null"
               ",\"lastTime\":null"
               ",\"bidTicks\":[null,null,null]"
               ",\"bidResd\":[null,null,null]"
               ",\"bidCount\":[null,null,null]"
               ",\"offerTicks\":[null,null,null]"
               ",\"offerResd\":[null,null,null]"
               ",\"offerCount\":[null,null,null]"
               "}");
}
