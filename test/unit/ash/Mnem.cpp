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
#include <swirly/ash/Mnem.hpp>

#include <swirly/tea/Test.hpp>

#include <swirly/ash/String.hpp>

using namespace std;
using namespace swirly;

static_assert(Mnem{}.empty(), "default constructor not constexpr");
static_assert(Mnem{Mnem{}}.empty(), "copy constructor not constexpr");

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
} // anonymous

SWIRLY_TEST_CASE(MnemEmpty)
{
  Mnem mnem;
  SWIRLY_CHECK(mnem.empty());
  SWIRLY_CHECK(mnem.size() == 0UL);
  SWIRLY_CHECK(mnem.compare(""_sv) == 0);
  SWIRLY_CHECK(mnem == Mnem{""_sv});
}

SWIRLY_TEST_CASE(MnemNonEmpty)
{
  Mnem mnem{"Foo"_sv};
  SWIRLY_CHECK(!mnem.empty());
  SWIRLY_CHECK(mnem.size() == 3UL);
  SWIRLY_CHECK(mnem.compare("Foo"_sv) == 0);
  SWIRLY_CHECK(mnem == Mnem{"Foo"_sv});
}

SWIRLY_TEST_CASE(MnemUpperBound)
{
  Mnem mnem{"0123456789ABCDEFx"_sv};
  SWIRLY_CHECK(mnem.size() == MaxMnem);
  SWIRLY_CHECK(mnem == "0123456789ABCDEF"_sv);

  mnem = "0123456789abcdefx"_sv;
  SWIRLY_CHECK(mnem.size() == MaxMnem);
  SWIRLY_CHECK(mnem == "0123456789abcdef"_sv);
}

SWIRLY_TEST_CASE(MnemIndex)
{
  Mnem mnem{"Bar"_sv};
  SWIRLY_CHECK(mnem[0] == 'B');
  SWIRLY_CHECK(mnem[1] == 'a');
  SWIRLY_CHECK(mnem[2] == 'r');
  SWIRLY_CHECK(mnem.front() == 'B');
  SWIRLY_CHECK(mnem.back() == 'r');
}

SWIRLY_TEST_CASE(MnemClear)
{
  Mnem mnem{"Foo"_sv};
  mnem.clear();
  SWIRLY_CHECK(mnem.empty());
}

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
