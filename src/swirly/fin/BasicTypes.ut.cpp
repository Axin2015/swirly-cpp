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
#include "BasicTypes.hpp"

#include <boost/test/unit_test.hpp>

#include <cstring>

using namespace swirly;

BOOST_AUTO_TEST_SUITE(BasicTypesSuite)

BOOST_AUTO_TEST_CASE(AssetTypeCase)
{
    BOOST_TEST(strcmp(enumString(AssetType::Cmdty), "Cmdty") == 0);
}

BOOST_AUTO_TEST_CASE(DirectCase)
{
    BOOST_TEST(strcmp(enumString(Direct::Paid), "Paid") == 0);
}

BOOST_AUTO_TEST_CASE(LiqIndCase)
{
    BOOST_TEST(strcmp(enumString(LiqInd::Maker), "Maker") == 0);
}

BOOST_AUTO_TEST_CASE(SideCase)
{
    BOOST_TEST(strcmp(enumString(Side::Buy), "Buy") == 0);
}

BOOST_AUTO_TEST_CASE(StateCase)
{
    BOOST_TEST(strcmp(enumString(State::New), "New") == 0);
}

BOOST_AUTO_TEST_CASE(StateResdCase)
{
    BOOST_TEST(strcmp(enumString(State::Trade, 0_lts), "Complete") == 0);
    BOOST_TEST(strcmp(enumString(State::Trade, 1_lts), "Partial") == 0);
}

BOOST_AUTO_TEST_SUITE_END()
