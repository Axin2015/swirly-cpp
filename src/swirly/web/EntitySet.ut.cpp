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
#include "EntitySet.hpp"

#include <swirly/fin/Exception.hpp>

#include <swirly/util/String.hpp>

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

using namespace swirly;

BOOST_AUTO_TEST_SUITE(EntitySetSuite)

BOOST_AUTO_TEST_CASE(EntitySetZeroCase)
{
    const auto es = EntitySet::parse(""sv);
    BOOST_TEST(!es.many());
    BOOST_TEST(!es.asset());
    BOOST_TEST(!es.instr());
    BOOST_TEST(!es.market());

    // End.
    BOOST_TEST(es.empty());
}

BOOST_AUTO_TEST_CASE(EntitySetOneCase)
{
    auto es = EntitySet::parse("assets"sv);
    BOOST_TEST(!es.many());
    BOOST_TEST(es.asset());
    BOOST_TEST(!es.instr());
    BOOST_TEST(!es.market());

    // First.
    BOOST_TEST(!es.empty());
    BOOST_TEST(es.pop() == EntitySet::Asset);

    // End.
    BOOST_TEST(es.empty());
}

BOOST_AUTO_TEST_CASE(EntitySetTwoCase)
{
    auto es = EntitySet::parse("assets,instrs"sv);
    BOOST_TEST(es.many());
    BOOST_TEST(es.asset());
    BOOST_TEST(es.instr());
    BOOST_TEST(!es.market());

    // First.
    BOOST_TEST(!es.empty());
    BOOST_TEST(es.pop() == EntitySet::Asset);

    // Second.
    BOOST_TEST(!es.empty());
    BOOST_TEST(es.pop() == EntitySet::Instr);

    // End.
    BOOST_TEST(es.empty());
}

BOOST_AUTO_TEST_CASE(EntitySetThreeCase)
{
    auto es = EntitySet::parse("markets,instrs,assets"sv);
    BOOST_TEST(es.many());
    BOOST_TEST(es.asset());
    BOOST_TEST(es.instr());
    BOOST_TEST(es.market());

    // First.
    BOOST_TEST(!es.empty());
    BOOST_TEST(es.pop() == EntitySet::Asset);

    // Second.
    BOOST_TEST(!es.empty());
    BOOST_TEST(es.pop() == EntitySet::Instr);

    // Third.
    BOOST_TEST(!es.empty());
    BOOST_TEST(es.pop() == EntitySet::Market);

    // End.
    BOOST_TEST(es.empty());
}

BOOST_AUTO_TEST_CASE(EntitySetTrailingCase)
{
    auto es = EntitySet::parse("markets,"sv);
    BOOST_TEST(!es.many());
    BOOST_TEST(!es.asset());
    BOOST_TEST(!es.instr());
    BOOST_TEST(es.market());

    // First.
    BOOST_TEST(!es.empty());
    BOOST_TEST(es.pop() == EntitySet::Market);

    // End.
    BOOST_TEST(es.empty());
}

BOOST_AUTO_TEST_CASE(EntitySetBadEntityCase)
{
    BOOST_CHECK_THROW(EntitySet::parse("bad"sv), NotFoundException);
}

BOOST_AUTO_TEST_SUITE_END()
