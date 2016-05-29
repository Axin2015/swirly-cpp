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
#include <swirly/elm/Rec.hpp>

#include <swirly/tea/Test.hpp>

using namespace std;
using namespace swirly;

namespace {
class Foo : public Rec {
 public:
  Foo(Mnem mnem, string_view display, int& alive) noexcept
    : Rec{RecType::Asset, mnem, display}, alive_{alive}
  {
    ++alive;
  }
  ~Foo() noexcept override { --alive_; }
  boost::intrusive::set_member_hook<> mnemHook_;

 private:
  int& alive_;
};
} // anonymous

SWIRLY_TEST_CASE(RecSet)
{
  int alive{0};
  {
    RecSet<Foo> s;

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
