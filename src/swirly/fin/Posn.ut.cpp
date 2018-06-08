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
#include "Posn.hpp"

#include <swirly/fin/MarketId.hpp>

#include <swirly/util/Date.hpp>
#include <swirly/util/Set.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(PosnSuite)

BOOST_AUTO_TEST_CASE(PosnSetCase)
{
    constexpr auto SettlDay = ymd_to_jd(2014, 3, 14);
    constexpr auto MarketId = to_market_id(1_id32, SettlDay);

    using PosnSet = IdSet<Posn, MarketIdTraits<Posn>>;
    PosnSet s;

    PosnPtr posn1{&*s.emplace("MARAYL"sv, MarketId, "EURUSD"sv, SettlDay)};
    BOOST_TEST(posn1->ref_count() == 2);
    BOOST_TEST(posn1->instr() == "EURUSD"sv);
    BOOST_TEST(posn1->settl_day() == SettlDay);
    BOOST_TEST(s.find(MarketId) != s.end());

    // Duplicate.
    PosnPtr posn2{&*s.emplace("MARAYL"sv, MarketId, "EURUSD"sv, SettlDay)};
    BOOST_TEST(posn2->ref_count() == 3);
    BOOST_TEST(posn2 == posn1);

    // Replace.
    PosnPtr posn3{&*s.emplace_or_replace("MARAYL"sv, MarketId, "EURUSD"sv, SettlDay)};
    BOOST_TEST(posn3->ref_count() == 2);
    BOOST_TEST(posn3 != posn1);
    BOOST_TEST(posn3->instr() == "EURUSD"sv);
    BOOST_TEST(posn3->settl_day() == SettlDay);
}

BOOST_AUTO_TEST_CASE(PosnCloseLong)
{
    constexpr auto SettlDay = ymd_to_jd(2014, 3, 14);
    constexpr auto MarketId = to_market_id(1_id32, SettlDay);

    Posn posn{"MARAYL"sv, MarketId, "EURUSD"sv, SettlDay};

    posn.add_trade(Side::Buy, 10_lts, 2_tks);
    BOOST_TEST(posn.buy_lots() == 10_lts);
    BOOST_TEST(posn.buy_cost() == 20_cst);
    BOOST_TEST(posn.sell_lots() == 0_lts);
    BOOST_TEST(posn.sell_cost() == 0_cst);
    BOOST_TEST(posn.net_lots() == 10_lts);
    BOOST_TEST(posn.net_cost() == 20_cst);

    posn.add_trade(Side::Buy, 5_lts, 3_tks);
    BOOST_TEST(posn.buy_lots() == 15_lts);
    BOOST_TEST(posn.buy_cost() == 35_cst);
    BOOST_TEST(posn.sell_lots() == 0_lts);
    BOOST_TEST(posn.sell_cost() == 0_cst);
    BOOST_TEST(posn.net_lots() == 15_lts);
    BOOST_TEST(posn.net_cost() == 35_cst);

    posn.add_trade(Side::Sell, 5_lts, 2_tks);
    BOOST_TEST(posn.buy_lots() == 15_lts);
    BOOST_TEST(posn.buy_cost() == 35_cst);
    BOOST_TEST(posn.sell_lots() == 5_lts);
    BOOST_TEST(posn.sell_cost() == 10_cst);
    BOOST_TEST(posn.net_lots() == 10_lts);
    BOOST_TEST(posn.net_cost() == 25_cst);

    posn.add_trade(Side::Sell, 15_lts, 3_tks);
    BOOST_TEST(posn.buy_lots() == 15_lts);
    BOOST_TEST(posn.buy_cost() == 35_cst);
    BOOST_TEST(posn.sell_lots() == 20_lts);
    BOOST_TEST(posn.sell_cost() == 55_cst);
    BOOST_TEST(posn.net_lots() == -5_lts);
    BOOST_TEST(posn.net_cost() == -15_cst);
}

BOOST_AUTO_TEST_CASE(PosnCloseShort)
{
    constexpr auto SettlDay = ymd_to_jd(2014, 3, 14);
    constexpr auto MarketId = to_market_id(1_id32, SettlDay);

    Posn posn{"MARAYL"sv, MarketId, "EURUSD"sv, SettlDay};

    posn.add_trade(Side::Sell, 10_lts, 2_tks);
    BOOST_TEST(posn.buy_lots() == 0_lts);
    BOOST_TEST(posn.buy_cost() == 0_cst);
    BOOST_TEST(posn.sell_lots() == 10_lts);
    BOOST_TEST(posn.sell_cost() == 20_cst);
    BOOST_TEST(posn.net_lots() == -10_lts);
    BOOST_TEST(posn.net_cost() == -20_cst);

    posn.add_trade(Side::Sell, 5_lts, 3_tks);
    BOOST_TEST(posn.buy_lots() == 0_lts);
    BOOST_TEST(posn.buy_cost() == 0_cst);
    BOOST_TEST(posn.sell_lots() == 15_lts);
    BOOST_TEST(posn.sell_cost() == 35_cst);
    BOOST_TEST(posn.net_lots() == -15_lts);
    BOOST_TEST(posn.net_cost() == -35_cst);

    posn.add_trade(Side::Buy, 5_lts, 2_tks);
    BOOST_TEST(posn.buy_lots() == 5_lts);
    BOOST_TEST(posn.buy_cost() == 10_cst);
    BOOST_TEST(posn.sell_lots() == 15_lts);
    BOOST_TEST(posn.sell_cost() == 35_cst);
    BOOST_TEST(posn.net_lots() == -10_lts);
    BOOST_TEST(posn.net_cost() == -25_cst);

    posn.add_trade(Side::Buy, 15_lts, 3_tks);
    BOOST_TEST(posn.buy_lots() == 20_lts);
    BOOST_TEST(posn.buy_cost() == 55_cst);
    BOOST_TEST(posn.sell_lots() == 15_lts);
    BOOST_TEST(posn.sell_cost() == 35_cst);
    BOOST_TEST(posn.net_lots() == 5_lts);
    BOOST_TEST(posn.net_cost() == 15_cst);
}

BOOST_AUTO_TEST_SUITE_END()
