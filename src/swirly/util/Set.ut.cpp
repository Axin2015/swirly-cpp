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
#include "Set.hpp"

#include <swirly/util/StringBuf.hpp>

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {

class Foo : public RefCount<Foo, ThreadUnsafePolicy> {
  public:
    Foo(Symbol symbol, string_view display, int& alive) noexcept
    : symbol_{symbol}
    , display_{display}
    , alive_{alive}
    {
        ++alive;
    }
    ~Foo() { --alive_; }

    auto symbol() const noexcept { return symbol_; }
    auto display() const noexcept { return +display_; }
    boost::intrusive::set_member_hook<> symbolHook;

  private:
    const Symbol symbol_;
    StringBuf<64> display_;
    int& alive_;
};

class Bar : public RefCount<Bar, ThreadUnsafePolicy> {
  public:
    Bar(Id64 id, string_view display, int& alive) noexcept
    : id_{id}
    , display_{display}
    , alive_{alive}
    {
        ++alive;
    }
    ~Bar() { --alive_; }

    auto id() const noexcept { return id_; }
    auto display() const noexcept { return +display_; }
    boost::intrusive::set_member_hook<> idHook;

  private:
    const Id64 id_;
    StringBuf<64> display_;
    int& alive_;
};

} // namespace

BOOST_AUTO_TEST_SUITE(SetSuite)

BOOST_AUTO_TEST_CASE(SymbolSetCase)
{
    int alive{0};
    {
        SymbolSet<Foo> s;

        Foo& foo1{*s.emplace("FOO"sv, "Foo One"sv, alive)};
        BOOST_TEST(alive == 1);
        BOOST_TEST(foo1.symbol() == "FOO"sv);
        BOOST_TEST(foo1.display() == "Foo One"sv);
        BOOST_TEST(s.find("FOO"sv) != s.end());

        // Duplicate.
        Foo& foo2{*s.emplace("FOO"sv, "Foo Two"sv, alive)};
        BOOST_TEST(alive == 1);
        BOOST_TEST(&foo2 == &foo1);

        // Replace.
        Foo& foo3{*s.emplaceOrReplace("FOO"sv, "Foo Three"sv, alive)};
        BOOST_TEST(alive == 1);
        BOOST_TEST(&foo3 != &foo1);
        BOOST_TEST(foo3.symbol() == "FOO"sv);
        BOOST_TEST(foo3.display() == "Foo Three"sv);
    }
    BOOST_TEST(alive == 0);
}

BOOST_AUTO_TEST_CASE(IdSetCase)
{
    int alive{0};
    {
        IdSet<Bar> s;

        Bar& bar1{*s.emplace(1_id64, "Bar One"sv, alive)};
        BOOST_TEST(alive == 1);
        BOOST_TEST(bar1.id() == 1_id64);
        BOOST_TEST(bar1.display() == "Bar One"sv);
        BOOST_TEST(s.find(1_id64) != s.end());

        // Duplicate.
        Bar& bar2{*s.emplace(1_id64, "Bar Two"sv, alive)};
        BOOST_TEST(alive == 1);
        BOOST_TEST(&bar2 == &bar1);

        // Replace.
        Bar& bar3{*s.emplaceOrReplace(1_id64, "Bar Three"sv, alive)};
        BOOST_TEST(alive == 1);
        BOOST_TEST(&bar3 != &bar1);
        BOOST_TEST(bar3.id() == 1_id64);
        BOOST_TEST(bar3.display() == "Bar Three"sv);
    }
    BOOST_TEST(alive == 0);
}

BOOST_AUTO_TEST_SUITE_END()
