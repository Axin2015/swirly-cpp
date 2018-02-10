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
#include "RestBody.hpp"

#include <swirly/fin/Exception.hpp>

#include <swirly/unit/Test.hpp>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(RestBodySymbol)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"symbol":"EURUSD"})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::Symbol);
    SWIRLY_CHECK(rb.symbol() == "EURUSD"sv);

    rb.reset(false);
    SWIRLY_CHECK(rb.parse(R"({"symbol":null})"sv));
    SWIRLY_CHECK(rb.fields() == 0U);
    SWIRLY_CHECK(rb.symbol().empty());
}

SWIRLY_TEST_CASE(RestBodyAccnt)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"accnt":"MARAYL"})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::Accnt);
    SWIRLY_CHECK(rb.accnt() == "MARAYL"sv);

    rb.reset(false);
    SWIRLY_CHECK(rb.parse(R"({"accnt":null})"sv));
    SWIRLY_CHECK(rb.fields() == 0U);
    SWIRLY_CHECK(rb.accnt().empty());
}

SWIRLY_TEST_CASE(RestBodyInstr)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"instr":"EURUSD"})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::Instr);
    SWIRLY_CHECK(rb.instr() == "EURUSD"sv);

    rb.reset(false);
    SWIRLY_CHECK(rb.parse(R"({"instr":null})"sv));
    SWIRLY_CHECK(rb.fields() == 0U);
    SWIRLY_CHECK(rb.instr().empty());
}

SWIRLY_TEST_CASE(RestBodySettlDate)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"settlDate":20140314})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::SettlDate);
    SWIRLY_CHECK(rb.settlDate() == 20140314_ymd);

    rb.reset(false);
    SWIRLY_CHECK(rb.parse(R"({"settlDate":null})"sv));
    SWIRLY_CHECK(rb.fields() == 0U);
    SWIRLY_CHECK(rb.settlDate() == 0_ymd);
}

SWIRLY_TEST_CASE(RestBodyRef)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"ref":"EURUSD"})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::Ref);
    SWIRLY_CHECK(rb.ref() == "EURUSD"sv);

    rb.reset(false);
    SWIRLY_CHECK(rb.parse(R"({"ref":null})"sv));
    SWIRLY_CHECK(rb.fields() == 0U);
    SWIRLY_CHECK(rb.ref().empty());
}

SWIRLY_TEST_CASE(RestBodyState)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"state":3})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::State);
    SWIRLY_CHECK(rb.state() == 3U);

    rb.reset(false);
    SWIRLY_CHECK(rb.parse(R"({"state":null})"sv));
    SWIRLY_CHECK(rb.fields() == 0U);
    SWIRLY_CHECK(rb.state() == 0U);
}

SWIRLY_TEST_CASE(RestBodySide)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"side":"BUY"})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::Side);
    SWIRLY_CHECK(rb.side() == Side::Buy);

    rb.reset(false);
    SWIRLY_CHECK(rb.parse(R"({"side":"SELL"})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::Side);
    SWIRLY_CHECK(rb.side() == Side::Sell);
}

SWIRLY_TEST_CASE(RestBodyLots)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"lots":101})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::Lots);
    SWIRLY_CHECK(rb.lots() == 101_lts);

    rb.reset(false);
    SWIRLY_CHECK(rb.parse(R"({"lots":null})"sv));
    SWIRLY_CHECK(rb.fields() == 0U);
    SWIRLY_CHECK(rb.lots() == 0_lts);
}

SWIRLY_TEST_CASE(RestBodyTicks)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"ticks":12345})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::Ticks);
    SWIRLY_CHECK(rb.ticks() == 12345_tks);

    rb.reset(false);
    SWIRLY_CHECK(rb.parse(R"({"ticks":null})"sv));
    SWIRLY_CHECK(rb.fields() == 0U);
    SWIRLY_CHECK(rb.ticks() == 0_tks);
}

SWIRLY_TEST_CASE(RestBodyMinLots)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"minLots":101})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::MinLots);
    SWIRLY_CHECK(rb.minLots() == 101_lts);

    rb.reset(false);
    SWIRLY_CHECK(rb.parse(R"({"minLots":null})"sv));
    SWIRLY_CHECK(rb.fields() == 0U);
    SWIRLY_CHECK(rb.minLots() == 0_lts);
}

SWIRLY_TEST_CASE(RestBodyLiqInd)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"liqInd":"MAKER"})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::LiqInd);
    SWIRLY_CHECK(rb.liqInd() == LiqInd::Maker);

    rb.reset(false);
    SWIRLY_CHECK(rb.parse(R"({"liqInd":"TAKER"})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::LiqInd);
    SWIRLY_CHECK(rb.liqInd() == LiqInd::Taker);

    rb.reset(false);
    SWIRLY_CHECK(rb.parse(R"({"liqInd":null})"sv));
    SWIRLY_CHECK(rb.fields() == 0U);
    SWIRLY_CHECK(rb.liqInd() == LiqInd::None);
}

SWIRLY_TEST_CASE(RestBodyCpty)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"cpty":"MARAYL"})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::Cpty);
    SWIRLY_CHECK(rb.cpty() == "MARAYL"sv);

    rb.reset(false);
    SWIRLY_CHECK(rb.parse(R"({"cpty":null})"sv));
    SWIRLY_CHECK(rb.fields() == 0U);
    SWIRLY_CHECK(rb.cpty().empty());
}

