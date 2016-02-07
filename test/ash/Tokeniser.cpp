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
#include <swirly/ash/Tokeniser.hpp>

#include <boost/test/unit_test.hpp>

using namespace swirly;

BOOST_AUTO_TEST_SUITE(TokeniserSuite)

BOOST_AUTO_TEST_CASE(EmptyCase)
{
    Tokeniser<','> toks;

    BOOST_CHECK(toks.empty());

    toks = Tokeniser<','>{""_sv};
    BOOST_CHECK(toks.empty());
}

BOOST_AUTO_TEST_CASE(DelimCase)
{
    Tokeniser<','> toks{","_sv};

    BOOST_CHECK(toks.top().empty());

    toks.pop();
    BOOST_CHECK(toks.empty());
}

BOOST_AUTO_TEST_CASE(SingleCase)
{
    Tokeniser<','> toks{"foo"_sv};

    BOOST_CHECK_EQUAL(toks.top(), "foo");

    toks.pop();
    BOOST_CHECK(toks.empty());
}

BOOST_AUTO_TEST_CASE(ManyCase)
{
    Tokeniser<','> toks{"foo,bar,baz"_sv};

    BOOST_CHECK_EQUAL(toks.top(), "foo");

    toks.pop();
    BOOST_CHECK_EQUAL(toks.top(), "bar");

    toks.pop();
    BOOST_CHECK_EQUAL(toks.top(), "baz");

    toks.pop();
    BOOST_CHECK(toks.empty());
}

BOOST_AUTO_TEST_CASE(LeadingCase)
{
    Tokeniser<','> toks{",foo"_sv};

    BOOST_CHECK(toks.top().empty());

    toks.pop();
    BOOST_CHECK_EQUAL(toks.top(), "foo");

    toks.pop();
    BOOST_CHECK(toks.empty());
}

BOOST_AUTO_TEST_CASE(TrailingCase)
{
    Tokeniser<','> toks{"foo,"_sv};

    BOOST_CHECK_EQUAL(toks.top(), "foo");

    toks.pop();
    BOOST_CHECK(toks.empty());
}

BOOST_AUTO_TEST_SUITE_END()
