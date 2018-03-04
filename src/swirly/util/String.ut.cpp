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
#include "String.hpp"

#include <swirly/unit/Test.hpp>

using namespace std;
using namespace swirly;

SWIRLY_TEST_CASE(ToString)
{
    SWIRLY_CHECK(toString("foo"sv) == "foo"s);
    SWIRLY_CHECK(stoi(toString(12345)) == 12345);
    SWIRLY_CHECK(stod(toString(12345.67)) == 12345.67);
}

SWIRLY_TEST_CASE(Stoi32NoSign)
{
    SWIRLY_CHECK(stoi32(""sv) == 0);
    SWIRLY_CHECK(stoi32("1"sv) == 1);
    SWIRLY_CHECK(stoi32("123"sv) == 123);
    SWIRLY_CHECK(stoi32(" "sv) == 0);
    SWIRLY_CHECK(stoi32(" 1 "sv) == 1);
    SWIRLY_CHECK(stoi32(" 123 "sv) == 123);
    SWIRLY_CHECK(stoi32("x"sv) == 0);
    SWIRLY_CHECK(stoi32(" 1x"sv) == 1);
    SWIRLY_CHECK(stoi32(" 123x"sv) == 123);
    SWIRLY_CHECK(stoi32("x1 "sv) == 0);
    SWIRLY_CHECK(stoi32("x123 "sv) == 0);
}

SWIRLY_TEST_CASE(Stoi32PosSign)
{
    SWIRLY_CHECK(stoi32("+"sv) == 0);
    SWIRLY_CHECK(stoi32("+1"sv) == 1);
    SWIRLY_CHECK(stoi32("+123"sv) == 123);
    SWIRLY_CHECK(stoi32(" + "sv) == 0);
    SWIRLY_CHECK(stoi32(" +1 "sv) == 1);
    SWIRLY_CHECK(stoi32(" +123 "sv) == 123);
    SWIRLY_CHECK(stoi32("+x"sv) == 0);
    SWIRLY_CHECK(stoi32(" +1x"sv) == 1);
    SWIRLY_CHECK(stoi32(" +123x"sv) == 123);
    SWIRLY_CHECK(stoi32("x+1 "sv) == 0);
    SWIRLY_CHECK(stoi32("x+123 "sv) == 0);
}

SWIRLY_TEST_CASE(Stoi32NegSign)
{
    SWIRLY_CHECK(stoi32("-"sv) == 0);
    SWIRLY_CHECK(stoi32("-1"sv) == -1);
    SWIRLY_CHECK(stoi32("-123"sv) == -123);
    SWIRLY_CHECK(stoi32(" - "sv) == 0);
    SWIRLY_CHECK(stoi32(" -1 "sv) == -1);
    SWIRLY_CHECK(stoi32(" -123 "sv) == -123);
    SWIRLY_CHECK(stoi32("-x"sv) == 0);
    SWIRLY_CHECK(stoi32(" -1x"sv) == -1);
    SWIRLY_CHECK(stoi32(" -123x"sv) == -123);
    SWIRLY_CHECK(stoi32("x-1 "sv) == 0);
    SWIRLY_CHECK(stoi32("x-123 "sv) == 0);
}

SWIRLY_TEST_CASE(Stou64)
{
    SWIRLY_CHECK(stou64(""sv) == 0UL);
    SWIRLY_CHECK(stou64("1"sv) == 1UL);
    SWIRLY_CHECK(stou64("123"sv) == 123UL);
    SWIRLY_CHECK(stou64(" "sv) == 0UL);
    SWIRLY_CHECK(stou64(" 1 "sv) == 1UL);
    SWIRLY_CHECK(stou64(" 123 "sv) == 123UL);
    SWIRLY_CHECK(stou64("x"sv) == 0UL);
    SWIRLY_CHECK(stou64(" 1x"sv) == 1UL);
    SWIRLY_CHECK(stou64(" 123x"sv) == 123UL);
    SWIRLY_CHECK(stou64("x1 "sv) == 0UL);
    SWIRLY_CHECK(stou64("x123 "sv) == 0UL);
    SWIRLY_CHECK(stou64("18446744073709551615"sv) == 18446744073709551615ULL);
}

SWIRLY_TEST_CASE(Stob)
{
    SWIRLY_CHECK(stob(""sv, false) == false);
    SWIRLY_CHECK(stob(""sv, true) == true);

    SWIRLY_CHECK(stob("0"sv, true) == false);
    SWIRLY_CHECK(stob("F"sv, true) == false);
    SWIRLY_CHECK(stob("N"sv, true) == false);
    SWIRLY_CHECK(stob("f"sv, true) == false);
    SWIRLY_CHECK(stob("n"sv, true) == false);

    SWIRLY_CHECK(stob("1"sv, false) == true);
    SWIRLY_CHECK(stob("T"sv, false) == true);
    SWIRLY_CHECK(stob("Y"sv, false) == true);
    SWIRLY_CHECK(stob("t"sv, false) == true);
    SWIRLY_CHECK(stob("y"sv, false) == true);

    SWIRLY_CHECK(stob("NO"sv, true) == false);
    SWIRLY_CHECK(stob("No"sv, true) == false);
    SWIRLY_CHECK(stob("no"sv, true) == false);

    SWIRLY_CHECK(stob("ON"sv, false) == true);
    SWIRLY_CHECK(stob("On"sv, false) == true);
    SWIRLY_CHECK(stob("on"sv, false) == true);

    SWIRLY_CHECK(stob("OFF"sv, true) == false);
    SWIRLY_CHECK(stob("Off"sv, true) == false);
    SWIRLY_CHECK(stob("off"sv, true) == false);

    SWIRLY_CHECK(stob("YES"sv, false) == true);
    SWIRLY_CHECK(stob("Yes"sv, false) == true);
    SWIRLY_CHECK(stob("yes"sv, false) == true);

    SWIRLY_CHECK(stob("TRUE"sv, false) == true);
    SWIRLY_CHECK(stob("True"sv, false) == true);
    SWIRLY_CHECK(stob("true"sv, false) == true);

    SWIRLY_CHECK(stob("FALSE"sv, true) == false);
    SWIRLY_CHECK(stob("False"sv, true) == false);
    SWIRLY_CHECK(stob("false"sv, true) == false);
}

