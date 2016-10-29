/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#include <swirly/util/MemPool.hpp>

#include <swirly/unit/Test.hpp>

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

} // anonymous

SWIRLY_TEST_CASE(MemPool)
{
  MemPool memPool;

  // Throw if no memory has been reserved.
  SWIRLY_CHECK(memPool.capacity() == 0);

  // These should all call the custom allocator with zero capacity.
  SWIRLY_CHECK_THROW(memPool.alloc(sizeof(Foo)), bad_alloc);
  SWIRLY_CHECK_THROW(memPool.alloc(sizeof(Bar)), bad_alloc);

  {
    memPool.reserve(1024);
    SWIRLY_CHECK(memPool.capacity() == 1024);
  }
  {
    memPool = MemPool{4096};
    SWIRLY_CHECK(memPool.capacity() == 4096);
  }

  char* p1{static_cast<char*>(memPool.alloc(sizeof(Foo)))};
  // Check that it's writable.
  strcpy(p1, "test");
  memPool.dealloc(p1, sizeof(Foo));

  // Check that the same address is returned.
  char* p2{static_cast<char*>(memPool.alloc(sizeof(Foo)))};
  SWIRLY_CHECK(p1 == p2);
  memPool.dealloc(p2, sizeof(Foo));
}
