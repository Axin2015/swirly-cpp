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
#include "SessId.hpp"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(SessIdSuite)

BOOST_AUTO_TEST_CASE(SessIdCompareCase)
{
    {
        const FixSessId s{Version{1, 2}, "foo", "bar"};
        const FixSessIdView sv{Version{1, 2}, "foo"sv, "bar"sv};
        BOOST_TEST(s == sv);
        BOOST_TEST(sv == s);
        BOOST_TEST(s <= sv);
        BOOST_TEST(sv <= s);
        BOOST_TEST(s >= sv);
        BOOST_TEST(sv >= s);
    }
    {
        const FixSessId s{Version{1, 2}, "foo", "bar"};
        const FixSessIdView sv{Version{1, 2}, "foo"sv, "baz"sv};
        BOOST_TEST(s < sv);
        BOOST_TEST(sv > s);
        BOOST_TEST(s <= sv);
        BOOST_TEST(sv >= s);
    }
    {
        const FixSessId s{Version{1, 2}, "foz", "bar"};
        const FixSessIdView sv{Version{1, 2}, "foo"sv, "bar"sv};
        BOOST_TEST(s > sv);
        BOOST_TEST(sv < s);
        BOOST_TEST(s >= sv);
        BOOST_TEST(sv <= s);
    }
}

BOOST_AUTO_TEST_CASE(SessIdHashCase)
{
    const FixSessId s{Version{1, 2}, "foo", "bar"};
    const FixSessIdView sv{Version{1, 2}, "foo"sv, "bar"sv};
    BOOST_TEST(hash_value(s) == hash_value(sv));
}

BOOST_AUTO_TEST_CASE(SessIdFlipCase)
{
    const FixSessId s{Version{1, 2}, "foo", "bar"};
    const FixSessIdView sv{Version{1, 2}, "bar"sv, "foo"sv};
    BOOST_TEST(s == ~sv);
}

BOOST_AUTO_TEST_CASE(SessIdTransparentCase)
{
    set<FixSessId, FixSessIdCompare> s;
    s.emplace(Version{1, 2}, "foo", "bar");
    // Transparent comparison.
    const FixSessIdView sv{Version{1, 2}, "foo"sv, "bar"sv};
    BOOST_TEST(s.count(sv) == 1);
}

BOOST_AUTO_TEST_SUITE_END()
