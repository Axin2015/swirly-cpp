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

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

using AccntPosnSet = IdSet<Posn, MarketIdTraits<Posn>>;

BOOST_AUTO_TEST_SUITE(PosnSuite)

BOOST_AUTO_TEST_CASE(AccntPosnSetCase)
{
    constexpr auto SettlDay = ymdToJd(2014, 2, 14);
    constexpr auto MarketId = toMarketId(1_id32, SettlDay);

    AccntPosnSet s;

    PosnPtr posn1{
        &*s.emplace("MARAYL"sv, MarketId, "EURUSD"sv, SettlDay, 0_lts, 0_cst, 0_lts, 0_cst)};
    BOOST_TEST(posn1->refCount() == 2);
    BOOST_TEST(posn1->instr() == "EURUSD"sv);
    BOOST_TEST(posn1->settlDay() == SettlDay);
    BOOST_TEST(s.find(MarketId) != s.end());

    // Duplicate.
    PosnPtr posn2{
        &*s.emplace("MARAYL"sv, MarketId, "EURUSD"sv, SettlDay, 0_lts, 0_cst, 0_lts, 0_cst)};
    BOOST_TEST(posn2->refCount() == 3);
    BOOST_TEST(posn2 == posn1);

    // Replace.
    PosnPtr posn3{&*s.emplaceOrReplace("MARAYL"sv, MarketId, "EURUSD"sv, SettlDay, 0_lts, 0_cst,
                                       0_lts, 0_cst)};
    BOOST_TEST(posn3->refCount() == 2);
    BOOST_TEST(posn3 != posn1);
    BOOST_TEST(posn3->instr() == "EURUSD"sv);
    BOOST_TEST(posn3->settlDay() == SettlDay);
}

BOOST_AUTO_TEST_SUITE_END()
