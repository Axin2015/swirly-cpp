/*
 * Swirly Order-Book and Matching-Engine.
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

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {
class Foo : public Rec {
    int& alive_;

 public:
    boost::intrusive::set_member_hook<> mnemHook_;

    Foo(const StringView& mnem, const StringView& display, int& alive) noexcept
        : Rec{RecType::ASSET, mnem, display},
          alive_{alive}
    {
        ++alive;
    }

    ~Foo() noexcept override
    {
        --alive_;
    }
};
} // anonymous

BOOST_AUTO_TEST_SUITE(RecSuite)

BOOST_AUTO_TEST_CASE(RecSetCase)
{
    int alive{0};
    {
        RecSet<Foo> s;

        Foo& foo1{*s.emplace("FOO", "Foo One", alive)};
        BOOST_CHECK_EQUAL(alive, 1);
        BOOST_CHECK_EQUAL(foo1.mnem(), "FOO");
        BOOST_CHECK_EQUAL(foo1.display(), "Foo One");
        BOOST_CHECK(s.find("FOO") != s.end());

        // Duplicate.
        Foo& foo2{*s.emplace("FOO", "Foo Two", alive)};
        BOOST_CHECK_EQUAL(alive, 1);
        BOOST_CHECK_EQUAL(&foo2, &foo1);

        // Replace.
        Foo& foo3{*s.emplaceOrReplace("FOO", "Foo Three", alive)};
        BOOST_CHECK_EQUAL(alive, 1);
        BOOST_CHECK_NE(&foo3, &foo1);
        BOOST_CHECK_EQUAL(foo3.mnem(), "FOO");
        BOOST_CHECK_EQUAL(foo3.display(), "Foo Three");
    }
    BOOST_CHECK_EQUAL(alive, 0);
}

BOOST_AUTO_TEST_SUITE_END()
