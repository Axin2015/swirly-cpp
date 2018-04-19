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
#include "Enum.hpp"

#include <boost/test/unit_test.hpp>

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;
using namespace swirly;

namespace swirly {
inline namespace util {

enum class Test : int { Foo = 1, Bar = 2, Baz = 4, Qux = 8 };

constexpr Test operator""_test(unsigned long long val) noexcept
{
    return box<Test>(val);
}

const char* enumString(Test t)
{
    switch (t) {
    case Test::Foo:
        return "FOO";
    case Test::Bar:
        return "BAR";
    case Test::Baz:
        return "BAZ";
    case Test::Qux:
        return "QUX";
    }
    terminate();
}

inline ostream& operator<<(ostream& os, Test t)
{
    return os << enumString(t);
}

} // namespace util
} // namespace swirly

BOOST_AUTO_TEST_SUITE(EnumSuite)

BOOST_AUTO_TEST_CASE(EnumStringCase)
{
    BOOST_TEST(lexical_cast<string>(Test::Foo) == "FOO");
    BOOST_TEST(lexical_cast<string>(Test::Bar) == "BAR");
    BOOST_TEST(lexical_cast<string>(Test::Baz) == "BAZ");
    BOOST_TEST(lexical_cast<string>(Test::Qux) == "QUX");
}

BOOST_AUTO_TEST_SUITE_END()
