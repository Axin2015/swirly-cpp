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
#include <swirly/clob/Serv.hpp>

#include <swirly/clob/Accnt.hpp>
#include <swirly/clob/Response.hpp>
#include <swirly/clob/Test.hpp>

#include <swirly/fin/Exception.hpp>

#include <swirly/util/Date.hpp>
#include <swirly/util/Time.hpp>

#include <swirly/unit/Test.hpp>

using namespace std;
using namespace swirly;

namespace {

constexpr auto Today = ymdToJd(2014, 2, 11);
constexpr auto SettlDay = Today + 2_jd;
constexpr auto MarketId = toMarketId(1_id32, SettlDay);

constexpr auto Now = jdToTime(Today);

class SWIRLY_API TestModel : public swirly::TestModel {
 protected:
  void doReadMarket(const ModelCallback<MarketPtr>& cb) const override
  {
    cb(Market::make(MarketId, "EURUSD"_sv, SettlDay, 0x1U));
  }
};

struct ServFixture {
  ServFixture() : serv{journ, 1 << 10, 1 << 4} { serv.load(TestModel{}, Now); }
  TestJourn journ;
  Serv serv;
};

} // anonymous

SWIRLY_FIXTURE_TEST_CASE(ServAssets, ServFixture)
{
  SWIRLY_CHECK(distance(serv.assets().begin(), serv.assets().end()) == 24);

  auto it = serv.assets().find("CHF"_sv);
  SWIRLY_CHECK(it != serv.assets().end());
  SWIRLY_CHECK(it->id() == 1_id32);
  SWIRLY_CHECK(it->mnem() == "CHF"_sv);
  SWIRLY_CHECK(it->display() == "Switzerland, Francs"_sv);

  SWIRLY_CHECK(it->type() == AssetType::Currency);
}

SWIRLY_FIXTURE_TEST_CASE(ServContrs, ServFixture)
{
  SWIRLY_CHECK(distance(serv.contrs().begin(), serv.contrs().end()) == 21);

  auto it = serv.contrs().find("EURUSD"_sv);
  SWIRLY_CHECK(it != serv.contrs().end());
  SWIRLY_CHECK(it->id() == 1_id32);
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

SWIRLY_FIXTURE_TEST_CASE(ServMarkets, ServFixture)
{
  SWIRLY_CHECK(distance(serv.markets().begin(), serv.markets().end()) == 1);

  auto it = serv.markets().find(MarketId);
  SWIRLY_CHECK(it != serv.markets().end());
  SWIRLY_CHECK(it->id() == MarketId);

  SWIRLY_CHECK(it->contr() == "EURUSD"_sv);
  SWIRLY_CHECK(it->settlDay() == SettlDay);
  SWIRLY_CHECK(it->state() == 0x1);
}

SWIRLY_FIXTURE_TEST_CASE(ServMarket, ServFixture)
{
  // Not found.
  SWIRLY_CHECK_THROW(serv.market(1_id64), MarketNotFoundException);

  auto& market = serv.market(MarketId);
  SWIRLY_CHECK(market.id() == MarketId);

  SWIRLY_CHECK(market.contr() == "EURUSD"_sv);
  SWIRLY_CHECK(market.settlDay() == SettlDay);
  SWIRLY_CHECK(market.state() == 0x1);
}

SWIRLY_FIXTURE_TEST_CASE(ServCreateMarket, ServFixture)
{
  const Contr& contr = serv.contr("USDJPY"_sv);
  const auto marketId = toMarketId(contr.id(), SettlDay);

  // Settl-day before bus-day.
  SWIRLY_CHECK_THROW(serv.createMarket(contr, Today - 1_jd, 0x1, Now), InvalidException);

  auto& market = serv.createMarket(contr, SettlDay, 0x1, Now);

  SWIRLY_CHECK(market.id() == marketId);

  SWIRLY_CHECK(market.contr() == "USDJPY"_sv);
  SWIRLY_CHECK(market.settlDay() == SettlDay);
  SWIRLY_CHECK(market.state() == 0x1);

  SWIRLY_CHECK(distance(serv.markets().begin(), serv.markets().end()) == 2);
  auto it = serv.markets().find(marketId);
  SWIRLY_CHECK(it != serv.markets().end());
  SWIRLY_CHECK(&*it == &market);

  // Already exists.
  SWIRLY_CHECK_THROW(serv.createMarket(contr, SettlDay, 0x1, Now), AlreadyExistsException);
}

SWIRLY_FIXTURE_TEST_CASE(ServUpdateMarket, ServFixture)
{
  const Contr& contr = serv.contr("USDJPY"_sv);
  const auto marketId = toMarketId(contr.id(), SettlDay);
  auto& market = serv.createMarket(contr, SettlDay, 0x1, Now);

  serv.updateMarket(market, 0x2, Now);

  SWIRLY_CHECK(market.id() == marketId);

  SWIRLY_CHECK(market.contr() == "USDJPY"_sv);
  SWIRLY_CHECK(market.settlDay() == SettlDay);
  SWIRLY_CHECK(market.state() == 0x2);
}

SWIRLY_FIXTURE_TEST_CASE(ServCreateOrder, ServFixture)
{
  auto& accnt = serv.accnt("MARAYL"_sv);
  const Contr& contr = serv.contr("EURUSD"_sv);
  const auto marketId = toMarketId(contr.id(), SettlDay);
  auto& market = serv.market(marketId);

  Response resp;
  serv.createOrder(accnt, market, ""_sv, Side::Buy, 5_lts, 12345_tks, 1_lts, Now, resp);

  SWIRLY_CHECK(resp.orders().size() == 1);
  SWIRLY_CHECK(resp.execs().size() == 1);

  ConstOrderPtr order{resp.orders().front()};
  SWIRLY_CHECK(order->marketId() == market.id());
  SWIRLY_CHECK(order->contr() == contr.mnem());
  SWIRLY_CHECK(order->settlDay() == SettlDay);
  SWIRLY_CHECK(order->id() == 1_id64);
  SWIRLY_CHECK(order->accnt() == accnt.mnem());
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
