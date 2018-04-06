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
#include "Serv.hpp"

#include <swirly/clob/Accnt.hpp>
#include <swirly/clob/Response.hpp>
#include <swirly/clob/Test.hpp>

#include <swirly/fin/Exception.hpp>

#include <swirly/util/Date.hpp>

#include <swirly/sys/Time.hpp>

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;
namespace {

constexpr auto Today = ymdToJd(2014, 3, 11);
constexpr auto SettlDay = Today + 2_jd;
constexpr auto MarketId = toMarketId(1_id32, SettlDay);

constexpr auto Now = jdToTime(Today);

class SWIRLY_API TestModel : public swirly::TestModel {
  protected:
    void doReadMarket(const ModelCallback<MarketPtr>& cb) const override
    {
        cb(Market::make(MarketId, "SWIRLY"sv, "EURUSD"sv, SettlDay, 0x1U));
    }
};

struct ServFixture {
    ServFixture()
    : serv{"SWIRLY"sv, journ, 1 << 10, 1 << 4}
    {
        serv.load(TestModel{}, Now);
    }
    TestJourn journ;
    Serv serv;
};

} // namespace

namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(ServSuite)

BOOST_FIXTURE_TEST_CASE(ServAssets, ServFixture)
{
    BOOST_TEST(distance(serv.assets().begin(), serv.assets().end()) == 24);

    auto it = serv.assets().find("CHF"sv);
    BOOST_TEST(it != serv.assets().end());
    BOOST_TEST(it->id() == 1_id32);
    BOOST_TEST(it->symbol() == "CHF"sv);
    BOOST_TEST(it->display() == "Switzerland, Francs"sv);

    BOOST_TEST(it->type() == AssetType::Ccy);
}

BOOST_FIXTURE_TEST_CASE(ServInstrs, ServFixture, *utf::tolerance(0.0000001))
{
    BOOST_TEST(distance(serv.instrs().begin(), serv.instrs().end()) == 21);

    auto it = serv.instrs().find("EURUSD"sv);
    BOOST_TEST(it != serv.instrs().end());
    BOOST_TEST(it->id() == 1_id32);
    BOOST_TEST(it->symbol() == "EURUSD"sv);
    BOOST_TEST(it->display() == "EURUSD"sv);

    BOOST_TEST(it->baseAsset() == "EUR"sv);
    BOOST_TEST(it->termCcy() == "USD"sv);
    BOOST_TEST(it->lotNumer() == 1000000);
    BOOST_TEST(it->lotDenom() == 1);
    BOOST_TEST(it->qtyInc() == 1e6);
    BOOST_TEST(it->tickNumer() == 1);
    BOOST_TEST(it->tickDenom() == 10000);
    BOOST_TEST(it->priceInc() == 1e-4);
    BOOST_TEST(it->pipDp() == 4);
    BOOST_TEST(it->qtyDp() == 0);
    BOOST_TEST(it->minLots() == 1_lts);
    BOOST_TEST(it->maxLots() == 10_lts);
}

BOOST_FIXTURE_TEST_CASE(ServMarkets, ServFixture)
{
    BOOST_TEST(distance(serv.markets().begin(), serv.markets().end()) == 1);

    auto it = serv.markets().find(MarketId);
    BOOST_TEST(it != serv.markets().end());
    BOOST_TEST(it->id() == MarketId);

    BOOST_TEST(it->broker() == "SWIRLY"sv);
    BOOST_TEST(it->instr() == "EURUSD"sv);
    BOOST_TEST(it->settlDay() == SettlDay);
    BOOST_TEST(it->state() == 0x1U);
}

BOOST_FIXTURE_TEST_CASE(ServMarket, ServFixture)
{
    // Not found.
    BOOST_CHECK_THROW(serv.market(1_id64), MarketNotFoundException);

    auto& market = serv.market(MarketId);
    BOOST_TEST(market.id() == MarketId);

    BOOST_TEST(market.broker() == "SWIRLY"sv);
    BOOST_TEST(market.instr() == "EURUSD"sv);
    BOOST_TEST(market.settlDay() == SettlDay);
    BOOST_TEST(market.state() == 0x1U);
}

