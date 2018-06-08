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
#include "MemCtx.hpp"

#include <boost/test/unit_test.hpp>

#include <cstring>

using namespace std;
using namespace swirly;

namespace {

struct Foo {
    char data[2 << 6];
};

struct Bar {
    char data[5 << 6];
};

} // namespace

BOOST_AUTO_TEST_SUITE(MemCtxSuite)

BOOST_AUTO_TEST_CASE(MemCtxCase)
{
    MemCtx mem_ctx{0};

    // Throw if no memory has been reserved.
    BOOST_TEST(mem_ctx.max_size() == 0U);

    // These should all call the custom allocator with zero capacity.
    BOOST_CHECK_THROW(mem_ctx.alloc(sizeof(Foo)), bad_alloc);
    BOOST_CHECK_THROW(mem_ctx.alloc(sizeof(Bar)), bad_alloc);

    {
        mem_ctx = MemCtx{4096};
        BOOST_TEST(mem_ctx.max_size() == 4096U);
    }

    char* p1{static_cast<char*>(mem_ctx.alloc(sizeof(Foo)))};
    // Check that it's writable.
    strcpy(p1, "test");
    mem_ctx.dealloc(p1, sizeof(Foo));

    // Check that the same address is returned.
    char* p2{static_cast<char*>(mem_ctx.alloc(sizeof(Foo)))};
    BOOST_TEST(p1 == p2);
    mem_ctx.dealloc(p2, sizeof(Foo));
}

BOOST_AUTO_TEST_SUITE_END()
