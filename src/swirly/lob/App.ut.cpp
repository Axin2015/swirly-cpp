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
#include "App.hpp"

#include <swirly/lob/Response.hpp>
#include <swirly/lob/Sess.hpp>
#include <swirly/lob/Test.hpp>

#include <swirly/fin/MsgQueue.hpp>

#include <swirly/app/Exception.hpp>

#include <swirly/util/Date.hpp>
#include <swirly/util/Time.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;
namespace {

constexpr auto Today = ymd_to_jd(2014, 3, 11);
constexpr auto SettlDay = Today + 2_jd;
constexpr auto MarketId = to_market_id(1_id32, SettlDay);

constexpr auto Now = jd_to_time(Today);

class SWIRLY_API TestModel : public swirly::TestModel {
  protected:
    void do_read_market(const ModelCallback<MarketPtr>& cb) const override
    {
        cb(Market::make(MarketId, "EURUSD"sv, SettlDay, 0x1U));
    }
};

struct AppFixture {
    AppFixture() { app.load(Now, TestModel{}); }
    MsgQueue mq{1 << 10};
    App app{mq, 1 << 4};
};

} // namespace

namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(AppSuite)

BOOST_FIXTURE_TEST_CASE(AppAssets, AppFixture)
{
    BOOST_TEST(distance(app.assets().begin(), app.assets().end()) == 24);

    auto it = app.assets().find("CHF"sv);
    BOOST_TEST(it != app.assets().end());
    BOOST_TEST(it->id() == 1_id32);
    BOOST_TEST(it->symbol() == "CHF"sv);
    BOOST_TEST(it->display() == "Switzerland, Francs"sv);

    BOOST_TEST(it->type() == AssetType::Ccy);
}

BOOST_FIXTURE_TEST_CASE(AppInstrs, AppFixture, *utf::tolerance(0.0000001))
{
    BOOST_TEST(distance(app.instrs().begin(), app.instrs().end()) == 21);

    auto it = app.instrs().find("EURUSD"sv);
    BOOST_TEST(it != app.instrs().end());
    BOOST_TEST(it->id() == 1_id32);
    BOOST_TEST(it->symbol() == "EURUSD"sv);
    BOOST_TEST(it->display() == "EURUSD"sv);

    BOOST_TEST(it->base_asset() == "EUR"sv);
    BOOST_TEST(it->term_ccy() == "USD"sv);
    BOOST_TEST(it->lot_numer() == 1000000);
    BOOST_TEST(it->lot_denom() == 1);
    BOOST_TEST(it->qty_inc() == 1e6);
    BOOST_TEST(it->tick_numer() == 1);
    BOOST_TEST(it->tick_denom() == 10000);
    BOOST_TEST(it->price_inc() == 1e-4);
    BOOST_TEST(it->pip_dp() == 4);
    BOOST_TEST(it->qty_dp() == 0);
    BOOST_TEST(it->min_lots() == 1_lts);
    BOOST_TEST(it->max_lots() == 10_lts);
}

BOOST_FIXTURE_TEST_CASE(AppMarkets, AppFixture)
{
    BOOST_TEST(distance(app.markets().begin(), app.markets().end()) == 1);

    auto it = app.markets().find(MarketId);
    BOOST_TEST(it != app.markets().end());
    BOOST_TEST(it->id() == MarketId);

    BOOST_TEST(it->instr() == "EURUSD"sv);
    BOOST_TEST(it->settl_day() == SettlDay);
    BOOST_TEST(it->state() == 0x1U);
}

BOOST_FIXTURE_TEST_CASE(AppMarket, AppFixture)
{
    // Not found.
    BOOST_CHECK_THROW(app.market(1_id64), MarketNotFoundException);

    auto& market = app.market(MarketId);
    BOOST_TEST(market.id() == MarketId);

    BOOST_TEST(market.instr() == "EURUSD"sv);
    BOOST_TEST(market.settl_day() == SettlDay);
    BOOST_TEST(market.state() == 0x1U);
}

BOOST_FIXTURE_TEST_CASE(AppCreateMarket, AppFixture)
{
    const Instr& instr = app.instr("USDJPY"sv);
    const auto market_id = to_market_id(instr.id(), SettlDay);

    // Settl-day before bus-day.
    BOOST_CHECK_THROW(app.create_market(Now, instr, Today - 1_jd, 0x1), InvalidException);

    auto& market = app.create_market(Now, instr, SettlDay, 0x1);

    BOOST_TEST(market.id() == market_id);

    BOOST_TEST(market.instr() == "USDJPY"sv);
    BOOST_TEST(market.settl_day() == SettlDay);
    BOOST_TEST(market.state() == 0x1U);

    BOOST_TEST(distance(app.markets().begin(), app.markets().end()) == 2);
    auto it = app.markets().find(market_id);
    BOOST_TEST(it != app.markets().end());
    BOOST_TEST(&*it == &market);

    // Already exists.
    BOOST_CHECK_THROW(app.create_market(Now, instr, SettlDay, 0x1), AlreadyExistsException);
}

BOOST_FIXTURE_TEST_CASE(AppUpdateMarket, AppFixture)
{
    const Instr& instr = app.instr("USDJPY"sv);
    const auto market_id = to_market_id(instr.id(), SettlDay);
    auto& market = app.create_market(Now, instr, SettlDay, 0x1);

    app.update_market(Now, market, 0x2);

    BOOST_TEST(market.id() == market_id);

    BOOST_TEST(market.instr() == "USDJPY"sv);
    BOOST_TEST(market.settl_day() == SettlDay);
    BOOST_TEST(market.state() == 0x2U);
}

BOOST_FIXTURE_TEST_CASE(AppCreateOrder, AppFixture)
{
    auto& sess = app.sess("MARAYL"sv);
    const Instr& instr = app.instr("EURUSD"sv);
    const auto market_id = to_market_id(instr.id(), SettlDay);
    auto& market = app.market(market_id);

    Response resp;
    app.create_order(Now, sess, market, ""sv, Side::Buy, 5_lts, 12345_tks, 1_lts, resp);

    BOOST_TEST(resp.orders().size() == 1U);
    BOOST_TEST(resp.execs().size() == 1U);

    ConstOrderPtr order{resp.orders().front()};
    BOOST_TEST(order->market_id() == market.id());
    BOOST_TEST(order->instr() == instr.symbol());
    BOOST_TEST(order->settl_day() == SettlDay);
    BOOST_TEST(order->id() == 1_id64);
    BOOST_TEST(order->accnt() == sess.accnt());
    BOOST_TEST(order->ref().empty());
    BOOST_TEST(order->state() == State::New);
    BOOST_TEST(order->side() == Side::Buy);
    BOOST_TEST(order->lots() == 5_lts);
    BOOST_TEST(order->ticks() == 12345_tks);
    BOOST_TEST(order->resd_lots() == 5_lts);
    BOOST_TEST(order->exec_lots() == 0_lts);
    BOOST_TEST(order->exec_cost() == 0_cst);
    BOOST_TEST(order->last_lots() == 0_lts);
    BOOST_TEST(order->last_ticks() == 0_tks);
    BOOST_TEST(order->min_lots() == 1_lts);
    BOOST_TEST(order->created() == Now);
    BOOST_TEST(order->modified() == Now);
}

BOOST_AUTO_TEST_SUITE_END()
