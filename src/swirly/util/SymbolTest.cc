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

#include <swirly/unit/Test.hpp>

#include <swirly/util/String.hpp>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(SymbolEmpty)
{
    Symbol symbol;
    SWIRLY_CHECK(symbol.empty());
    SWIRLY_CHECK(symbol.size() == 0UL);
    SWIRLY_CHECK(symbol.compare(""sv) == 0);
    SWIRLY_CHECK(symbol == Symbol{""sv});
}

SWIRLY_TEST_CASE(SymbolNonEmpty)
{
    Symbol symbol{"Foo"sv};
    SWIRLY_CHECK(!symbol.empty());
    SWIRLY_CHECK(symbol.size() == 3UL);
    SWIRLY_CHECK(symbol.compare("Foo"sv) == 0);
    SWIRLY_CHECK(symbol == Symbol{"Foo"sv});
}

SWIRLY_TEST_CASE(SymbolUpperBound)
{
    Symbol symbol{"0123456789ABCDEFx"sv};
    SWIRLY_CHECK(symbol.size() == MaxSymbol);
    SWIRLY_CHECK(symbol == "0123456789ABCDEF"sv);

    symbol = "0123456789abcdefx"sv;
    SWIRLY_CHECK(symbol.size() == MaxSymbol);
    SWIRLY_CHECK(symbol == "0123456789abcdef"sv);
}

SWIRLY_TEST_CASE(SymbolIndex)
{
    Symbol symbol{"Bar"sv};
    SWIRLY_CHECK(symbol[0] == 'B');
    SWIRLY_CHECK(symbol[1] == 'a');
    SWIRLY_CHECK(symbol[2] == 'r');
    SWIRLY_CHECK(symbol.front() == 'B');
    SWIRLY_CHECK(symbol.back() == 'r');
}

SWIRLY_TEST_CASE(SymbolClear)
{
    Symbol symbol{"Foo"sv};
    symbol.clear();
    SWIRLY_CHECK(symbol.empty());
}
