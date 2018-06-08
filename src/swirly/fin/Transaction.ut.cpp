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

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {

struct Foo {

    int begin_calls{0};
    int commit_calls{0};
    int rollback_calls{0};

    void clear() noexcept
    {
        begin_calls = 0;
        commit_calls = 0;
        rollback_calls = 0;
    }

    void begin() { ++begin_calls; }
    void commit() { ++commit_calls; }
    void rollback() noexcept { ++rollback_calls; }
};

using Transaction = BasicTransaction<Foo>;

} // namespace

BOOST_AUTO_TEST_SUITE(TransactionSuite)

BOOST_AUTO_TEST_CASE(TransScopedCommitCase)
{
    Foo foo;
    {
        Transaction trans{foo};
        trans.commit();
    }
    BOOST_TEST(foo.begin_calls == 1);
    BOOST_TEST(foo.commit_calls == 1);
    BOOST_TEST(foo.rollback_calls == 0);
    {
        Transaction trans{foo};
        trans.commit();
    }
    BOOST_TEST(foo.begin_calls == 2);
    BOOST_TEST(foo.commit_calls == 2);
    BOOST_TEST(foo.rollback_calls == 0);
}

BOOST_AUTO_TEST_CASE(TransScopedRollbackCase)
{
    Foo foo;
    {
        Transaction trans{foo};
    }
    BOOST_TEST(foo.begin_calls == 1);
    BOOST_TEST(foo.commit_calls == 0);
    BOOST_TEST(foo.rollback_calls == 1);
    {
        Transaction trans{foo};
    }
    BOOST_TEST(foo.begin_calls == 2);
    BOOST_TEST(foo.commit_calls == 0);
    BOOST_TEST(foo.rollback_calls == 2);
}

BOOST_AUTO_TEST_SUITE_END()
