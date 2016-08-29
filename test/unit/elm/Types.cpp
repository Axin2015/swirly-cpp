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
#include <swirly/elm/Types.hpp>

#include <swirly/tea/Test.hpp>

using namespace swirly;

SWIRLY_TEST_CASE(AssetType)
{
  SWIRLY_CHECK(strcmp(enumString(AssetType::Commodity), "COMMODITY") == 0);
}

SWIRLY_TEST_CASE(Direct)
{
  SWIRLY_CHECK(strcmp(enumString(Direct::Paid), "PAID") == 0);
}

SWIRLY_TEST_CASE(LiqInd)
{
  SWIRLY_CHECK(strcmp(enumString(LiqInd::Maker), "MAKER") == 0);
}

SWIRLY_TEST_CASE(Side)
{
  SWIRLY_CHECK(strcmp(enumString(Side::Buy), "BUY") == 0);
}

SWIRLY_TEST_CASE(State)
{
  SWIRLY_CHECK(strcmp(enumString(State::New), "NEW") == 0);
}
