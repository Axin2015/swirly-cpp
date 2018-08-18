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
#include "Lexer.hpp"

#include <boost/test/unit_test.hpp>

#include <iostream>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(LexerSuite)

BOOST_AUTO_TEST_CASE(FindTagEmptyCase)
{
    {
        const auto [val, found] = find_tag(""sv, 123);
        BOOST_TEST(!found);
        BOOST_TEST(val == ""sv);
    }
    {
        const auto [val, found] = find_tag("foo"sv, 123);
        BOOST_TEST(!found);
        BOOST_TEST(val == ""sv);
    }
}

BOOST_AUTO_TEST_CASE(FindTagFirstCase)
{
    {
        const auto [val, found] = find_tag("1=foo\1"sv, 1);
        BOOST_TEST(found);
        BOOST_TEST(val == "foo"sv);
    }
    {
        const auto [val, found] = find_tag("123=foo\1"sv, 123);
        BOOST_TEST(found);
        BOOST_TEST(val == "foo"sv);
    }
    {
        // No trailing delimiter.
        const auto [val, found] = find_tag("123=foo"sv, 123);
        BOOST_TEST(found);
        BOOST_TEST(val == "foo"sv);
    }
}

BOOST_AUTO_TEST_CASE(FindTagSecondCase)
{
    {
        const auto [val, found] = find_tag("123=foo\1"
                                           "4=bar\1"sv,
                                           4);
        BOOST_TEST(found);
        BOOST_TEST(val == "bar"sv);
    }
    {
        const auto [val, found] = find_tag("123=foo\1"
                                           "456=bar\1"sv,
                                           456);
        BOOST_TEST(found);
        BOOST_TEST(val == "bar"sv);
    }
    {
        // No trailing delimiter.
        const auto [val, found] = find_tag("123=\1"
                                           "456=bar"sv,
                                           456);
        BOOST_TEST(found);
        BOOST_TEST(val == "bar"sv);
    }
    {
        const auto [val, found] = find_tag("123=\1"
                                           "456=bar\1"sv,
                                           456);
        BOOST_TEST(found);
        BOOST_TEST(val == "bar"sv);
    }
}

BOOST_AUTO_TEST_CASE(FindTagThirdCase)
{
    {
        const auto [val, found] = find_tag("123=foo\1"
                                           "456=bar\1"
                                           "7=baz\1"sv,
                                           7);
        BOOST_TEST(found);
        BOOST_TEST(val == "baz"sv);
    }
    {
        const auto [val, found] = find_tag("123=foo\1"
                                           "456=bar\1"
                                           "789=baz\1"sv,
                                           789);
        BOOST_TEST(found);
        BOOST_TEST(val == "baz"sv);
    }
    {
        // No trailing delimiter.
        const auto [val, found] = find_tag("123=foo\1"
                                           "456=bar\1"
                                           "789=baz"sv,
                                           789);
        BOOST_TEST(found);
        BOOST_TEST(val == "baz"sv);
    }
    {
        const auto [val, found] = find_tag("123=\1"
                                           "456=\1"
                                           "789=baz"sv,
                                           789);
        BOOST_TEST(found);
        BOOST_TEST(val == "baz"sv);
    }
}

BOOST_AUTO_TEST_CASE(FindTagZeroCase)
{
    {
        const auto [val, found] = find_tag("0=foo\1"sv, 0);
        BOOST_TEST(found);
        BOOST_TEST(val == "foo"sv);
    }
    {
        const auto [val, found] = find_tag("1=foo\1"
                                           "0=bar\1"sv,
                                           0);
        BOOST_TEST(found);
        BOOST_TEST(val == "bar"sv);
    }
}

BOOST_AUTO_TEST_CASE(LexerEmptyCase)
{
    FixLexer lex{""sv};
    BOOST_TEST(lex.empty());
    BOOST_TEST(lex.offset() == 0U);
}

BOOST_AUTO_TEST_CASE(LexerTopCase)
{
    FixLexer lex{"1=foo\1"
                 "23=bar\1"sv};
    {
        BOOST_TEST(!lex.empty());
        BOOST_TEST(lex.offset() == 0U);

        const auto [tag, val] = lex.top();
        BOOST_TEST(tag == 1);
        BOOST_TEST(val == "foo"sv);
    }
    {
        BOOST_TEST(!lex.empty());
        BOOST_TEST(lex.offset() == 0U);

        const auto [tag, val] = lex.top();
        BOOST_TEST(tag == 1);
        BOOST_TEST(val == "foo"sv);
    }
    lex.pop();
    {
        BOOST_TEST(!lex.empty());
        BOOST_TEST(lex.offset() == 6U);

        const auto [tag, val] = lex.top();
        BOOST_TEST(tag == 23);
        BOOST_TEST(val == "bar"sv);
    }
    {
        BOOST_TEST(!lex.empty());
        BOOST_TEST(lex.offset() == 6U);

        const auto [tag, val] = lex.top();
        BOOST_TEST(tag == 23);
        BOOST_TEST(val == "bar"sv);
    }
    lex.pop();
    BOOST_TEST(lex.empty());
    BOOST_TEST(lex.offset() == 13U);
}

BOOST_AUTO_TEST_CASE(LexerNextCase)
{
    FixLexer lex{"1=foo\1"
                 "23=bar\1"sv};
    {
        BOOST_TEST(!lex.empty());
        BOOST_TEST(lex.offset() == 0U);

        const auto [tag, val] = lex.next();
        BOOST_TEST(tag == 1);
        BOOST_TEST(val == "foo"sv);
    }
    {
        BOOST_TEST(!lex.empty());
        BOOST_TEST(lex.offset() == 6U);

        const auto [tag, val] = lex.next();
        BOOST_TEST(tag == 23);
        BOOST_TEST(val == "bar"sv);
    }
    BOOST_TEST(lex.empty());
    BOOST_TEST(lex.offset() == 13U);
}

BOOST_AUTO_TEST_CASE(LexerPopCase)
{
    {
        FixLexer lex{"1=foo\1"
                     "23=bar\1"sv};

        BOOST_TEST(!lex.empty());
        BOOST_TEST(lex.offset() == 0U);

        const auto [tag, val] = lex.top();
        BOOST_TEST(tag == 1);
        BOOST_TEST(val == "foo"sv);

        lex.pop();
        BOOST_TEST(!lex.empty());
        BOOST_TEST(lex.offset() == 6U);

        lex.pop();
        BOOST_TEST(lex.empty());
        BOOST_TEST(lex.offset() == 13U);
    }
    {
        FixLexer lex{"1=foo\1"
                     "23=bar\1"sv};

        BOOST_TEST(!lex.empty());
        BOOST_TEST(lex.offset() == 0U);

        lex.pop();
        BOOST_TEST(!lex.empty());
        BOOST_TEST(lex.offset() == 6U);

        const auto [tag, val] = lex.top();
        BOOST_TEST(tag == 23);
        BOOST_TEST(val == "bar"sv);

        lex.pop();
        BOOST_TEST(lex.empty());
        BOOST_TEST(lex.offset() == 13U);
    }
}

BOOST_AUTO_TEST_SUITE_END()
