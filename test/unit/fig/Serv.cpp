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

#include <swirly/fig/Accnt.hpp>
#include <swirly/fig/Response.hpp>
#include <swirly/fig/Test.hpp>

#include <swirly/elm/Exception.hpp>
#include <swirly/elm/MarketBook.hpp>

#include <swirly/ash/Date.hpp>
#include <swirly/ash/Time.hpp>

#include <swirly/tea/Test.hpp>

using namespace std;
using namespace swirly;
using namespace swirly::enumops;

namespace {

constexpr auto Today = ymdToJd(2014, 2, 11);
constexpr auto SettlDay = Today + 2_jd;

constexpr auto Now = jdToMs(Today);

class SWIRLY_API TestModel : public swirly::TestModel {
 protected:
  void doReadMarket(const ModelCallback<MarketBookPtr>& cb) const override
  {
    cb(MarketBook::make("EURUSD.MAR14"_sv, "EURUSD March 14"_sv, "EURUSD"_sv, SettlDay, 0x1));
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

  auto it = serv.markets().find("EURUSD.MAR14"_sv);
  SWIRLY_CHECK(it != serv.markets().end());
  SWIRLY_CHECK(it->mnem() == "EURUSD.MAR14"_sv);
  SWIRLY_CHECK(it->display() == "EURUSD March 14"_sv);

  SWIRLY_CHECK(it->contr() == "EURUSD"_sv);
  SWIRLY_CHECK(it->settlDay() == SettlDay);
  SWIRLY_CHECK(it->state() == 0x1);
}

SWIRLY_FIXTURE_TEST_CASE(ServMarket, ServFixture)
{
  // Not found.
  SWIRLY_CHECK_THROW(serv.market("EURUSD.MAR14x"_sv), MarketNotFoundException);

  auto& book = serv.market("EURUSD.MAR14"_sv);
  SWIRLY_CHECK(book.mnem() == "EURUSD.MAR14"_sv);
  SWIRLY_CHECK(book.display() == "EURUSD March 14"_sv);

  SWIRLY_CHECK(book.contr() == "EURUSD"_sv);
  SWIRLY_CHECK(book.settlDay() == SettlDay);
  SWIRLY_CHECK(book.state() == 0x1);
}

SWIRLY_FIXTURE_TEST_CASE(ServCreateMarket, ServFixture)
{
  // Contr does not exist.
  SWIRLY_CHECK_THROW(
    serv.createMarket("USDJPY.MAR14"_sv, "USDJPY March 14"_sv, "USDJPYx"_sv, SettlDay, 0x1, Now),
    NotFoundException);

  // Settl-day before bus-day.
  SWIRLY_CHECK_THROW(
    serv.createMarket("USDJPY.MAR14"_sv, "USDJPY March 14"_sv, "USDJPY"_sv, Today - 1_jd, 0x1, Now),
    InvalidException);

  // Mnemonic too short.
  SWIRLY_CHECK_THROW(
    serv.createMarket("x"_sv, "USDJPY March 14"_sv, "USDJPY"_sv, SettlDay, 0x1, Now),
    InvalidException);

  // Mnemonic contains invalid characters.
  SWIRLY_CHECK_THROW(
    serv.createMarket("USDJPY/MAR14"_sv, "USDJPY March 14"_sv, "USDJPY"_sv, SettlDay, 0x1, Now),
    InvalidException);

  auto& book
    = serv.createMarket("USDJPY.MAR14"_sv, "USDJPY March 14"_sv, "USDJPY"_sv, SettlDay, 0x1, Now);

  SWIRLY_CHECK(book.mnem() == "USDJPY.MAR14"_sv);
  SWIRLY_CHECK(book.display() == "USDJPY March 14"_sv);

  SWIRLY_CHECK(book.contr() == "USDJPY"_sv);
  SWIRLY_CHECK(book.settlDay() == SettlDay);
  SWIRLY_CHECK(book.state() == 0x1);

  SWIRLY_CHECK(distance(serv.markets().begin(), serv.markets().end()) == 2);
  auto it = serv.markets().find("USDJPY.MAR14"_sv);
  SWIRLY_CHECK(it != serv.markets().end());
  SWIRLY_CHECK(&*it == &book);

  // Already exists.
  SWIRLY_CHECK_THROW(
    serv.createMarket("USDJPY.MAR14"_sv, "USDJPY March 14"_sv, "USDJPY"_sv, SettlDay, 0x1, Now),
    AlreadyExistsException);
}

SWIRLY_FIXTURE_TEST_CASE(ServUpdateMarket, ServFixture)
{
  auto& orig = serv.createMarket("USDJPY.MAR14"_sv, "first"_sv, "USDJPY"_sv, SettlDay, 0x1, Now);

  // Update display and state.
  {
    auto& book = serv.updateMarket("USDJPY.MAR14"_sv, "second"_sv, 0x2, Now);

    SWIRLY_CHECK(&book == &orig);

    SWIRLY_CHECK(book.mnem() == "USDJPY.MAR14"_sv);
    SWIRLY_CHECK(book.display() == "second"_sv);

    SWIRLY_CHECK(book.contr() == "USDJPY"_sv);
    SWIRLY_CHECK(book.settlDay() == SettlDay);
    SWIRLY_CHECK(book.state() == 0x2);

    SWIRLY_CHECK(distance(serv.markets().begin(), serv.markets().end()) == 2);
    auto it = serv.markets().find("USDJPY.MAR14"_sv);
    SWIRLY_CHECK(it != serv.markets().end());
    SWIRLY_CHECK(&*it == &book);
  }

  // Update display only.
  {
    auto& book = serv.updateMarket("USDJPY.MAR14"_sv, "third"_sv, nullopt, Now);

    SWIRLY_CHECK(&book == &orig);

    SWIRLY_CHECK(book.mnem() == "USDJPY.MAR14"_sv);
    SWIRLY_CHECK(book.display() == "third"_sv);

    SWIRLY_CHECK(book.contr() == "USDJPY"_sv);
    SWIRLY_CHECK(book.settlDay() == SettlDay);
    SWIRLY_CHECK(book.state() == 0x2);
  }

  // Update state only.
  {
    auto& book = serv.updateMarket("USDJPY.MAR14"_sv, nullopt, 0x3, Now);

    SWIRLY_CHECK(&book == &orig);

    SWIRLY_CHECK(book.mnem() == "USDJPY.MAR14"_sv);
    SWIRLY_CHECK(book.display() == "third"_sv);

    SWIRLY_CHECK(book.contr() == "USDJPY"_sv);
    SWIRLY_CHECK(book.settlDay() == SettlDay);
    SWIRLY_CHECK(book.state() == 0x3);
  }
}

SWIRLY_FIXTURE_TEST_CASE(ServCreateOrder, ServFixture)
{
  auto& accnt = serv.accnt("MARAYL"_sv);
  auto& book = serv.market("EURUSD.MAR14"_sv);

  Response resp;
  serv.createOrder(accnt, book, ""_sv, Side::Buy, 5_lts, 12345_tks, 1_lts, Now, resp);

  SWIRLY_CHECK(resp.orders().size() == 1);
  SWIRLY_CHECK(resp.execs().size() == 1);

  ConstOrderPtr order{resp.orders().front()};
  SWIRLY_CHECK(order->accnt() == accnt.mnem());
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
