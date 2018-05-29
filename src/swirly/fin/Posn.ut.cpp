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
    constexpr auto SettlDay = ymdToJd(2014, 3, 14);
    constexpr auto MarketId = toMarketId(1_id32, SettlDay);

    using PosnSet = IdSet<Posn, MarketIdTraits<Posn>>;
    PosnSet s;

    PosnPtr posn1{&*s.emplace("MARAYL"sv, MarketId, "EURUSD"sv, SettlDay)};
    BOOST_TEST(posn1->refCount() == 2);
    BOOST_TEST(posn1->instr() == "EURUSD"sv);
    BOOST_TEST(posn1->settlDay() == SettlDay);
    BOOST_TEST(s.find(MarketId) != s.end());

    // Duplicate.
    PosnPtr posn2{&*s.emplace("MARAYL"sv, MarketId, "EURUSD"sv, SettlDay)};
    BOOST_TEST(posn2->refCount() == 3);
    BOOST_TEST(posn2 == posn1);

    // Replace.
    PosnPtr posn3{&*s.emplaceOrReplace("MARAYL"sv, MarketId, "EURUSD"sv, SettlDay)};
    BOOST_TEST(posn3->refCount() == 2);
    BOOST_TEST(posn3 != posn1);
    BOOST_TEST(posn3->instr() == "EURUSD"sv);
    BOOST_TEST(posn3->settlDay() == SettlDay);
}

BOOST_AUTO_TEST_CASE(PosnCloseLong)
{
    constexpr auto SettlDay = ymdToJd(2014, 3, 14);
    constexpr auto MarketId = toMarketId(1_id32, SettlDay);

    Posn posn{"MARAYL"sv, MarketId, "EURUSD"sv, SettlDay};

    posn.addTrade(Side::Buy, 10_lts, 2_tks);
    BOOST_TEST(posn.buyLots() == 10_lts);
    BOOST_TEST(posn.buyCost() == 20_cst);
    BOOST_TEST(posn.sellLots() == 0_lts);
    BOOST_TEST(posn.sellCost() == 0_cst);
    BOOST_TEST(posn.netLots() == 10_lts);
    BOOST_TEST(posn.netCost() == 20_cst);

    posn.addTrade(Side::Buy, 5_lts, 3_tks);
    BOOST_TEST(posn.buyLots() == 15_lts);
    BOOST_TEST(posn.buyCost() == 35_cst);
    BOOST_TEST(posn.sellLots() == 0_lts);
    BOOST_TEST(posn.sellCost() == 0_cst);
    BOOST_TEST(posn.netLots() == 15_lts);
    BOOST_TEST(posn.netCost() == 35_cst);

    posn.addTrade(Side::Sell, 5_lts, 2_tks);
    BOOST_TEST(posn.buyLots() == 15_lts);
    BOOST_TEST(posn.buyCost() == 35_cst);
    BOOST_TEST(posn.sellLots() == 5_lts);
    BOOST_TEST(posn.sellCost() == 10_cst);
    BOOST_TEST(posn.netLots() == 10_lts);
    BOOST_TEST(posn.netCost() == 25_cst);

    posn.addTrade(Side::Sell, 15_lts, 3_tks);
    BOOST_TEST(posn.buyLots() == 15_lts);
    BOOST_TEST(posn.buyCost() == 35_cst);
    BOOST_TEST(posn.sellLots() == 20_lts);
    BOOST_TEST(posn.sellCost() == 55_cst);
    BOOST_TEST(posn.netLots() == -5_lts);
    BOOST_TEST(posn.netCost() == -15_cst);
}

BOOST_AUTO_TEST_CASE(PosnCloseShort)
{
    constexpr auto SettlDay = ymdToJd(2014, 3, 14);
    constexpr auto MarketId = toMarketId(1_id32, SettlDay);

    Posn posn{"MARAYL"sv, MarketId, "EURUSD"sv, SettlDay};

    posn.addTrade(Side::Sell, 10_lts, 2_tks);
    BOOST_TEST(posn.buyLots() == 0_lts);
    BOOST_TEST(posn.buyCost() == 0_cst);
    BOOST_TEST(posn.sellLots() == 10_lts);
    BOOST_TEST(posn.sellCost() == 20_cst);
    BOOST_TEST(posn.netLots() == -10_lts);
    BOOST_TEST(posn.netCost() == -20_cst);

    posn.addTrade(Side::Sell, 5_lts, 3_tks);
    BOOST_TEST(posn.buyLots() == 0_lts);
    BOOST_TEST(posn.buyCost() == 0_cst);
    BOOST_TEST(posn.sellLots() == 15_lts);
    BOOST_TEST(posn.sellCost() == 35_cst);
    BOOST_TEST(posn.netLots() == -15_lts);
    BOOST_TEST(posn.netCost() == -35_cst);

    posn.addTrade(Side::Buy, 5_lts, 2_tks);
    BOOST_TEST(posn.buyLots() == 5_lts);
    BOOST_TEST(posn.buyCost() == 10_cst);
    BOOST_TEST(posn.sellLots() == 15_lts);
    BOOST_TEST(posn.sellCost() == 35_cst);
    BOOST_TEST(posn.netLots() == -10_lts);
    BOOST_TEST(posn.netCost() == -25_cst);

    posn.addTrade(Side::Buy, 15_lts, 3_tks);
    BOOST_TEST(posn.buyLots() == 20_lts);
    BOOST_TEST(posn.buyCost() == 55_cst);
    BOOST_TEST(posn.sellLots() == 15_lts);
    BOOST_TEST(posn.sellCost() == 35_cst);
    BOOST_TEST(posn.netLots() == 5_lts);
    BOOST_TEST(posn.netCost() == 15_cst);
}

BOOST_AUTO_TEST_SUITE_END()
