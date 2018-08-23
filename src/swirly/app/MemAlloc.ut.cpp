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
#include "MemAlloc.hpp"

#include <swirly/util/RefCount.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {

std::size_t size_{};
std::align_val_t al_{};

struct alignas(64) Foo : RefCount<Foo, ThreadUnsafePolicy> {

    static void* operator new(std::size_t size, std::align_val_t al)
    {
        size_ = size;
        al_ = al;
        return ::operator new(size, al);
    }
    static void operator delete(void* ptr, std::size_t size, std::align_val_t al) noexcept
    {
        size_ = size;
        al_ = al;
#if __cpp_sized_deallocation
        ::operator delete(ptr, size, al);
#else
        ::operator delete(ptr, al);
#endif
    }
    char data[64 + 1];
};

} // namespace

BOOST_AUTO_TEST_SUITE(MemAllocSuite)

BOOST_AUTO_TEST_CASE(MemAllocCase)
{
    {
        auto h = make_intrusive<Foo>();
        BOOST_TEST(size_ == 128);
        BOOST_TEST(static_cast<int>(al_) == 64);
        size_ = {};
        al_ = {};
    }
    BOOST_TEST(size_ == 128);
    BOOST_TEST(static_cast<int>(al_) == 64);
}

BOOST_AUTO_TEST_SUITE_END()
