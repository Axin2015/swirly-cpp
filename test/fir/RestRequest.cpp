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
#include <RestRequest.hpp>

#include <swirly/elm/Exception.hpp>

#include <swirly/tea/Test.hpp>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(RestRequestMnem)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"mnem":"EURUSD"})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::Mnem);
  SWIRLY_CHECK(rr.mnem() == "EURUSD"_sv);

  rr.reset(false);
  SWIRLY_CHECK(rr.parse(R"({"mnem":null})"_sv));
  SWIRLY_CHECK(rr.fields() == 0U);
  SWIRLY_CHECK(rr.mnem().empty());
}

SWIRLY_TEST_CASE(RestRequestDisplay)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"display":"Euro Dollar"})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::Display);
  SWIRLY_CHECK(rr.display() == "Euro Dollar"_sv);

  rr.reset(false);
  SWIRLY_CHECK(rr.parse(R"({"display":null})"_sv));
  SWIRLY_CHECK(rr.fields() == 0U);
  SWIRLY_CHECK(rr.display().empty());
}

SWIRLY_TEST_CASE(RestRequestEmail)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"email":"mark.aylett@gmail.com"})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::Email);
  SWIRLY_CHECK(rr.email() == "mark.aylett@gmail.com"_sv);

  rr.reset(false);
  rr.parse(R"({"email":null})"_sv);
  SWIRLY_CHECK(rr.fields() == 0U);
  SWIRLY_CHECK(rr.email().empty());
}

SWIRLY_TEST_CASE(RestRequestTrader)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"trader":"MARAYL"})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::Trader);
  SWIRLY_CHECK(rr.trader() == "MARAYL"_sv);

  rr.reset(false);
  rr.parse(R"({"trader":null})"_sv);
  SWIRLY_CHECK(rr.fields() == 0U);
  SWIRLY_CHECK(rr.trader().empty());
}

SWIRLY_TEST_CASE(RestRequestContr)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"contr":"EURUSD"})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::Contr);
  SWIRLY_CHECK(rr.contr() == "EURUSD"_sv);

  rr.reset(false);
  rr.parse(R"({"contr":null})"_sv);
  SWIRLY_CHECK(rr.fields() == 0U);
  SWIRLY_CHECK(rr.contr().empty());
}

SWIRLY_TEST_CASE(RestRequestSettlDate)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"settlDate":20140314})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::SettlDate);
  SWIRLY_CHECK(rr.settlDate() == 20140314_dt);

  rr.reset(false);
  rr.parse(R"({"settlDate":null})"_sv);
  SWIRLY_CHECK(rr.fields() == 0U);
  SWIRLY_CHECK(rr.settlDate() == 0_dt);
}

SWIRLY_TEST_CASE(RestRequestExpiryDate)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"expiryDate":20140314})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::ExpiryDate);
  SWIRLY_CHECK(rr.expiryDate() == 20140314_dt);

  rr.reset(false);
  rr.parse(R"({"expiryDate":null})"_sv);
  SWIRLY_CHECK(rr.fields() == 0U);
  SWIRLY_CHECK(rr.expiryDate() == 0_dt);
}

SWIRLY_TEST_CASE(RestRequestRef)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"ref":"EURUSD"})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::Ref);
  SWIRLY_CHECK(rr.ref() == "EURUSD"_sv);

  rr.reset(false);
  rr.parse(R"({"ref":null})"_sv);
  SWIRLY_CHECK(rr.fields() == 0U);
  SWIRLY_CHECK(rr.ref().empty());
}

SWIRLY_TEST_CASE(RestRequestState)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"state":3})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::State);
  SWIRLY_CHECK(rr.state() == 3U);

  rr.reset(false);
  rr.parse(R"({"state":null})"_sv);
  SWIRLY_CHECK(rr.fields() == 0U);
  SWIRLY_CHECK(rr.state() == 0U);
}

SWIRLY_TEST_CASE(RestRequestSide)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"side":"BUY"})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::Side);
  SWIRLY_CHECK(rr.side() == Side::Buy);

  rr.reset(false);
  rr.parse(R"({"side":null})"_sv);
  SWIRLY_CHECK(rr.fields() == 0U);
}

SWIRLY_TEST_CASE(RestRequestLots)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"lots":101})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::Lots);
  SWIRLY_CHECK(rr.lots() == 101_lts);

  rr.reset(false);
  rr.parse(R"({"lots":null})"_sv);
  SWIRLY_CHECK(rr.fields() == 0U);
  SWIRLY_CHECK(rr.lots() == 0_lts);
}

SWIRLY_TEST_CASE(RestRequestTicks)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"ticks":12345})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::Ticks);
  SWIRLY_CHECK(rr.ticks() == 12345_tks);

  rr.reset(false);
  rr.parse(R"({"ticks":null})"_sv);
  SWIRLY_CHECK(rr.fields() == 0U);
  SWIRLY_CHECK(rr.ticks() == 0_tks);
}

