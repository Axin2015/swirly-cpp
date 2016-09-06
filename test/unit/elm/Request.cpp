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
#include <swirly/elm/Request.hpp>

#include <swirly/tea/Test.hpp>

using namespace std;
using namespace swirly;

namespace {
class Foo : public Request {
 public:
  Foo(Id64 marketId, Id64 id, int& alive) noexcept
    : Request{{}, marketId, {}, 0_jd, id, {}, Side::Buy, 0_lts, 0_ms}, alive_{alive}
  {
    ++alive;
  }
  ~Foo() noexcept override { --alive_; }
  boost::intrusive::set_member_hook<> idHook_;

 private:
  int& alive_;
};

using FooPtr = boost::intrusive_ptr<Foo>;

} // anonymous

SWIRLY_TEST_CASE(RequestIdSet)
{
  int alive{0};
  {
    RequestIdSet<Foo> s;

    FooPtr foo1{&*s.emplace(1_id64, 2_id64, alive)};
    SWIRLY_CHECK(alive == 1);
    SWIRLY_CHECK(foo1->refs() == 2);
    SWIRLY_CHECK(foo1->marketId() == 1_id64);
    SWIRLY_CHECK(foo1->id() == 2_id64);
    SWIRLY_CHECK(s.find(1_id64, 2_id64) != s.end());

    // Duplicate.
    FooPtr foo2{&*s.emplace(1_id64, 2_id64, alive)};
    SWIRLY_CHECK(alive == 1);
    SWIRLY_CHECK(foo2->refs() == 3);
    SWIRLY_CHECK(foo2 == foo1);

    // Replace.
    FooPtr foo3{&*s.emplaceOrReplace(1_id64, 2_id64, alive)};
    SWIRLY_CHECK(alive == 2);
    SWIRLY_CHECK(foo3->refs() == 2);
    SWIRLY_CHECK(foo3 != foo1);
    SWIRLY_CHECK(foo3->marketId() == 1_id64);
    SWIRLY_CHECK(foo3->id() == 2_id64);
  }
  SWIRLY_CHECK(alive == 0);
}
