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
#include "Struct.hpp"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {
namespace tag {
struct Foo {
};
struct Bar {
};
} // namespace tag

static_assert(empty(Struct));
static_assert(!empty(Struct.extend<tag::Foo>(101)));
static_assert(size(Struct.extend<tag::Foo>(101)) == 1);
static_assert(has<tag::Foo>(Struct.extend<tag::Foo>(101)));
static_assert(!has<tag::Bar>(Struct.extend<tag::Foo>(101)));
static_assert(get<tag::Foo>(Struct.extend<tag::Foo>(101)) == 101);

/*
        get<TestTag2>(s) = Test{303};
        cout << get<TestTag2>(s).x << endl;
        cout << has<TestTag1>(s) << endl;
        cout << has<int>(s) << endl;
        cout << size(s) << endl;
 */

} // namespace

BOOST_AUTO_TEST_SUITE(StructSuite)

BOOST_AUTO_TEST_CASE(StructEmptyCase)
{
    const auto s = Struct;
    BOOST_TEST(empty(s));
    BOOST_TEST(size(s) == 0U);
    BOOST_TEST(!has<tag::Foo>(s));
    BOOST_TEST(!has<tag::Bar>(s));
}

BOOST_AUTO_TEST_CASE(StructFooCase)
{
    struct Test {
        int x;
    };

    const auto s = Struct.extend<tag::Foo>(Test{101});
    BOOST_TEST(!empty(s));
    BOOST_TEST(size(s) == 1U);
    BOOST_TEST(has<tag::Foo>(s));
    BOOST_TEST(!has<tag::Bar>(s));
    BOOST_TEST(get<tag::Foo>(s).x == 101);
}

BOOST_AUTO_TEST_CASE(StructFooBarCase)
{
    struct Test {
        int x;
    };

    const auto s = Struct.extend<tag::Foo>(Test{101}).extend<tag::Bar>(Test{202});
    BOOST_TEST(!empty(s));
    BOOST_TEST(size(s) == 2U);
    BOOST_TEST(has<tag::Foo>(s));
    BOOST_TEST(has<tag::Bar>(s));
    BOOST_TEST(get<tag::Foo>(s).x == 101);
    BOOST_TEST(get<tag::Bar>(s).x == 202);
}

BOOST_AUTO_TEST_CASE(StructLambdaCase)
{
    const auto s = Struct.extend<tag::Foo>([](int x) { return x + x; });
    BOOST_TEST(get<tag::Foo>(s)(101) == 202);
}

BOOST_AUTO_TEST_CASE(StructMutableCase)
{
    auto s = Struct.extend<tag::Foo>(101);
    BOOST_TEST(get<tag::Foo>(s) == 101);
    get<tag::Foo>(s) = 202;
    BOOST_TEST(get<tag::Foo>(s) == 202);
}

BOOST_AUTO_TEST_SUITE_END()