SWIRLY_TEST_CASE(RestRequestMinLots)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"minLots":101})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::MinLots);
  SWIRLY_CHECK(rr.minLots() == 101_lts);

  rr.reset(false);
  rr.parse(R"({"minLots":null})"_sv);
  SWIRLY_CHECK(rr.fields() == 0U);
  SWIRLY_CHECK(rr.minLots() == 0_lts);
}

SWIRLY_TEST_CASE(RestRequestRole)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"role":"MAKER"})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::Role);
  SWIRLY_CHECK(rr.role() == Role::Maker);

  rr.reset(false);
  rr.parse(R"({"role":null})"_sv);
  SWIRLY_CHECK(rr.fields() == 0U);
  SWIRLY_CHECK(rr.role() == Role::None);
}

SWIRLY_TEST_CASE(RestRequestCpty)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"cpty":"MARAYL"})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::Cpty);
  SWIRLY_CHECK(rr.cpty() == "MARAYL"_sv);

  rr.reset(false);
  rr.parse(R"({"cpty":null})"_sv);
  SWIRLY_CHECK(rr.fields() == 0U);
  SWIRLY_CHECK(rr.cpty().empty());
}

SWIRLY_TEST_CASE(RestRequestMulti)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"contr":"EURUSD","settlDate":20140314})"_sv));
  SWIRLY_CHECK(rr.fields() == (RestRequest::Contr | RestRequest::SettlDate));
  SWIRLY_CHECK(rr.contr() == "EURUSD"_sv);
  SWIRLY_CHECK(rr.settlDate() == 20140314_dt);
}

SWIRLY_TEST_CASE(RestRequestDuplicate)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"trader":"MARAYL1","trader":"MARAYL2"})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::Trader);
  SWIRLY_CHECK(rr.trader() == "MARAYL2"_sv);
}

SWIRLY_TEST_CASE(RestRequestBadField)
{
  RestRequest rr;

  SWIRLY_CHECK_THROW(rr.parse(R"({"foo":null})"_sv), BadRequestException);
}

SWIRLY_TEST_CASE(RestRequestBadType)
{
  RestRequest rr;

  SWIRLY_CHECK_THROW(rr.parse(R"({"ticks":"101"})"_sv), BadRequestException);
}

SWIRLY_TEST_CASE(RestRequestBadObject)
{
  RestRequest rr;

  SWIRLY_CHECK_THROW(rr.parse(R"([{"ticks":101}])"_sv), BadRequestException);
}

SWIRLY_TEST_CASE(RestRequestMaxLen)
{
  RestRequest rr;

  SWIRLY_CHECK_THROW(rr.parse(R"({"mnem":"0123456789ABCDEFx"})"_sv), BadRequestException);
}

SWIRLY_TEST_CASE(RestRequestNegative)
{
  using namespace enumops;

  RestRequest rr;

  SWIRLY_CHECK(rr.parse(R"({"ticks":-1})"_sv));
  SWIRLY_CHECK(rr.fields() == RestRequest::Ticks);
  SWIRLY_CHECK(rr.ticks() == -1_tks);

  rr.reset();
  SWIRLY_CHECK_THROW(rr.parse(R"({"state":-1})"_sv), BadRequestException);
}

SWIRLY_TEST_CASE(RestRequestAll)
{
  RestRequest rr;

  SWIRLY_CHECK(rr.parse(
    R"({"mnem":"EURUSD","display":"Euro Dollar","email":"mark.aylett@gmail.com","trader":"MARAYL","contr":"EURUSD","settlDate":20140315,"expiryDate":20140314,"ref":"EURUSD","state":3,"side":"BUY","lots":101,"ticks":12345,"minLots":101,"role":"MAKER","cpty":"MARAYL"})"_sv));
  SWIRLY_CHECK(rr.fields() == ((RestRequest::Cpty - 1) | RestRequest::Cpty));
  SWIRLY_CHECK(rr.mnem() == "EURUSD"_sv);
  SWIRLY_CHECK(rr.display() == "Euro Dollar"_sv);
  SWIRLY_CHECK(rr.email() == "mark.aylett@gmail.com"_sv);
  SWIRLY_CHECK(rr.trader() == "MARAYL"_sv);
  SWIRLY_CHECK(rr.contr() == "EURUSD"_sv);
  SWIRLY_CHECK(rr.settlDate() == 20140315_dt);
  SWIRLY_CHECK(rr.expiryDate() == 20140314_dt);
  SWIRLY_CHECK(rr.ref() == "EURUSD"_sv);
  SWIRLY_CHECK(rr.state() == 3U);
  SWIRLY_CHECK(rr.side() == Side::Buy);
  SWIRLY_CHECK(rr.lots() == 101_lts);
  SWIRLY_CHECK(rr.ticks() == 12345_tks);
  SWIRLY_CHECK(rr.minLots() == 101_lts);
  SWIRLY_CHECK(rr.role() == Role::Maker);
  SWIRLY_CHECK(rr.cpty() == "MARAYL"_sv);
}
