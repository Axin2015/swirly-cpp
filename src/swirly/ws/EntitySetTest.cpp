/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#include <swirly/ws/EntitySet.hpp>

#include <swirly/fin/Exception.hpp>

#include <swirly/util/String.hpp>

#include <swirly/unit/Test.hpp>

using namespace swirly;

SWIRLY_TEST_CASE(EntitySetZero)
{
    const auto es = EntitySet::parse(""_sv);
    SWIRLY_CHECK(!es.many());
    SWIRLY_CHECK(!es.asset());
    SWIRLY_CHECK(!es.contr());
    SWIRLY_CHECK(!es.market());

    // End.
    SWIRLY_CHECK(es.empty());
}

SWIRLY_TEST_CASE(EntitySetOne)
{
    auto es = EntitySet::parse("asset"_sv);
    SWIRLY_CHECK(!es.many());
    SWIRLY_CHECK(es.asset());
    SWIRLY_CHECK(!es.contr());
    SWIRLY_CHECK(!es.market());

    // First.
    SWIRLY_CHECK(!es.empty());
    SWIRLY_CHECK(es.pop() == EntitySet::Asset);

    // End.
    SWIRLY_CHECK(es.empty());
}

SWIRLY_TEST_CASE(EntitySetTwo)
{
    auto es = EntitySet::parse("asset,contr"_sv);
    SWIRLY_CHECK(es.many());
    SWIRLY_CHECK(es.asset());
    SWIRLY_CHECK(es.contr());
    SWIRLY_CHECK(!es.market());

    // First.
    SWIRLY_CHECK(!es.empty());
    SWIRLY_CHECK(es.pop() == EntitySet::Asset);

    // Second.
    SWIRLY_CHECK(!es.empty());
    SWIRLY_CHECK(es.pop() == EntitySet::Contr);

    // End.
    SWIRLY_CHECK(es.empty());
}

SWIRLY_TEST_CASE(EntitySetThree)
{
    auto es = EntitySet::parse("market,contr,asset"_sv);
    SWIRLY_CHECK(es.many());
    SWIRLY_CHECK(es.asset());
    SWIRLY_CHECK(es.contr());
    SWIRLY_CHECK(es.market());

    // First.
    SWIRLY_CHECK(!es.empty());
    SWIRLY_CHECK(es.pop() == EntitySet::Asset);

    // Second.
    SWIRLY_CHECK(!es.empty());
    SWIRLY_CHECK(es.pop() == EntitySet::Contr);

    // Third.
    SWIRLY_CHECK(!es.empty());
    SWIRLY_CHECK(es.pop() == EntitySet::Market);

    // End.
    SWIRLY_CHECK(es.empty());
}

SWIRLY_TEST_CASE(EntitySetTrailing)
{
    auto es = EntitySet::parse("market,"_sv);
    SWIRLY_CHECK(!es.many());
    SWIRLY_CHECK(!es.asset());
    SWIRLY_CHECK(!es.contr());
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
