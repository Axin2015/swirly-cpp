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
  const auto es = EntitySet::parse(""_sv);
  BOOST_CHECK(!es.many());
  BOOST_CHECK(!es.asset());
  BOOST_CHECK(!es.contr());
  BOOST_CHECK(!es.market());
  BOOST_CHECK(!es.trader());

  // End.
  BOOST_CHECK(es.empty());
}

BOOST_AUTO_TEST_CASE(OneCase)
{
  auto es = EntitySet::parse("asset"_sv);
  BOOST_CHECK(!es.many());
  BOOST_CHECK(es.asset());
  BOOST_CHECK(!es.contr());
  BOOST_CHECK(!es.market());
  BOOST_CHECK(!es.trader());

  // First.
  BOOST_CHECK(!es.empty());
  BOOST_CHECK_EQUAL(es.pop(), EntitySet::Asset);

  // End.
  BOOST_CHECK(es.empty());
}

BOOST_AUTO_TEST_CASE(TwoCase)
{
  auto es = EntitySet::parse("asset,contr"_sv);
  BOOST_CHECK(es.many());
  BOOST_CHECK(es.asset());
  BOOST_CHECK(es.contr());
  BOOST_CHECK(!es.market());
  BOOST_CHECK(!es.trader());

  // First.
  BOOST_CHECK(!es.empty());
  BOOST_CHECK_EQUAL(es.pop(), EntitySet::Asset);

  // Second.
  BOOST_CHECK(!es.empty());
  BOOST_CHECK_EQUAL(es.pop(), EntitySet::Contr);

  // End.
  BOOST_CHECK(es.empty());
}

BOOST_AUTO_TEST_CASE(ThreeCase)
{
  auto es = EntitySet::parse("market,contr,asset"_sv);
  BOOST_CHECK(es.many());
  BOOST_CHECK(es.asset());
  BOOST_CHECK(es.contr());
  BOOST_CHECK(es.market());
  BOOST_CHECK(!es.trader());

  // First.
  BOOST_CHECK(!es.empty());
  BOOST_CHECK_EQUAL(es.pop(), EntitySet::Asset);

  // Second.
  BOOST_CHECK(!es.empty());
  BOOST_CHECK_EQUAL(es.pop(), EntitySet::Contr);

  // Third.
  BOOST_CHECK(!es.empty());
  BOOST_CHECK_EQUAL(es.pop(), EntitySet::Market);

  // End.
  BOOST_CHECK(es.empty());
}

BOOST_AUTO_TEST_CASE(TrailingCase)
{
  auto es = EntitySet::parse("trader,"_sv);
  BOOST_CHECK(!es.many());
  BOOST_CHECK(!es.asset());
  BOOST_CHECK(!es.contr());
  BOOST_CHECK(!es.market());
  BOOST_CHECK(es.trader());

  // First.
  BOOST_CHECK(!es.empty());
  BOOST_CHECK_EQUAL(es.pop(), EntitySet::Trader);

  // End.
  BOOST_CHECK(es.empty());
}

BOOST_AUTO_TEST_CASE(BadEntityCase)
{
  BOOST_CHECK_THROW(EntitySet::parse("bad"_sv), ParseException);
}

BOOST_AUTO_TEST_SUITE_END()
