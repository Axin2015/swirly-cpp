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
#include "RefCounted.hpp"

#include <swirly/unit/Test.hpp>

using namespace std;
using namespace swirly;

namespace {
class Foo : public RefCounted<Foo> {
  public:
    explicit Foo(int& alive) noexcept : alive_{alive} { ++alive; }
    ~Foo() noexcept { --alive_; }

  private:
    int& alive_;
};
} // anonymous

SWIRLY_TEST_CASE(RefCounted)
{
    int alive{0};
    {
        auto ptr1 = makeRefCounted<Foo>(alive);
        SWIRLY_CHECK(alive == 1);
        SWIRLY_CHECK(ptr1->refs() == 1);
        {
            auto ptr2 = ptr1;
            SWIRLY_CHECK(ptr1->refs() == 2);
        }
        SWIRLY_CHECK(ptr1->refs() == 1);
    }
    SWIRLY_CHECK(alive == 0);
}