BOOST_FIXTURE_TEST_CASE(ServCreateMarket, ServFixture)
{
    const Instr& instr = serv.instr("USDJPY"sv);
    const auto marketId = toMarketId(instr.id(), SettlDay);

    // Settl-day before bus-day.
    BOOST_CHECK_THROW(serv.createMarket(instr, Today - 1_jd, 0x1, Now), InvalidException);

    auto& market = serv.createMarket(instr, SettlDay, 0x1, Now);

    BOOST_TEST(market.id() == marketId);

    BOOST_TEST(market.broker() == "SWIRLY"sv);
    BOOST_TEST(market.instr() == "USDJPY"sv);
    BOOST_TEST(market.settlDay() == SettlDay);
    BOOST_TEST(market.state() == 0x1U);

    BOOST_TEST(distance(serv.markets().begin(), serv.markets().end()) == 2);
    auto it = serv.markets().find(marketId);
    BOOST_TEST(it != serv.markets().end());
    BOOST_TEST(&*it == &market);

    // Already exists.
    BOOST_CHECK_THROW(serv.createMarket(instr, SettlDay, 0x1, Now), AlreadyExistsException);
}

BOOST_FIXTURE_TEST_CASE(ServUpdateMarket, ServFixture)
{
    const Instr& instr = serv.instr("USDJPY"sv);
    const auto marketId = toMarketId(instr.id(), SettlDay);
    auto& market = serv.createMarket(instr, SettlDay, 0x1, Now);

    serv.updateMarket(market, 0x2, Now);

    BOOST_TEST(market.id() == marketId);

    BOOST_TEST(market.broker() == "SWIRLY"sv);
    BOOST_TEST(market.instr() == "USDJPY"sv);
    BOOST_TEST(market.settlDay() == SettlDay);
    BOOST_TEST(market.state() == 0x2U);
}

BOOST_FIXTURE_TEST_CASE(ServCreateOrder, ServFixture)
{
    auto& accnt = serv.accnt("MARAYL"sv);
    const Instr& instr = serv.instr("EURUSD"sv);
    const auto marketId = toMarketId(instr.id(), SettlDay);
    auto& market = serv.market(marketId);

    Response resp;
    serv.createOrder(accnt, market, ""sv, Side::Buy, 5_lts, 12345_tks, 1_lts, Now, resp);

    BOOST_TEST(resp.orders().size() == 1U);
    BOOST_TEST(resp.execs().size() == 1U);

    ConstOrderPtr order{resp.orders().front()};
    BOOST_TEST(order->marketId() == market.id());
    BOOST_TEST(order->instr() == instr.symbol());
    BOOST_TEST(order->settlDay() == SettlDay);
    BOOST_TEST(order->id() == 1_id64);
    BOOST_TEST(order->accnt() == accnt.symbol());
    BOOST_TEST(order->ref().empty());
    BOOST_TEST(order->state() == State::New);
    BOOST_TEST(order->side() == Side::Buy);
    BOOST_TEST(order->lots() == 5_lts);
    BOOST_TEST(order->ticks() == 12345_tks);
    BOOST_TEST(order->resdLots() == 5_lts);
    BOOST_TEST(order->execLots() == 0_lts);
    BOOST_TEST(order->execCost() == 0_cst);
    BOOST_TEST(order->lastLots() == 0_lts);
    BOOST_TEST(order->lastTicks() == 0_tks);
    BOOST_TEST(order->minLots() == 1_lts);
    BOOST_TEST(order->created() == Now);
    BOOST_TEST(order->modified() == Now);
}

BOOST_AUTO_TEST_SUITE_END()
