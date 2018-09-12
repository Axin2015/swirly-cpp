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

#include <swirly/app/Exception.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(RestBodySuite)

BOOST_AUTO_TEST_CASE(RestBodySymbolCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"symbol":"EURUSD"})"sv));
    BOOST_TEST(rb.fields() == RestBody::Symbol);
    BOOST_TEST(rb.symbol() == "EURUSD"sv);

    rb.reset(false);
    BOOST_TEST(rb.parse(R"({"symbol":null})"sv));
    BOOST_TEST(rb.fields() == 0U);
    BOOST_TEST(rb.symbol().empty());
}

BOOST_AUTO_TEST_CASE(RestBodyAccntCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"accnt":"MARAYL"})"sv));
    BOOST_TEST(rb.fields() == RestBody::Accnt);
    BOOST_TEST(rb.accnt() == "MARAYL"sv);

    rb.reset(false);
    BOOST_TEST(rb.parse(R"({"accnt":null})"sv));
    BOOST_TEST(rb.fields() == 0U);
    BOOST_TEST(rb.accnt().empty());
}

BOOST_AUTO_TEST_CASE(RestBodyInstrCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"instr":"EURUSD"})"sv));
    BOOST_TEST(rb.fields() == RestBody::Instr);
    BOOST_TEST(rb.instr() == "EURUSD"sv);

    rb.reset(false);
    BOOST_TEST(rb.parse(R"({"instr":null})"sv));
    BOOST_TEST(rb.fields() == 0U);
    BOOST_TEST(rb.instr().empty());
}

BOOST_AUTO_TEST_CASE(RestBodySettlDateCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"settl_date":20140314})"sv));
    BOOST_TEST(rb.fields() == RestBody::SettlDate);
    BOOST_TEST(rb.settl_date() == 20140314_ymd);

    rb.reset(false);
    BOOST_TEST(rb.parse(R"({"settl_date":null})"sv));
    BOOST_TEST(rb.fields() == 0U);
    BOOST_TEST(rb.settl_date() == 0_ymd);
}

BOOST_AUTO_TEST_CASE(RestBodyRefCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"ref":"EURUSD"})"sv));
    BOOST_TEST(rb.fields() == RestBody::Ref);
    BOOST_TEST(rb.ref() == "EURUSD"sv);

    rb.reset(false);
    BOOST_TEST(rb.parse(R"({"ref":null})"sv));
    BOOST_TEST(rb.fields() == 0U);
    BOOST_TEST(rb.ref().empty());
}

BOOST_AUTO_TEST_CASE(RestBodyStateCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"state":3})"sv));
    BOOST_TEST(rb.fields() == RestBody::State);
    BOOST_TEST(rb.state() == 3U);

    rb.reset(false);
    BOOST_TEST(rb.parse(R"({"state":null})"sv));
    BOOST_TEST(rb.fields() == 0U);
    BOOST_TEST(rb.state() == 0U);
}

BOOST_AUTO_TEST_CASE(RestBodySideCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"side":"Buy"})"sv));
    BOOST_TEST(rb.fields() == RestBody::Side);
    BOOST_TEST(rb.side() == Side::Buy);

    rb.reset(false);
    BOOST_TEST(rb.parse(R"({"side":"Sell"})"sv));
    BOOST_TEST(rb.fields() == RestBody::Side);
    BOOST_TEST(rb.side() == Side::Sell);
}

BOOST_AUTO_TEST_CASE(RestBodyLotsCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"lots":101})"sv));
    BOOST_TEST(rb.fields() == RestBody::Lots);
    BOOST_TEST(rb.lots() == 101_lts);

    rb.reset(false);
    BOOST_TEST(rb.parse(R"({"lots":null})"sv));
    BOOST_TEST(rb.fields() == 0U);
    BOOST_TEST(rb.lots() == 0_lts);
}

BOOST_AUTO_TEST_CASE(RestBodyTicksCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"ticks":12345})"sv));
    BOOST_TEST(rb.fields() == RestBody::Ticks);
    BOOST_TEST(rb.ticks() == 12345_tks);

    rb.reset(false);
    BOOST_TEST(rb.parse(R"({"ticks":null})"sv));
    BOOST_TEST(rb.fields() == 0U);
    BOOST_TEST(rb.ticks() == 0_tks);
}

BOOST_AUTO_TEST_CASE(RestBodyMinLotsCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"min_lots":101})"sv));
    BOOST_TEST(rb.fields() == RestBody::MinLots);
    BOOST_TEST(rb.min_lots() == 101_lts);

    rb.reset(false);
    BOOST_TEST(rb.parse(R"({"min_lots":null})"sv));
    BOOST_TEST(rb.fields() == 0U);
    BOOST_TEST(rb.min_lots() == 0_lts);
}

