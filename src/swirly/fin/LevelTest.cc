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
#include "Level.hpp"

#include <swirly/fin/Order.hpp>

#include <swirly/unit/Test.hpp>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(LevelSet)
{
    const Order order{"MARAYL"sv, 1_id64,    "EURUSD"sv, 0_jd,      0_id64, ""sv,
                      State::New, Side::Buy, 10_lts,     12345_tks, 0_lts,  0_lts,
                      0_cst,      0_lts,     0_tks,      0_lts,     {},     {}};

    LevelSet s;

    Level& level1{*s.emplace(order)};
    SWIRLY_CHECK(level1.key() == -12345);
    SWIRLY_CHECK(s.find(Side::Buy, 12345_tks) != s.end());

    // Duplicate.
    Level& level2{*s.emplace(order)};
    SWIRLY_CHECK(&level2 == &level1);

    // Replace.
    Level& level3{*s.emplaceOrReplace(order)};
    SWIRLY_CHECK(&level3 != &level1);
    SWIRLY_CHECK(level3.key() == -12345);
}
