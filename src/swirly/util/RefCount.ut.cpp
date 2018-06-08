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
#include "RefCount.hpp"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {
class Foo : public RefCount<Foo, ThreadUnsafePolicy> {
  public:
    explicit Foo(int& alive) noexcept
    : alive_{alive}
    {
        ++alive;
    }
    ~Foo() { --alive_; }

  private:
    int& alive_;
};
} // namespace

BOOST_AUTO_TEST_SUITE(RefCountSuite)

BOOST_AUTO_TEST_CASE(RefCountCase)
{
    int alive{0};
    {
        auto ptr1 = make_intrusive<Foo>(alive);
        BOOST_TEST(alive == 1);
        BOOST_TEST(ptr1->ref_count() == 1);
        {
            auto ptr2 = ptr1;
            BOOST_TEST(ptr1->ref_count() == 2);
        }
        BOOST_TEST(ptr1->ref_count() == 1);
    }
    BOOST_TEST(alive == 0);
}

BOOST_AUTO_TEST_SUITE_END()
