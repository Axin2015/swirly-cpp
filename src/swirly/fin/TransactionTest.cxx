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
#include "Transaction.hpp"

#include <swirly/unit/Test.hpp>

using namespace std;
using namespace swirly;

namespace {

struct Foo : Transactional {

    int beginCalls{0};
    int commitCalls{0};
    int rollbackCalls{0};

    void clear() noexcept
    {
        reset();
        beginCalls = 0;
        commitCalls = 0;
        rollbackCalls = 0;
    }

  protected:
    void doBegin() override { ++beginCalls; }
    void doCommit() override { ++commitCalls; }
    void doRollback() override { ++rollbackCalls; }
};

} // namespace

SWIRLY_TEST_CASE(TransScopedCommit)
{
    Foo foo;
    {
        Transaction trans{foo};
        SWIRLY_CHECK(!foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 1);
    SWIRLY_CHECK(foo.rollbackCalls == 0);
    {
        Transaction trans{foo};
        SWIRLY_CHECK(!foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 2);
    SWIRLY_CHECK(foo.commitCalls == 2);
    SWIRLY_CHECK(foo.rollbackCalls == 0);
}

SWIRLY_TEST_CASE(TransScopedRollback)
{
    Foo foo;
    {
        Transaction trans{foo};
        SWIRLY_CHECK(!foo.failed());
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 1);
    {
        Transaction trans{foo};
        SWIRLY_CHECK(!foo.failed());
    }
    SWIRLY_CHECK(foo.beginCalls == 2);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 2);
}

SWIRLY_TEST_CASE(TransSingleCommit)
{
    Foo foo;
    {
        Transaction trans{foo, More::No};
        SWIRLY_CHECK(!foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 0);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 0);

    SWIRLY_CHECK(!foo.failed());

    // Next.
    {
        Transaction trans{foo};
        SWIRLY_CHECK(!foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 1);
    SWIRLY_CHECK(foo.rollbackCalls == 0);
}

SWIRLY_TEST_CASE(TransSingleRollback)
{
    Foo foo;
    {
        Transaction trans{foo, More::No};
        SWIRLY_CHECK(!foo.failed());
    }
    SWIRLY_CHECK(foo.beginCalls == 0);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 0);

    SWIRLY_CHECK(!foo.failed());

    // Next.
    {
        Transaction trans{foo};
        SWIRLY_CHECK(!foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 1);
    SWIRLY_CHECK(foo.rollbackCalls == 0);
}

SWIRLY_TEST_CASE(TransMultiCommit)
{
    Foo foo;
    {
        Transaction trans{foo, More::Yes};
        SWIRLY_CHECK(!foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 0);
    {
        Transaction trans{foo, More::Yes};
        SWIRLY_CHECK(!foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 0);
    {
        Transaction trans{foo, More::No};
        SWIRLY_CHECK(!foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 1);
    SWIRLY_CHECK(foo.rollbackCalls == 0);

    SWIRLY_CHECK(!foo.failed());

    // Next.
    {
        Transaction trans{foo};
        SWIRLY_CHECK(!foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 2);
    SWIRLY_CHECK(foo.commitCalls == 2);
    SWIRLY_CHECK(foo.rollbackCalls == 0);
}

SWIRLY_TEST_CASE(TransMultiRollbackFirst)
{
    Foo foo;
    {
        Transaction trans{foo, More::Yes};
        SWIRLY_CHECK(!foo.failed());
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 1);
    {
        Transaction trans{foo, More::Yes};
        SWIRLY_CHECK(foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 1);
    {
        Transaction trans{foo, More::No};
        SWIRLY_CHECK(foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 1);

    SWIRLY_CHECK(!foo.failed());

    // Next.
    {
        Transaction trans{foo};
        SWIRLY_CHECK(!foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 2);
    SWIRLY_CHECK(foo.commitCalls == 1);
    SWIRLY_CHECK(foo.rollbackCalls == 1);
}

SWIRLY_TEST_CASE(TransMultiRollbackSecond)
{
    Foo foo;
    {
        Transaction trans{foo, More::Yes};
        SWIRLY_CHECK(!foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 0);
    {
        Transaction trans{foo, More::Yes};
        SWIRLY_CHECK(!foo.failed());
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 1);
    {
        Transaction trans{foo, More::No};
        SWIRLY_CHECK(foo.failed());
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 1);

    SWIRLY_CHECK(!foo.failed());

    // Next.
    {
        Transaction trans{foo};
        SWIRLY_CHECK(!foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 2);
    SWIRLY_CHECK(foo.commitCalls == 1);
    SWIRLY_CHECK(foo.rollbackCalls == 1);
}

SWIRLY_TEST_CASE(TransMultiRollbackAll)
{
    Foo foo;
    {
        Transaction trans{foo, More::Yes};
        SWIRLY_CHECK(!foo.failed());
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 1);
    {
        Transaction trans{foo, More::Yes};
        SWIRLY_CHECK(foo.failed());
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 1);
    {
        Transaction trans{foo, More::No};
        SWIRLY_CHECK(foo.failed());
    }
    SWIRLY_CHECK(foo.beginCalls == 1);
    SWIRLY_CHECK(foo.commitCalls == 0);
    SWIRLY_CHECK(foo.rollbackCalls == 1);

    SWIRLY_CHECK(!foo.failed());

    // Next.
    {
        Transaction trans{foo};
        SWIRLY_CHECK(!foo.failed());
        trans.commit();
    }
    SWIRLY_CHECK(foo.beginCalls == 2);
    SWIRLY_CHECK(foo.commitCalls == 1);
    SWIRLY_CHECK(foo.rollbackCalls == 1);
}
