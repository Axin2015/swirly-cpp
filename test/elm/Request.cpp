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
#include <swirly/elm/Request.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {
class Foo : public Request {
    int& alive_;

 public:
    boost::intrusive::set_member_hook<> idHook_;

    Foo(const StringView& market, Iden id, int& alive) noexcept
        : Request{"", market, "", 0_jd, id, "", Side::BUY, 0_lts, 0_ms},
          alive_{alive}
    {
        ++alive;
    }

    ~Foo() noexcept override
    {
        --alive_;
    }
};

using FooPtr = boost::intrusive_ptr<Foo>;

} // anonymous

BOOST_AUTO_TEST_SUITE(RequestSuite)

BOOST_AUTO_TEST_CASE(RequestIdSetCase)
{
    int alive{0};
    {
        RequestIdSet<Foo> s;

        FooPtr foo1{&*s.emplace("FOO", 1_id, alive)};
        BOOST_CHECK_EQUAL(alive, 1);
        BOOST_CHECK_EQUAL(foo1->refs(), 2);
        BOOST_CHECK_EQUAL(foo1->market(), "FOO");
        BOOST_CHECK_EQUAL(foo1->id(), 1_id);
        BOOST_CHECK(s.find("FOO", 1_id) != s.end());

        // Duplicate.
        FooPtr foo2{&*s.emplace("FOO", 1_id, alive)};
        BOOST_CHECK_EQUAL(alive, 1);
        BOOST_CHECK_EQUAL(foo2->refs(), 3);
        BOOST_CHECK_EQUAL(foo2, foo1);

        // Replace.
        FooPtr foo3{&*s.emplaceOrReplace("FOO", 1_id, alive)};
        BOOST_CHECK_EQUAL(alive, 2);
        BOOST_CHECK_EQUAL(foo3->refs(), 2);
        BOOST_CHECK_NE(foo3, foo1);
        BOOST_CHECK_EQUAL(foo3->market(), "FOO");
        BOOST_CHECK_EQUAL(foo3->id(), 1_id);
    }
    BOOST_CHECK_EQUAL(alive, 0);
}

BOOST_AUTO_TEST_SUITE_END()