SWIRLY_TEST_CASE(RestBodyMulti)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"instr":"EURUSD","settlDate":20140314})"sv));
    SWIRLY_CHECK(rb.fields() == (RestBody::Instr | RestBody::SettlDate));
    SWIRLY_CHECK(rb.instr() == "EURUSD"sv);
    SWIRLY_CHECK(rb.settlDate() == 20140314_ymd);
}

SWIRLY_TEST_CASE(RestBodyDuplicate)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"accnt":"MARAYL1","accnt":"MARAYL2"})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::Accnt);
    SWIRLY_CHECK(rb.accnt() == "MARAYL2"sv);
}

SWIRLY_TEST_CASE(RestBodyBadField)
{
    RestBody rb;

    SWIRLY_CHECK_THROW(rb.parse(R"({"foo":null})"sv), BadRequestException);
}

SWIRLY_TEST_CASE(RestBodyBadType)
{
    RestBody rb;

    SWIRLY_CHECK_THROW(rb.parse(R"({"ticks":"101"})"sv), BadRequestException);
}

SWIRLY_TEST_CASE(RestBodyBadObject)
{
    RestBody rb;

    SWIRLY_CHECK_THROW(rb.parse(R"([{"ticks":101}])"sv), BadRequestException);
}

SWIRLY_TEST_CASE(RestBodyMaxLen)
{
    RestBody rb;

    SWIRLY_CHECK_THROW(rb.parse(R"({"symbol":"0123456789ABCDEFx"})"sv), BadRequestException);
}

SWIRLY_TEST_CASE(RestBodyNegative)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(R"({"ticks":-1})"sv));
    SWIRLY_CHECK(rb.fields() == RestBody::Ticks);
    SWIRLY_CHECK(rb.ticks() == -1_tks);

    rb.reset();
    SWIRLY_CHECK_THROW(rb.parse(R"({"state":-1})"sv), BadRequestException);
}

SWIRLY_TEST_CASE(RestBodyAll)
{
    RestBody rb;

    SWIRLY_CHECK(rb.parse(
        R"({"accnt":"MARAYL","symbol":"EURUSD","instr":"EURUSD","settlDate":20140315,"ref":"EURUSD","state":3,"side":"BUY","lots":101,"ticks":12345,"minLots":101,"liqInd":"MAKER","cpty":"MARAYL"})"sv));
    SWIRLY_CHECK(rb.fields() == ((RestBody::Cpty - 1) | RestBody::Cpty));
    SWIRLY_CHECK(rb.symbol() == "EURUSD"sv);
    SWIRLY_CHECK(rb.accnt() == "MARAYL"sv);
    SWIRLY_CHECK(rb.instr() == "EURUSD"sv);
    SWIRLY_CHECK(rb.settlDate() == 20140315_ymd);
    SWIRLY_CHECK(rb.ref() == "EURUSD"sv);
    SWIRLY_CHECK(rb.state() == 3U);
    SWIRLY_CHECK(rb.side() == Side::Buy);
    SWIRLY_CHECK(rb.lots() == 101_lts);
    SWIRLY_CHECK(rb.ticks() == 12345_tks);
    SWIRLY_CHECK(rb.minLots() == 101_lts);
    SWIRLY_CHECK(rb.liqInd() == LiqInd::Maker);
    SWIRLY_CHECK(rb.cpty() == "MARAYL"sv);
}

#include <iostream>
SWIRLY_TEST_CASE(RestBodyPartial)
{
    RestBody rb;

    cerr << __LINE__ << endl;
    SWIRLY_CHECK(!rb.parse(R"({"symbol":"E)"sv));
    cerr << __LINE__ << endl;
    SWIRLY_CHECK(!rb.parse(R"(URUSD","ac)"sv));
    cerr << __LINE__ << endl;
    SWIRLY_CHECK(!rb.parse(R"(cnt":"MAR)"sv));
    cerr << __LINE__ << endl;
    SWIRLY_CHECK(rb.parse(R"(AYL"})"sv));
    cerr << __LINE__ << endl;

    cerr << __LINE__ << endl;
    SWIRLY_CHECK(rb.fields() == (RestBody::Symbol | RestBody::Accnt));
    cerr << __LINE__ << endl;
    SWIRLY_CHECK(rb.symbol() == "EURUSD"sv);
    cerr << __LINE__ << endl;
    SWIRLY_CHECK(rb.accnt() == "MARAYL"sv);
    cerr << __LINE__ << endl;
}
