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
#include <swirly/ash/Set.hpp>

#include <swirly/ash/String.hpp>

#include <swirly/tea/Test.hpp>

using namespace std;
using namespace swirly;

namespace {

class Foo {
 public:
  Foo(Mnem mnem, string_view display, int& alive) noexcept
    : mnem_{mnem}, display_{display}, alive_{alive}
  {
    ++alive;
  }
  ~Foo() noexcept { --alive_; }

  auto mnem() const noexcept { return mnem_; }
  auto display() const noexcept { return +display_; }
  boost::intrusive::set_member_hook<> mnemHook_;

 private:
  const Mnem mnem_;
  String<64> display_;
  int& alive_;
};

class Bar {
 public:
  Bar(Id64 id, string_view display, int& alive) noexcept : id_{id}, display_{display}, alive_{alive}
  {
    ++alive;
  }
  ~Bar() noexcept { --alive_; }

  auto id() const noexcept { return id_; }
  auto display() const noexcept { return +display_; }
  boost::intrusive::set_member_hook<> idHook_;

 private:
  const Id64 id_;
  String<64> display_;
  int& alive_;
};

} // anonymous

SWIRLY_TEST_CASE(MnemSet)
{
  int alive{0};
  {
    MnemSet<Foo> s;

    Foo& foo1{*s.emplace("FOO"_sv, "Foo One"_sv, alive)};
    SWIRLY_CHECK(alive == 1);
    SWIRLY_CHECK(foo1.mnem() == "FOO"_sv);
    SWIRLY_CHECK(foo1.display() == "Foo One"_sv);
    SWIRLY_CHECK(s.find("FOO"_sv) != s.end());

    // Duplicate.
    Foo& foo2{*s.emplace("FOO"_sv, "Foo Two"_sv, alive)};
    SWIRLY_CHECK(alive == 1);
    SWIRLY_CHECK(&foo2 == &foo1);

    // Replace.
    Foo& foo3{*s.emplaceOrReplace("FOO"_sv, "Foo Three"_sv, alive)};
    SWIRLY_CHECK(alive == 1);
    SWIRLY_CHECK(&foo3 != &foo1);
    SWIRLY_CHECK(foo3.mnem() == "FOO"_sv);
    SWIRLY_CHECK(foo3.display() == "Foo Three"_sv);
  }
  SWIRLY_CHECK(alive == 0);
}

SWIRLY_TEST_CASE(IdSet)
{
  int alive{0};
  {
    IdSet<Bar> s;

    Bar& bar1{*s.emplace(1_id64, "Bar One"_sv, alive)};
    SWIRLY_CHECK(alive == 1);
    SWIRLY_CHECK(bar1.id() == 1_id64);
    SWIRLY_CHECK(bar1.display() == "Bar One"_sv);
    SWIRLY_CHECK(s.find(1_id64) != s.end());

    // Duplicate.
    Bar& bar2{*s.emplace(1_id64, "Bar Two"_sv, alive)};
    SWIRLY_CHECK(alive == 1);
    SWIRLY_CHECK(&bar2 == &bar1);

    // Replace.
    Bar& bar3{*s.emplaceOrReplace(1_id64, "Bar Three"_sv, alive)};
    SWIRLY_CHECK(alive == 1);
    SWIRLY_CHECK(&bar3 != &bar1);
    SWIRLY_CHECK(bar3.id() == 1_id64);
    SWIRLY_CHECK(bar3.display() == "Bar Three"_sv);
  }
  SWIRLY_CHECK(alive == 0);
}