SWIRLY_TEST_CASE(LtrimCopy)
{
    SWIRLY_CHECK(ltrimCopy(""sv) == ""sv);
    SWIRLY_CHECK(ltrimCopy(" \t\n\v\f\r"sv) == ""sv);
    SWIRLY_CHECK(ltrimCopy(" \t\n\v\f\rfoo "sv) == "foo "sv);
    SWIRLY_CHECK(ltrimCopy("foo"sv) == "foo"sv);

    SWIRLY_CHECK(ltrimCopy(""s) == ""s);
    SWIRLY_CHECK(ltrimCopy(" \t\n\v\f\r"s) == ""s);
    SWIRLY_CHECK(ltrimCopy(" \t\n\v\f\rfoo "s) == "foo "s);
    SWIRLY_CHECK(ltrimCopy("foo"s) == "foo"s);
}

SWIRLY_TEST_CASE(RtrimCopy)
{
    SWIRLY_CHECK(rtrimCopy(""sv) == ""sv);
    SWIRLY_CHECK(rtrimCopy(" \t\n\v\f\r"sv) == ""sv);
    SWIRLY_CHECK(rtrimCopy(" foo \t\n\v\f\r"sv) == " foo"sv);
    SWIRLY_CHECK(rtrimCopy("foo"sv) == "foo"sv);

    SWIRLY_CHECK(rtrimCopy(""s) == ""s);
    SWIRLY_CHECK(rtrimCopy(" \t\n\v\f\r"s) == ""s);
    SWIRLY_CHECK(rtrimCopy(" foo \t\n\v\f\r"s) == " foo"s);
    SWIRLY_CHECK(rtrimCopy("foo"s) == "foo"s);
}

SWIRLY_TEST_CASE(TrimCopy)
{
    SWIRLY_CHECK(trimCopy(""sv) == ""sv);
    SWIRLY_CHECK(trimCopy(" \t\n\v\f\r"sv) == ""sv);
    SWIRLY_CHECK(trimCopy(" \t\n\v\f\rfoo \t\n\v\f\r"sv) == "foo"sv);
    SWIRLY_CHECK(trimCopy("foo"sv) == "foo"sv);

    SWIRLY_CHECK(trimCopy(""s) == ""s);
    SWIRLY_CHECK(trimCopy(" \t\n\v\f\r"s) == ""s);
    SWIRLY_CHECK(trimCopy(" \t\n\v\f\rfoo \t\n\v\f\r"s) == "foo"s);
    SWIRLY_CHECK(trimCopy("foo"s) == "foo"s);
}

SWIRLY_TEST_CASE(SplitPair)
{
    SWIRLY_CHECK(splitPair(""sv, '=') == make_pair(""sv, ""sv));
    SWIRLY_CHECK(splitPair("="sv, '=') == make_pair(""sv, ""sv));
    SWIRLY_CHECK(splitPair("a"sv, '=') == make_pair("a"sv, ""sv));
    SWIRLY_CHECK(splitPair("a="sv, '=') == make_pair("a"sv, ""sv));
    SWIRLY_CHECK(splitPair("=b"sv, '=') == make_pair(""sv, "b"sv));
    SWIRLY_CHECK(splitPair("a=b"sv, '=') == make_pair("a"sv, "b"sv));
    SWIRLY_CHECK(splitPair("a:b"sv, ':') == make_pair("a"sv, "b"sv));
    SWIRLY_CHECK(splitPair(" a = b "sv, '=') == make_pair(" a "sv, " b "sv));

    SWIRLY_CHECK(splitPair(""s, '=') == make_pair(""s, ""s));
    SWIRLY_CHECK(splitPair("="s, '=') == make_pair(""s, ""s));
    SWIRLY_CHECK(splitPair("a"s, '=') == make_pair("a"s, ""s));
    SWIRLY_CHECK(splitPair("a="s, '=') == make_pair("a"s, ""s));
    SWIRLY_CHECK(splitPair("=b"s, '=') == make_pair(""s, "b"s));
    SWIRLY_CHECK(splitPair("a=b"s, '=') == make_pair("a"s, "b"s));
    SWIRLY_CHECK(splitPair("a:b"s, ':') == make_pair("a"s, "b"s));
    SWIRLY_CHECK(splitPair(" a = b "s, '=') == make_pair(" a "s, " b "s));
}
