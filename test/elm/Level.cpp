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
#include <swirly/elm/Level.hpp>

#include <swirly/elm/Order.hpp>

#include <test/Test.hpp>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(LevelSet)
{
  const Order order{"MARAYL",   "EURUSD",  "EURUSD", 0_jd,      0_id,  "",
                    State::New, Side::Buy, 10_lts,   12345_tks, 0_lts, 0_lts,
                    0_cst,      0_lts,     0_tks,    0_lts,     0_ms,  0_ms};

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
