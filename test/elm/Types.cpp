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

#include <boost/test/unit_test.hpp>

using namespace swirly;

BOOST_AUTO_TEST_SUITE(TypesSuite)

BOOST_AUTO_TEST_CASE(AssetTypeCase)
{
  BOOST_CHECK_EQUAL(enumString(AssetType::Commodity), "COMMODITY");
}

BOOST_AUTO_TEST_CASE(DirectCase)
{
  BOOST_CHECK_EQUAL(enumString(Direct::Paid), "PAID");
}

BOOST_AUTO_TEST_CASE(RecTypeCase)
{
  BOOST_CHECK_EQUAL(enumString(RecType::Asset), "ASSET");
}

BOOST_AUTO_TEST_CASE(RoleCase)
{
  BOOST_CHECK_EQUAL(enumString(Role::Maker), "MAKER");
}

BOOST_AUTO_TEST_CASE(SideCase)
{
  BOOST_CHECK_EQUAL(enumString(Side::Buy), "BUY");
}

BOOST_AUTO_TEST_CASE(StateCase)
{
  BOOST_CHECK_EQUAL(enumString(State::New), "NEW");
}

BOOST_AUTO_TEST_SUITE_END()
