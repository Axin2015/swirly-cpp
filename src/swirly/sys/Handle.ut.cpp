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

#include <swirly/unit/Test.hpp>

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

using TestHandle = Handle<TestPolicy>;
} // namespace

SWIRLY_TEST_CASE(HandleInvalid)
{
    lastClosed = 0;

    SWIRLY_CHECK(!TestHandle{});
    SWIRLY_CHECK(TestHandle{}.get() == -1);
    SWIRLY_CHECK(lastClosed == 0);

    SWIRLY_CHECK(!TestHandle{nullptr});
    SWIRLY_CHECK(TestHandle{nullptr}.get() == -1);
    SWIRLY_CHECK(lastClosed == 0);
}

SWIRLY_TEST_CASE(HandleClose)
{
    lastClosed = 0;

    SWIRLY_CHECK(TestHandle{1});
    SWIRLY_CHECK(TestHandle{1}.get() == 1);
    SWIRLY_CHECK(lastClosed == 1);
}

SWIRLY_TEST_CASE(HandleRelease)
{
    lastClosed = 0;

    SWIRLY_CHECK(TestHandle{1}.release() == 1);
    SWIRLY_CHECK(lastClosed == 0);
}

SWIRLY_TEST_CASE(HandleReset)
{
    lastClosed = 0;
    {
        TestHandle h{1};
        h.reset(2);
        SWIRLY_CHECK(lastClosed == 1);
    }
    SWIRLY_CHECK(lastClosed == 2);
}

SWIRLY_TEST_CASE(HandleSwap)
{
    lastClosed = 0;
    {
        TestHandle h{1};
        TestHandle{2}.swap(h);
        SWIRLY_CHECK(lastClosed == 1);
    }
    SWIRLY_CHECK(lastClosed == 2);
}

SWIRLY_TEST_CASE(HandleMove)
{
    lastClosed = 0;

    TestHandle h{1};
    TestHandle{move(h)};
    SWIRLY_CHECK(lastClosed == 1);
    SWIRLY_CHECK(h.get() == -1);

    h.reset(2);
    {
        TestHandle tmp;
        tmp = move(h);
    }
    SWIRLY_CHECK(lastClosed == 2);
    SWIRLY_CHECK(h.get() == -1);
}

SWIRLY_TEST_CASE(HandleEquality)
{
    SWIRLY_CHECK(TestHandle{1} == TestHandle{1});
    SWIRLY_CHECK(TestHandle{1} != TestHandle{2});
}
