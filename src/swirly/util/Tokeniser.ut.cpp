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
#include "Tokeniser.hpp"

#include <boost/test/unit_test.hpp>

using namespace swirly;

BOOST_AUTO_TEST_SUITE(TokeniserSuite)

BOOST_AUTO_TEST_CASE(TokeniserEmptyCase)
{
    Tokeniser toks;

    BOOST_TEST(toks.empty());
    BOOST_TEST(!toks.has_delim());

    toks = Tokeniser{""sv, ","sv};
    BOOST_TEST(toks.empty());
    BOOST_TEST(!toks.has_delim());
}

BOOST_AUTO_TEST_CASE(TokeniserMultiEmptyCase)
{
    Tokeniser toks{","sv, ","sv};

    BOOST_TEST(!toks.empty());
    BOOST_TEST(toks.has_delim());
    BOOST_TEST(toks.top().empty());

    toks.pop();
    BOOST_TEST(toks.empty());
    BOOST_TEST(!toks.has_delim());
    BOOST_TEST(toks.top().empty());
}

BOOST_AUTO_TEST_CASE(TokeniserSingleValueCase)
{
    Tokeniser toks{"foo"sv, ","sv};

    BOOST_TEST(!toks.empty());
    BOOST_TEST(!toks.has_delim());
    BOOST_TEST(toks.top() == "foo");

    toks.pop();
    BOOST_TEST(toks.empty());
    BOOST_TEST(!toks.has_delim());
    BOOST_TEST(toks.top().empty());
}

BOOST_AUTO_TEST_CASE(TokeniserMultiValueCase)
{
    Tokeniser toks{"foo,bar,baz"sv, ","sv};

    BOOST_TEST(!toks.empty());
    BOOST_TEST(toks.has_delim());
    BOOST_TEST(toks.top() == "foo");

    toks.pop();
    BOOST_TEST(!toks.empty());
    BOOST_TEST(toks.has_delim());
    BOOST_TEST(toks.top() == "bar");

    toks.pop();
    BOOST_TEST(!toks.empty());
    BOOST_TEST(!toks.has_delim());
    BOOST_TEST(toks.top() == "baz");

    toks.pop();
    BOOST_TEST(toks.empty());
    BOOST_TEST(!toks.has_delim());
    BOOST_TEST(toks.top().empty());
}

BOOST_AUTO_TEST_CASE(TokeniserMultiSepCase)
{
    Tokeniser toks{"foo,bar;baz"sv, ",;"sv};

    BOOST_TEST(!toks.empty());
    BOOST_TEST(toks.has_delim());
    BOOST_TEST(toks.top() == "foo");

    toks.pop();
    BOOST_TEST(!toks.empty());
    BOOST_TEST(toks.has_delim());
    BOOST_TEST(toks.top() == "bar");

    toks.pop();
    BOOST_TEST(!toks.empty());
    BOOST_TEST(!toks.has_delim());
    BOOST_TEST(toks.top() == "baz");

    toks.pop();
    BOOST_TEST(toks.empty());
    BOOST_TEST(!toks.has_delim());
    BOOST_TEST(toks.top().empty());
}

BOOST_AUTO_TEST_CASE(TokeniserLeadingCase)
{
    Tokeniser toks{",foo"sv, ","sv};

    BOOST_TEST(!toks.empty());
    BOOST_TEST(toks.has_delim());
    BOOST_TEST(toks.top().empty());

    toks.pop();
    BOOST_TEST(!toks.empty());
    BOOST_TEST(!toks.has_delim());
    BOOST_TEST(toks.top() == "foo");

    toks.pop();
    BOOST_TEST(toks.empty());
    BOOST_TEST(!toks.has_delim());
    BOOST_TEST(toks.top().empty());
}

BOOST_AUTO_TEST_CASE(TokeniserTrailingCase)
{
    Tokeniser toks{"foo,"sv, ","sv};

    BOOST_TEST(!toks.empty());
    BOOST_TEST(toks.has_delim());
    BOOST_TEST(toks.top() == "foo");

    toks.pop();
    BOOST_TEST(toks.empty());
    BOOST_TEST(!toks.has_delim());
    BOOST_TEST(toks.top().empty());
}

BOOST_AUTO_TEST_CASE(TokeniserLineCase)
{
    Tokeniser lines{"foo\nbar\nbaz..."sv, "\n"sv};

    BOOST_TEST(!lines.empty());
    BOOST_TEST(lines.has_delim());
    BOOST_TEST(lines.top() == "foo");

    lines.pop();
    BOOST_TEST(!lines.empty());
    BOOST_TEST(lines.has_delim());
    BOOST_TEST(lines.top() == "bar");

    lines.pop();
    BOOST_TEST(!lines.empty());
    BOOST_TEST(!lines.has_delim());
    BOOST_TEST(lines.top() == "baz...");
    BOOST_TEST(lines.consumed() == 8);

    lines.pop();
    BOOST_TEST(lines.empty());
    BOOST_TEST(!lines.has_delim());
    BOOST_TEST(lines.top().empty());
}

BOOST_AUTO_TEST_SUITE_END()
