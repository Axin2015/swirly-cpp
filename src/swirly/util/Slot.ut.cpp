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
#include "Slot.hpp"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {
void foo(int& x)
{
    x <<= 1;
}
} // namespace

// Test constexpr.
static_assert(BasicSlot<int>{} == BasicSlot<int>{});
static_assert(BasicSlot<int>{}.empty());
static_assert(!BasicSlot<int>{});

BOOST_AUTO_TEST_SUITE(SlotSuite)

BOOST_AUTO_TEST_CASE(SlotEmptyCase)
{
    BOOST_TEST(BasicSlot<int>{}.empty());
    BOOST_TEST(!BasicSlot<int>{});
}

BOOST_AUTO_TEST_CASE(SlotFreeFunCase)
{
    int x{2};
    auto cb = bind<foo>();
    cb(x);
    BOOST_TEST(x == 4);
    cb(x);
    BOOST_TEST(x == 8);
}

BOOST_AUTO_TEST_CASE(SlotFunctorCase)
{
    struct Test {
        void operator()() { x <<= 1; }
        int x{2};
    } t;

    auto cb = bind(&t);
    cb();
    BOOST_TEST(t.x == 4);
    cb();
    BOOST_TEST(t.x == 8);
}

BOOST_AUTO_TEST_CASE(SlotConstFunctorCase)
{
    struct Test {
        void operator()(int& x) const { x <<= 1; }
    } t;

    int x{2};
    auto cb = bind(&t);
    cb(x);
    BOOST_TEST(x == 4);
    cb(x);
    BOOST_TEST(x == 8);
}

BOOST_AUTO_TEST_CASE(SlotLambdaNullaryCase)
{
    int x{2};
    auto fn = [&x]() { x <<= 1; };

    auto cb = bind(&fn);
    cb();
    BOOST_TEST(x == 4);
    cb();
    BOOST_TEST(x == 8);
}

BOOST_AUTO_TEST_CASE(SlotLambdaUnaryCase)
{
    int x{2};
    auto fn = [](int& x) { x <<= 1; };

    auto cb = bind(&fn);
    cb(x);
    BOOST_TEST(x == 4);
    cb(x);
    BOOST_TEST(x == 8);
}

BOOST_AUTO_TEST_CASE(SlotMemFunCase)
{
    struct Test {
        void foo() { x <<= 1; }
        void bar(int n) { x += n; }
        int x{2};
    } t;

    auto cbfoo = bind<&Test::foo>(&t);
    cbfoo();
    BOOST_TEST(t.x == 4);

    auto cbbar = bind<&Test::bar>(&t);
    cbbar(3);
    BOOST_TEST(t.x == 7);
}

BOOST_AUTO_TEST_CASE(SlotConstMemFunCase)
{
    struct Test {
        void get(int& val) const { val = x; }
        int x{101};
    } t;

    auto cb = bind<&Test::get>(&t);
    int x{};
    cb(x);
    BOOST_TEST(x == t.x);
}

BOOST_AUTO_TEST_SUITE_END()