BOOST_AUTO_TEST_CASE(RestBodyLiqIndCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"liq_ind":"Maker"})"sv));
    BOOST_TEST(rb.fields() == RestBody::LiqInd);
    BOOST_TEST(rb.liq_ind() == LiqInd::Maker);

    rb.reset(false);
    BOOST_TEST(rb.parse(R"({"liq_ind":"Taker"})"sv));
    BOOST_TEST(rb.fields() == RestBody::LiqInd);
    BOOST_TEST(rb.liq_ind() == LiqInd::Taker);

    rb.reset(false);
    BOOST_TEST(rb.parse(R"({"liq_ind":null})"sv));
    BOOST_TEST(rb.fields() == 0U);
    BOOST_TEST(rb.liq_ind() == LiqInd::None);
}

BOOST_AUTO_TEST_CASE(RestBodyCptyCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"cpty":"MARAYL"})"sv));
    BOOST_TEST(rb.fields() == RestBody::Cpty);
    BOOST_TEST(rb.cpty() == "MARAYL"sv);

    rb.reset(false);
    BOOST_TEST(rb.parse(R"({"cpty":null})"sv));
    BOOST_TEST(rb.fields() == 0U);
    BOOST_TEST(rb.cpty().empty());
}

BOOST_AUTO_TEST_CASE(RestBodyMultiCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"instr":"EURUSD","settl_date":20140314})"sv));
    BOOST_TEST(rb.fields() == (RestBody::Instr | RestBody::SettlDate));
    BOOST_TEST(rb.instr() == "EURUSD"sv);
    BOOST_TEST(rb.settl_date() == 20140314_ymd);
}

BOOST_AUTO_TEST_CASE(RestBodyDuplicateCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"accnt":"MARAYL1","accnt":"MARAYL2"})"sv));
    BOOST_TEST(rb.fields() == RestBody::Accnt);
    BOOST_TEST(rb.accnt() == "MARAYL2"sv);
}

BOOST_AUTO_TEST_CASE(RestBodyBadFieldCase)
{
    RestBody rb;

    BOOST_CHECK_THROW(rb.parse(R"({"foo":null})"sv), BadRequestException);
}

BOOST_AUTO_TEST_CASE(RestBodyBadTypeCase)
{
    RestBody rb;

    BOOST_CHECK_THROW(rb.parse(R"({"ticks":"101"})"sv), BadRequestException);
}

BOOST_AUTO_TEST_CASE(RestBodyBadObjectCase)
{
    RestBody rb;

    BOOST_CHECK_THROW(rb.parse(R"([{"ticks":101}])"sv), BadRequestException);
}

BOOST_AUTO_TEST_CASE(RestBodyMaxLenCase)
{
    RestBody rb;

    BOOST_CHECK_THROW(rb.parse(R"({"symbol":"0123456789ABCDEFx"})"sv), BadRequestException);
}

BOOST_AUTO_TEST_CASE(RestBodyNegativeCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(R"({"ticks":-1})"sv));
    BOOST_TEST(rb.fields() == RestBody::Ticks);
    BOOST_TEST(rb.ticks() == -1_tks);

    rb.reset();
    BOOST_CHECK_THROW(rb.parse(R"({"state":-1})"sv), BadRequestException);
}

BOOST_AUTO_TEST_CASE(RestBodyAllCase)
{
    RestBody rb;

    BOOST_TEST(rb.parse(
        R"({"accnt":"MARAYL","symbol":"EURUSD","instr":"EURUSD","settl_date":20140315,"ref":"EURUSD","state":3,"side":"Buy","lots":101,"ticks":12345,"min_lots":101,"liq_ind":"Maker","cpty":"MARAYL"})"sv));
    BOOST_TEST(rb.fields() == ((RestBody::Cpty - 1) | RestBody::Cpty));
    BOOST_TEST(rb.symbol() == "EURUSD"sv);
    BOOST_TEST(rb.accnt() == "MARAYL"sv);
    BOOST_TEST(rb.instr() == "EURUSD"sv);
    BOOST_TEST(rb.settl_date() == 20140315_ymd);
    BOOST_TEST(rb.ref() == "EURUSD"sv);
    BOOST_TEST(rb.state() == 3U);
    BOOST_TEST(rb.side() == Side::Buy);
    BOOST_TEST(rb.lots() == 101_lts);
    BOOST_TEST(rb.ticks() == 12345_tks);
    BOOST_TEST(rb.min_lots() == 101_lts);
    BOOST_TEST(rb.liq_ind() == LiqInd::Maker);
    BOOST_TEST(rb.cpty() == "MARAYL"sv);
}

BOOST_AUTO_TEST_CASE(RestBodyPartialCase)
{
    RestBody rb;

    BOOST_TEST(!rb.parse(R"({"symbol":"E)"sv));
    BOOST_TEST(!rb.parse(R"(URUSD","ac)"sv));
    BOOST_TEST(!rb.parse(R"(cnt":"MAR)"sv));
    BOOST_TEST(rb.parse(R"(AYL"})"sv));

    BOOST_TEST(rb.fields() == (RestBody::Symbol | RestBody::Accnt));
    BOOST_TEST(rb.symbol() == "EURUSD"sv);
    BOOST_TEST(rb.accnt() == "MARAYL"sv);
}

BOOST_AUTO_TEST_SUITE_END()
