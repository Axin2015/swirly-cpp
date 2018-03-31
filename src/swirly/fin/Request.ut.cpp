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
#include "Request.hpp"

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {
class Foo
: public RefCount<Foo, ThreadUnsafePolicy>
, public Request {
  public:
    Foo(Id64 marketId, Id64 id, int& alive) noexcept
    : Request{{}, marketId, {}, 0_jd, id, {}, Side::Buy, 0_lts, {}}
    , alive_{alive}
    {
        ++alive;
    }
    ~Foo() { --alive_; }
    boost::intrusive::set_member_hook<> idHook;

  private:
    int& alive_;
};

using FooPtr = boost::intrusive_ptr<Foo>;

} // namespace

BOOST_AUTO_TEST_SUITE(RequestSuite)

BOOST_AUTO_TEST_CASE(RequestIdSetCase)
{
    int alive{0};
    {
        RequestIdSet<Foo> s;

        FooPtr foo1{&*s.emplace(1_id64, 2_id64, alive)};
        BOOST_TEST(alive == 1);
        BOOST_TEST(foo1->refCount() == 2);
        BOOST_TEST(foo1->marketId() == 1_id64);
        BOOST_TEST(foo1->id() == 2_id64);
        BOOST_TEST(s.find(1_id64, 2_id64) != s.end());

        // Duplicate.
        FooPtr foo2{&*s.emplace(1_id64, 2_id64, alive)};
        BOOST_TEST(alive == 1);
        BOOST_TEST(foo2->refCount() == 3);
        BOOST_TEST(foo2 == foo1);

        // Replace.
        FooPtr foo3{&*s.emplaceOrReplace(1_id64, 2_id64, alive)};
        BOOST_TEST(alive == 2);
        BOOST_TEST(foo3->refCount() == 2);
        BOOST_TEST(foo3 != foo1);
        BOOST_TEST(foo3->marketId() == 1_id64);
        BOOST_TEST(foo3->id() == 2_id64);
    }
    BOOST_TEST(alive == 0);
}

BOOST_AUTO_TEST_SUITE_END()
