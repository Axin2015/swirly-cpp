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
#include "Symbol.hpp"

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

#include <swirly/util/String.hpp>

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(SymbolSuite)

BOOST_AUTO_TEST_CASE(SymbolEmptyCase)
{
    Symbol symbol;
    BOOST_TEST(symbol.empty());
    BOOST_TEST(symbol.size() == 0U);
    BOOST_TEST(symbol.compare(""sv) == 0);
    BOOST_TEST(symbol == Symbol{""sv});
}

BOOST_AUTO_TEST_CASE(SymbolNonEmptyCase)
{
    Symbol symbol{"Foo"sv};
    BOOST_TEST(!symbol.empty());
    BOOST_TEST(symbol.size() == 3U);
    BOOST_TEST(symbol.compare("Foo"sv) == 0);
    BOOST_TEST(symbol == Symbol{"Foo"sv});
}

BOOST_AUTO_TEST_CASE(SymbolUpperBoundCase)
{
    Symbol symbol{"0123456789ABCDEFx"sv};
    BOOST_TEST(symbol.size() == MaxSymbol);
    BOOST_TEST(symbol == "0123456789ABCDEF"sv);

    symbol = "0123456789abcdefx"sv;
    BOOST_TEST(symbol.size() == MaxSymbol);
    BOOST_TEST(symbol == "0123456789abcdef"sv);
}

BOOST_AUTO_TEST_CASE(SymbolIndexCase)
{
    Symbol symbol{"Bar"sv};
    BOOST_TEST(symbol[0] == 'B');
    BOOST_TEST(symbol[1] == 'a');
    BOOST_TEST(symbol[2] == 'r');
    BOOST_TEST(symbol.front() == 'B');
    BOOST_TEST(symbol.back() == 'r');
}

BOOST_AUTO_TEST_CASE(SymbolClearCase)
{
    Symbol symbol{"Foo"sv};
    symbol.clear();
    BOOST_TEST(symbol.empty());
}

BOOST_AUTO_TEST_SUITE_END()
