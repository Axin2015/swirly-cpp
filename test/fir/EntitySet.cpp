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
#include <swirly/fir/EntitySet.hpp>

#include <Exception.hpp>

#include <swirly/ash/String.hpp>

#include <boost/test/unit_test.hpp>

using namespace swirly;

BOOST_AUTO_TEST_SUITE(EntitySetSuite)

BOOST_AUTO_TEST_CASE(ZeroCase)
{
  const auto set = EntitySet::parse(""_sv);
  BOOST_CHECK(!set.many());
  BOOST_CHECK(!set.asset());
  BOOST_CHECK(!set.contr());
  BOOST_CHECK(!set.market());
  BOOST_CHECK(!set.trader());

  // End.
  BOOST_CHECK(set.empty());
}

BOOST_AUTO_TEST_CASE(OneCase)
{
  auto set = EntitySet::parse("asset"_sv);
  BOOST_CHECK(!set.many());
  BOOST_CHECK(set.asset());
  BOOST_CHECK(!set.contr());
  BOOST_CHECK(!set.market());
  BOOST_CHECK(!set.trader());

  // First.
  BOOST_CHECK(!set.empty());
  BOOST_CHECK_EQUAL(set.pop(), EntitySet::Asset);

  // End.
  BOOST_CHECK(set.empty());
}

BOOST_AUTO_TEST_CASE(TwoCase)
{
  auto set = EntitySet::parse("asset,contr"_sv);
  BOOST_CHECK(set.many());
  BOOST_CHECK(set.asset());
  BOOST_CHECK(set.contr());
  BOOST_CHECK(!set.market());
  BOOST_CHECK(!set.trader());

  // First.
  BOOST_CHECK(!set.empty());
  BOOST_CHECK_EQUAL(set.pop(), EntitySet::Asset);

  // Second.
  BOOST_CHECK(!set.empty());
  BOOST_CHECK_EQUAL(set.pop(), EntitySet::Contr);

  // End.
  BOOST_CHECK(set.empty());
}

BOOST_AUTO_TEST_CASE(ThreeCase)
{
  auto set = EntitySet::parse("market,contr,asset"_sv);
  BOOST_CHECK(set.many());
  BOOST_CHECK(set.asset());
  BOOST_CHECK(set.contr());
  BOOST_CHECK(set.market());
  BOOST_CHECK(!set.trader());

  // First.
  BOOST_CHECK(!set.empty());
  BOOST_CHECK_EQUAL(set.pop(), EntitySet::Asset);

  // Second.
  BOOST_CHECK(!set.empty());
  BOOST_CHECK_EQUAL(set.pop(), EntitySet::Contr);

  // Third.
  BOOST_CHECK(!set.empty());
  BOOST_CHECK_EQUAL(set.pop(), EntitySet::Market);

  // End.
  BOOST_CHECK(set.empty());
}

BOOST_AUTO_TEST_CASE(TrailingCase)
{
  auto set = EntitySet::parse("trader,"_sv);
  BOOST_CHECK(!set.many());
  BOOST_CHECK(!set.asset());
  BOOST_CHECK(!set.contr());
  BOOST_CHECK(!set.market());
  BOOST_CHECK(set.trader());

  // First.
  BOOST_CHECK(!set.empty());
  BOOST_CHECK_EQUAL(set.pop(), EntitySet::Trader);

  // End.
  BOOST_CHECK(set.empty());
}

BOOST_AUTO_TEST_CASE(BadEntityCase)
{
  BOOST_CHECK_THROW(EntitySet::parse("bad"_sv), ParseException);
}

BOOST_AUTO_TEST_SUITE_END()
