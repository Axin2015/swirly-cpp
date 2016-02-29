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

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(RestRequestSuite)

BOOST_AUTO_TEST_CASE(MnemCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"mnem":"EURUSD"})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::Mnem);
  BOOST_CHECK_EQUAL(rr.mnem(), "EURUSD");

  rr.reset(false);
  BOOST_CHECK(rr.parse(R"({"mnem":null})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
  BOOST_CHECK(rr.mnem().empty());
}

BOOST_AUTO_TEST_CASE(DisplayCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"display":"Euro Dollar"})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::Display);
  BOOST_CHECK_EQUAL(rr.display(), "Euro Dollar");

  rr.reset(false);
  BOOST_CHECK(rr.parse(R"({"display":null})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
  BOOST_CHECK(rr.display().empty());
}

BOOST_AUTO_TEST_CASE(EmailCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"email":"mark.aylett@gmail.com"})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::Email);
  BOOST_CHECK_EQUAL(rr.email(), "mark.aylett@gmail.com");

  rr.reset(false);
  rr.parse(R"({"email":null})"_sv);
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
  BOOST_CHECK(rr.email().empty());
}

BOOST_AUTO_TEST_CASE(TraderCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"trader":"MARAYL"})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::Trader);
  BOOST_CHECK_EQUAL(rr.trader(), "MARAYL");

  rr.reset(false);
  rr.parse(R"({"trader":null})"_sv);
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
  BOOST_CHECK(rr.trader().empty());
}

BOOST_AUTO_TEST_CASE(ContrCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"contr":"EURUSD"})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::Contr);
  BOOST_CHECK_EQUAL(rr.contr(), "EURUSD");

  rr.reset(false);
  rr.parse(R"({"contr":null})"_sv);
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
  BOOST_CHECK(rr.contr().empty());
}

BOOST_AUTO_TEST_CASE(SettlDateCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"settlDate":20140314})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::SettlDate);
  BOOST_CHECK_EQUAL(rr.settlDate(), 20140314_dt);

  rr.reset(false);
  rr.parse(R"({"settlDate":null})"_sv);
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
  BOOST_CHECK_EQUAL(rr.settlDate(), 0_dt);
}

BOOST_AUTO_TEST_CASE(ExpiryDateCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"expiryDate":20140314})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::ExpiryDate);
  BOOST_CHECK_EQUAL(rr.expiryDate(), 20140314_dt);

  rr.reset(false);
  rr.parse(R"({"expiryDate":null})"_sv);
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
  BOOST_CHECK_EQUAL(rr.expiryDate(), 0_dt);
}

BOOST_AUTO_TEST_CASE(RefCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"ref":"EURUSD"})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::Ref);
  BOOST_CHECK_EQUAL(rr.ref(), "EURUSD");

  rr.reset(false);
  rr.parse(R"({"ref":null})"_sv);
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
  BOOST_CHECK(rr.ref().empty());
}

BOOST_AUTO_TEST_CASE(StateCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"state":3})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::State);
  BOOST_CHECK_EQUAL(rr.state(), 3U);

  rr.reset(false);
  rr.parse(R"({"state":null})"_sv);
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
  BOOST_CHECK_EQUAL(rr.state(), 0U);
}

BOOST_AUTO_TEST_CASE(SideCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"side":"BUY"})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::Side);
  BOOST_CHECK_EQUAL(rr.side(), Side::Buy);

  rr.reset(false);
  rr.parse(R"({"side":null})"_sv);
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
}

BOOST_AUTO_TEST_CASE(LotsCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"lots":101})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::Lots);
  BOOST_CHECK_EQUAL(rr.lots(), 101_lts);

  rr.reset(false);
  rr.parse(R"({"lots":null})"_sv);
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
  BOOST_CHECK_EQUAL(rr.lots(), 0_lts);
}

BOOST_AUTO_TEST_CASE(TicksCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"ticks":12345})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::Ticks);
  BOOST_CHECK_EQUAL(rr.ticks(), 12345_tks);

  rr.reset(false);
  rr.parse(R"({"ticks":null})"_sv);
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
  BOOST_CHECK_EQUAL(rr.ticks(), 0_tks);
}

BOOST_AUTO_TEST_CASE(MinLotsCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"minLots":101})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::MinLots);
  BOOST_CHECK_EQUAL(rr.minLots(), 101_lts);

  rr.reset(false);
  rr.parse(R"({"minLots":null})"_sv);
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
  BOOST_CHECK_EQUAL(rr.minLots(), 0_lts);
}

BOOST_AUTO_TEST_CASE(RoleCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"role":"MAKER"})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::Role);
  BOOST_CHECK_EQUAL(rr.role(), Role::Maker);

  rr.reset(false);
  rr.parse(R"({"role":null})"_sv);
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
  BOOST_CHECK_EQUAL(rr.role(), Role::None);
}

BOOST_AUTO_TEST_CASE(CptyCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"cpty":"MARAYL"})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::Cpty);
  BOOST_CHECK_EQUAL(rr.cpty(), "MARAYL");

  rr.reset(false);
  rr.parse(R"({"cpty":null})"_sv);
  BOOST_CHECK_EQUAL(rr.fields(), 0U);
  BOOST_CHECK(rr.cpty().empty());
}

BOOST_AUTO_TEST_CASE(MultiCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"contr":"EURUSD","settlDate":20140314})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::Contr | RestRequest::SettlDate);
  BOOST_CHECK_EQUAL(rr.contr(), "EURUSD");
  BOOST_CHECK_EQUAL(rr.settlDate(), 20140314_dt);
}

BOOST_AUTO_TEST_CASE(DuplicateCase)
{
  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"trader":"MARAYL1","trader":"MARAYL2"})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::Trader);
  BOOST_CHECK_EQUAL(rr.trader(), "MARAYL2");
}

BOOST_AUTO_TEST_CASE(BadFieldCase)
{
  RestRequest rr;

  BOOST_CHECK_THROW(rr.parse(R"({"foo":null})"_sv), BadRequestException);
}

BOOST_AUTO_TEST_CASE(BadTypeCase)
{
  RestRequest rr;

  BOOST_CHECK_THROW(rr.parse(R"({"ticks":"101"})"_sv), BadRequestException);
}

BOOST_AUTO_TEST_CASE(BadObjectCase)
{
  RestRequest rr;

  BOOST_CHECK_THROW(rr.parse(R"([{"ticks":101}])"_sv), BadRequestException);
}

BOOST_AUTO_TEST_CASE(MaxLenCase)
{
  RestRequest rr;

  BOOST_CHECK_THROW(rr.parse(R"({"mnem":"0123456789ABCDEFx"})"_sv), BadRequestException);
}

BOOST_AUTO_TEST_CASE(NegativeCase)
{
  using namespace enumops;

  RestRequest rr;

  BOOST_CHECK(rr.parse(R"({"ticks":-1})"_sv));
  BOOST_CHECK_EQUAL(rr.fields(), RestRequest::Ticks);
  BOOST_CHECK_EQUAL(rr.ticks(), -1_tks);

  rr.reset();
  BOOST_CHECK_THROW(rr.parse(R"({"state":-1})"_sv), BadRequestException);
}

BOOST_AUTO_TEST_SUITE_END()
