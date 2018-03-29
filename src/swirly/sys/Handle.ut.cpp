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
#include "Handle.hpp"

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

#include <iostream>

using namespace std;
using namespace swirly;

namespace {
int lastClosed{};

struct TestPolicy {
    using Id = int;
    static constexpr int invalid() noexcept { return -1; }
    static void close(int id) noexcept { lastClosed = id; }
};

using TestHandle = BasicHandle<TestPolicy>;

ostream& operator<<(ostream& os, const TestHandle& h)
{
    return os << h.get();
}

} // namespace

BOOST_AUTO_TEST_SUITE(HandleSuite)

BOOST_AUTO_TEST_CASE(HandleInvalidCase)
{
    lastClosed = 0;

    BOOST_TEST(!TestHandle{});
    BOOST_TEST(TestHandle{}.get() == -1);
    BOOST_TEST(lastClosed == 0);

    BOOST_TEST(!TestHandle{nullptr});
    BOOST_TEST(TestHandle{nullptr}.get() == -1);
    BOOST_TEST(lastClosed == 0);
}

BOOST_AUTO_TEST_CASE(HandleCloseCase)
{
    lastClosed = 0;

    BOOST_TEST(TestHandle{1});
    BOOST_TEST(TestHandle{1}.get() == 1);
    BOOST_TEST(lastClosed == 1);
}

BOOST_AUTO_TEST_CASE(HandleReleaseCase)
{
    lastClosed = 0;

    BOOST_TEST(TestHandle{1}.release() == 1);
    BOOST_TEST(lastClosed == 0);
}

BOOST_AUTO_TEST_CASE(HandleResetCase)
{
    lastClosed = 0;
    {
        TestHandle h{1};
        h.reset(2);
        BOOST_TEST(lastClosed == 1);
    }
    BOOST_TEST(lastClosed == 2);
}

BOOST_AUTO_TEST_CASE(HandleSwapCase)
{
    lastClosed = 0;
    {
        TestHandle h{1};
        TestHandle{2}.swap(h);
        BOOST_TEST(lastClosed == 1);
    }
    BOOST_TEST(lastClosed == 2);
}

BOOST_AUTO_TEST_CASE(HandleMoveCase)
{
    lastClosed = 0;

    TestHandle h{1};
    TestHandle{move(h)};
    BOOST_TEST(lastClosed == 1);
    BOOST_TEST(h.get() == -1);

    h.reset(2);
    {
        TestHandle tmp;
        tmp = move(h);
    }
    BOOST_TEST(lastClosed == 2);
    BOOST_TEST(h.get() == -1);
}

BOOST_AUTO_TEST_CASE(HandleEqualityCase)
{
    BOOST_TEST(TestHandle{1} == TestHandle{1});
    BOOST_TEST(TestHandle{1} != TestHandle{2});
}

BOOST_AUTO_TEST_SUITE_END()
