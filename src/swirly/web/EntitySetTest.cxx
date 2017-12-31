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

#include <swirly/unit/Test.hpp>

using namespace swirly;

SWIRLY_TEST_CASE(EntitySetZero)
{
    const auto es = EntitySet::parse(""_sv);
    SWIRLY_CHECK(!es.many());
    SWIRLY_CHECK(!es.asset());
    SWIRLY_CHECK(!es.instr());
    SWIRLY_CHECK(!es.market());

    // End.
    SWIRLY_CHECK(es.empty());
}

SWIRLY_TEST_CASE(EntitySetOne)
{
    auto es = EntitySet::parse("assets"_sv);
    SWIRLY_CHECK(!es.many());
    SWIRLY_CHECK(es.asset());
    SWIRLY_CHECK(!es.instr());
    SWIRLY_CHECK(!es.market());

    // First.
    SWIRLY_CHECK(!es.empty());
    SWIRLY_CHECK(es.pop() == EntitySet::Asset);

    // End.
    SWIRLY_CHECK(es.empty());
}

SWIRLY_TEST_CASE(EntitySetTwo)
{
    auto es = EntitySet::parse("assets,instrs"_sv);
    SWIRLY_CHECK(es.many());
    SWIRLY_CHECK(es.asset());
    SWIRLY_CHECK(es.instr());
    SWIRLY_CHECK(!es.market());

    // First.
    SWIRLY_CHECK(!es.empty());
    SWIRLY_CHECK(es.pop() == EntitySet::Asset);

    // Second.
    SWIRLY_CHECK(!es.empty());
    SWIRLY_CHECK(es.pop() == EntitySet::Instr);

    // End.
    SWIRLY_CHECK(es.empty());
}

SWIRLY_TEST_CASE(EntitySetThree)
{
    auto es = EntitySet::parse("markets,instrs,assets"_sv);
    SWIRLY_CHECK(es.many());
    SWIRLY_CHECK(es.asset());
    SWIRLY_CHECK(es.instr());
    SWIRLY_CHECK(es.market());

    // First.
    SWIRLY_CHECK(!es.empty());
    SWIRLY_CHECK(es.pop() == EntitySet::Asset);

    // Second.
    SWIRLY_CHECK(!es.empty());
    SWIRLY_CHECK(es.pop() == EntitySet::Instr);

    // Third.
    SWIRLY_CHECK(!es.empty());
    SWIRLY_CHECK(es.pop() == EntitySet::Market);

    // End.
    SWIRLY_CHECK(es.empty());
}

SWIRLY_TEST_CASE(EntitySetTrailing)
{
    auto es = EntitySet::parse("markets,"_sv);
    SWIRLY_CHECK(!es.many());
    SWIRLY_CHECK(!es.asset());
    SWIRLY_CHECK(!es.instr());
    SWIRLY_CHECK(es.market());

    // First.
    SWIRLY_CHECK(!es.empty());
    SWIRLY_CHECK(es.pop() == EntitySet::Market);

    // End.
    SWIRLY_CHECK(es.empty());
}

SWIRLY_TEST_CASE(EntitySetBadEntity)
{
    SWIRLY_CHECK_THROW(EntitySet::parse("bad"_sv), NotFoundException);
}
