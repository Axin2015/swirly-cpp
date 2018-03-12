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
#include "Transaction.hpp"

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

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

BOOST_AUTO_TEST_SUITE(TransactionSuite)

BOOST_AUTO_TEST_CASE(TransScopedCommitCase)
{
    Foo foo;
    {
        Transaction trans{foo};
        BOOST_TEST(!foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 1);
    BOOST_TEST(foo.rollbackCalls == 0);
    {
        Transaction trans{foo};
        BOOST_TEST(!foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 2);
    BOOST_TEST(foo.commitCalls == 2);
    BOOST_TEST(foo.rollbackCalls == 0);
}

BOOST_AUTO_TEST_CASE(TransScopedRollbackCase)
{
    Foo foo;
    {
        Transaction trans{foo};
        BOOST_TEST(!foo.failed());
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 1);
    {
        Transaction trans{foo};
        BOOST_TEST(!foo.failed());
    }
    BOOST_TEST(foo.beginCalls == 2);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 2);
}

BOOST_AUTO_TEST_CASE(TransSingleCommitCase)
{
    Foo foo;
    {
        Transaction trans{foo, More::No};
        BOOST_TEST(!foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 0);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 0);

    BOOST_TEST(!foo.failed());

    // Next.
    {
        Transaction trans{foo};
        BOOST_TEST(!foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 1);
    BOOST_TEST(foo.rollbackCalls == 0);
}

BOOST_AUTO_TEST_CASE(TransSingleRollbackCase)
{
    Foo foo;
    {
        Transaction trans{foo, More::No};
        BOOST_TEST(!foo.failed());
    }
    BOOST_TEST(foo.beginCalls == 0);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 0);

    BOOST_TEST(!foo.failed());

    // Next.
    {
        Transaction trans{foo};
        BOOST_TEST(!foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 1);
    BOOST_TEST(foo.rollbackCalls == 0);
}

BOOST_AUTO_TEST_CASE(TransMultiCommitCase)
{
    Foo foo;
    {
        Transaction trans{foo, More::Yes};
        BOOST_TEST(!foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 0);
    {
        Transaction trans{foo, More::Yes};
        BOOST_TEST(!foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 0);
    {
        Transaction trans{foo, More::No};
        BOOST_TEST(!foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 1);
    BOOST_TEST(foo.rollbackCalls == 0);

    BOOST_TEST(!foo.failed());

    // Next.
    {
        Transaction trans{foo};
        BOOST_TEST(!foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 2);
    BOOST_TEST(foo.commitCalls == 2);
    BOOST_TEST(foo.rollbackCalls == 0);
}

BOOST_AUTO_TEST_CASE(TransMultiRollbackFirstCase)
{
    Foo foo;
    {
        Transaction trans{foo, More::Yes};
        BOOST_TEST(!foo.failed());
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 1);
    {
        Transaction trans{foo, More::Yes};
        BOOST_TEST(foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 1);
    {
        Transaction trans{foo, More::No};
        BOOST_TEST(foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 1);

    BOOST_TEST(!foo.failed());

    // Next.
    {
        Transaction trans{foo};
        BOOST_TEST(!foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 2);
    BOOST_TEST(foo.commitCalls == 1);
    BOOST_TEST(foo.rollbackCalls == 1);
}

BOOST_AUTO_TEST_CASE(TransMultiRollbackSecondCase)
{
    Foo foo;
    {
        Transaction trans{foo, More::Yes};
        BOOST_TEST(!foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 0);
    {
        Transaction trans{foo, More::Yes};
        BOOST_TEST(!foo.failed());
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 1);
    {
        Transaction trans{foo, More::No};
        BOOST_TEST(foo.failed());
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 1);

    BOOST_TEST(!foo.failed());

    // Next.
    {
        Transaction trans{foo};
        BOOST_TEST(!foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 2);
    BOOST_TEST(foo.commitCalls == 1);
    BOOST_TEST(foo.rollbackCalls == 1);
}

BOOST_AUTO_TEST_CASE(TransMultiRollbackAllCase)
{
    Foo foo;
    {
        Transaction trans{foo, More::Yes};
        BOOST_TEST(!foo.failed());
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 1);
    {
        Transaction trans{foo, More::Yes};
        BOOST_TEST(foo.failed());
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 1);
    {
        Transaction trans{foo, More::No};
        BOOST_TEST(foo.failed());
    }
    BOOST_TEST(foo.beginCalls == 1);
    BOOST_TEST(foo.commitCalls == 0);
    BOOST_TEST(foo.rollbackCalls == 1);

    BOOST_TEST(!foo.failed());

    // Next.
    {
        Transaction trans{foo};
        BOOST_TEST(!foo.failed());
        trans.commit();
    }
    BOOST_TEST(foo.beginCalls == 2);
    BOOST_TEST(foo.commitCalls == 1);
    BOOST_TEST(foo.rollbackCalls == 1);
}

BOOST_AUTO_TEST_SUITE_END()
