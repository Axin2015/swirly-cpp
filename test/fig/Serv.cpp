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
#include <swirly/fig/Serv.hpp>

#include <swirly/fig/TraderSess.hpp>

#include <swirly/fig/Mock.hpp>
#include <swirly/fig/Response.hpp>

#include <swirly/elm/Factory.hpp>
#include <swirly/elm/MarketBook.hpp>

#include <swirly/ash/JulianDay.hpp>
#include <swirly/ash/Time.hpp>

#include <swirly/tea/Test.hpp>

using namespace swirly;
using namespace swirly::enumops;

namespace {

constexpr auto Today = ymdToJd(2014, 2, 11);
constexpr auto SettlDay = Today + 2_jd;
constexpr auto ExpiryDay = Today + 1_jd;

constexpr auto Now = jdToMs(Today);

class SWIRLY_API TestModel : public MockModel {
 protected:
  MarketSet doReadMarket(const Factory& factory) const override
  {
    MarketSet s;
    s.insert(factory.newMarket("EURUSD.MAR14"_sv, "EURUSD March 14"_sv, "EURUSD"_sv, SettlDay,
                               ExpiryDay, 0x1));
    return s;
  }
};

struct Fixture {
  Fixture() : now{getTimeOfDay()}, serv{model, journ, now} {}
  TestModel model;
  MockJourn journ;
  const Millis now;
  Serv serv;
};

} // anonymous

SWIRLY_FIXTURE_TEST_CASE(FindAsset, Fixture)
{
  auto it = serv.assets().find("CHF"_sv);
  SWIRLY_CHECK(it != serv.assets().end());
  SWIRLY_CHECK(it->mnem() == "CHF"_sv);
  SWIRLY_CHECK(it->display() == "Switzerland, Francs"_sv);

  SWIRLY_CHECK(it->assetType() == AssetType::Currency);
}

SWIRLY_FIXTURE_TEST_CASE(FindContr, Fixture)
{
  auto it = serv.contrs().find("EURUSD"_sv);
  SWIRLY_CHECK(it != serv.contrs().end());
  SWIRLY_CHECK(it->mnem() == "EURUSD"_sv);
  SWIRLY_CHECK(it->display() == "EURUSD"_sv);

  SWIRLY_CHECK(it->asset() == "EUR"_sv);
  SWIRLY_CHECK(it->ccy() == "USD"_sv);
  SWIRLY_CHECK(it->lotNumer() == 1000000);
  SWIRLY_CHECK(it->lotDenom() == 1);
  SWIRLY_CHECK(test::isSame(it->qtyInc(), 1e6, 0.1));
  SWIRLY_CHECK(it->tickNumer() == 1);
  SWIRLY_CHECK(it->tickDenom() == 10000);
  SWIRLY_CHECK(test::isSame(it->priceInc(), 1e-4));
  SWIRLY_CHECK(it->pipDp() == 4);
  SWIRLY_CHECK(it->qtyDp() == 0);
  SWIRLY_CHECK(it->minLots() == 1_lts);
  SWIRLY_CHECK(it->maxLots() == 10_lts);
}

SWIRLY_FIXTURE_TEST_CASE(FindMarket, Fixture)
{
  auto it = serv.markets().find("EURUSD.MAR14"_sv);
  SWIRLY_CHECK(it != serv.markets().end());
  SWIRLY_CHECK(it->mnem() == "EURUSD.MAR14"_sv);
  SWIRLY_CHECK(it->display() == "EURUSD March 14"_sv);

  SWIRLY_CHECK(it->contr() == "EURUSD"_sv);
  SWIRLY_CHECK(it->settlDay() == SettlDay);
  SWIRLY_CHECK(it->expiryDay() == ExpiryDay);
  SWIRLY_CHECK(it->state() == 0x1);
}

SWIRLY_FIXTURE_TEST_CASE(FindTrader, Fixture)
{
  auto it = serv.traders().find("MARAYL"_sv);
  SWIRLY_CHECK(it != serv.traders().end());
  SWIRLY_CHECK(it->mnem() == "MARAYL"_sv);
  SWIRLY_CHECK(it->display() == "Mark Aylett"_sv);

  SWIRLY_CHECK(it->email() == "mark.aylett@gmail.com"_sv);
}

SWIRLY_FIXTURE_TEST_CASE(CreateOrder, Fixture)
{
  auto& sess = serv.trader("MARAYL"_sv);
  auto& book = serv.market("EURUSD.MAR14"_sv);

  Response resp;
  serv.createOrder(sess, book, ""_sv, Side::Buy, 5_lts, 12345_tks, 1_lts, Now, resp);

  SWIRLY_CHECK(resp.orders().size() == 1);
  SWIRLY_CHECK(resp.execs().size() == 1);

  OrderPtr order{resp.orders().front()};
  SWIRLY_CHECK(order->trader() == sess.mnem());
  SWIRLY_CHECK(order->market() == book.mnem());
  SWIRLY_CHECK(order->ref().empty());
  SWIRLY_CHECK(order->state() == State::New);
  SWIRLY_CHECK(order->side() == Side::Buy);
  SWIRLY_CHECK(order->lots() == 5_lts);
  SWIRLY_CHECK(order->ticks() == 12345_tks);
  SWIRLY_CHECK(order->resd() == 5_lts);
  SWIRLY_CHECK(order->exec() == 0_lts);
  SWIRLY_CHECK(order->cost() == 0_cst);
  SWIRLY_CHECK(order->lastLots() == 0_lts);
  SWIRLY_CHECK(order->lastTicks() == 0_tks);
  SWIRLY_CHECK(order->minLots() == 1_lts);
  SWIRLY_CHECK(order->created() == Now);
  SWIRLY_CHECK(order->modified() == Now);
}
