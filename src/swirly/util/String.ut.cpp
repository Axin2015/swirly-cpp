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

#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

namespace std {
template <typename T, typename U>
ostream& operator<<(ostream& os, const pair<T, U>& p)
{
    return os << '(' << p.first << ',' << p.second << ')';
}
} // namespace std

using namespace std;
using namespace swirly;

BOOST_AUTO_TEST_SUITE(StringSuite)

BOOST_AUTO_TEST_CASE(ToStringCase)
{
    BOOST_TEST(toString("foo"sv) == "foo"s);
    BOOST_TEST(stoi(toString(12345)) == 12345);
    BOOST_TEST(stod(toString(12345.67)) == 12345.67);
}

BOOST_AUTO_TEST_CASE(Stoi32NoSignCase)
{
    BOOST_TEST(stoi32(""sv) == 0);
    BOOST_TEST(stoi32("1"sv) == 1);
    BOOST_TEST(stoi32("123"sv) == 123);
    BOOST_TEST(stoi32(" "sv) == 0);
    BOOST_TEST(stoi32(" 1 "sv) == 1);
    BOOST_TEST(stoi32(" 123 "sv) == 123);
    BOOST_TEST(stoi32("x"sv) == 0);
    BOOST_TEST(stoi32(" 1x"sv) == 1);
    BOOST_TEST(stoi32(" 123x"sv) == 123);
    BOOST_TEST(stoi32("x1 "sv) == 0);
    BOOST_TEST(stoi32("x123 "sv) == 0);
}

BOOST_AUTO_TEST_CASE(Stoi32PosSignCase)
{
    BOOST_TEST(stoi32("+"sv) == 0);
    BOOST_TEST(stoi32("+1"sv) == 1);
    BOOST_TEST(stoi32("+123"sv) == 123);
    BOOST_TEST(stoi32(" + "sv) == 0);
    BOOST_TEST(stoi32(" +1 "sv) == 1);
    BOOST_TEST(stoi32(" +123 "sv) == 123);
    BOOST_TEST(stoi32("+x"sv) == 0);
    BOOST_TEST(stoi32(" +1x"sv) == 1);
    BOOST_TEST(stoi32(" +123x"sv) == 123);
    BOOST_TEST(stoi32("x+1 "sv) == 0);
    BOOST_TEST(stoi32("x+123 "sv) == 0);
}

BOOST_AUTO_TEST_CASE(Stoi32NegSignCase)
{
    BOOST_TEST(stoi32("-"sv) == 0);
    BOOST_TEST(stoi32("-1"sv) == -1);
    BOOST_TEST(stoi32("-123"sv) == -123);
    BOOST_TEST(stoi32(" - "sv) == 0);
    BOOST_TEST(stoi32(" -1 "sv) == -1);
    BOOST_TEST(stoi32(" -123 "sv) == -123);
    BOOST_TEST(stoi32("-x"sv) == 0);
    BOOST_TEST(stoi32(" -1x"sv) == -1);
    BOOST_TEST(stoi32(" -123x"sv) == -123);
    BOOST_TEST(stoi32("x-1 "sv) == 0);
    BOOST_TEST(stoi32("x-123 "sv) == 0);
}

BOOST_AUTO_TEST_CASE(Stou64Case)
{
    BOOST_TEST(stou64(""sv) == 0UL);
    BOOST_TEST(stou64("1"sv) == 1UL);
    BOOST_TEST(stou64("123"sv) == 123UL);
    BOOST_TEST(stou64(" "sv) == 0UL);
    BOOST_TEST(stou64(" 1 "sv) == 1UL);
    BOOST_TEST(stou64(" 123 "sv) == 123UL);
    BOOST_TEST(stou64("x"sv) == 0UL);
    BOOST_TEST(stou64(" 1x"sv) == 1UL);
    BOOST_TEST(stou64(" 123x"sv) == 123UL);
    BOOST_TEST(stou64("x1 "sv) == 0UL);
    BOOST_TEST(stou64("x123 "sv) == 0UL);
    BOOST_TEST(stou64("18446744073709551615"sv) == 18446744073709551615ULL);
}

BOOST_AUTO_TEST_CASE(StobCase)
{
    BOOST_TEST(stob(""sv, false) == false);
    BOOST_TEST(stob(""sv, true) == true);

    BOOST_TEST(stob("0"sv, true) == false);
    BOOST_TEST(stob("F"sv, true) == false);
    BOOST_TEST(stob("N"sv, true) == false);
    BOOST_TEST(stob("f"sv, true) == false);
    BOOST_TEST(stob("n"sv, true) == false);

    BOOST_TEST(stob("1"sv, false) == true);
    BOOST_TEST(stob("T"sv, false) == true);
    BOOST_TEST(stob("Y"sv, false) == true);
    BOOST_TEST(stob("t"sv, false) == true);
    BOOST_TEST(stob("y"sv, false) == true);

    BOOST_TEST(stob("NO"sv, true) == false);
    BOOST_TEST(stob("No"sv, true) == false);
    BOOST_TEST(stob("no"sv, true) == false);

    BOOST_TEST(stob("ON"sv, false) == true);
    BOOST_TEST(stob("On"sv, false) == true);
    BOOST_TEST(stob("on"sv, false) == true);

    BOOST_TEST(stob("OFF"sv, true) == false);
    BOOST_TEST(stob("Off"sv, true) == false);
    BOOST_TEST(stob("off"sv, true) == false);

    BOOST_TEST(stob("YES"sv, false) == true);
    BOOST_TEST(stob("Yes"sv, false) == true);
    BOOST_TEST(stob("yes"sv, false) == true);

    BOOST_TEST(stob("TRUE"sv, false) == true);
    BOOST_TEST(stob("True"sv, false) == true);
    BOOST_TEST(stob("true"sv, false) == true);

    BOOST_TEST(stob("FALSE"sv, true) == false);
    BOOST_TEST(stob("False"sv, true) == false);
    BOOST_TEST(stob("false"sv, true) == false);
}

BOOST_AUTO_TEST_CASE(LtrimCopyCase)
{
    BOOST_TEST(ltrimCopy(""sv) == ""sv);
    BOOST_TEST(ltrimCopy(" \t\n\v\f\r"sv) == ""sv);
    BOOST_TEST(ltrimCopy(" \t\n\v\f\rfoo "sv) == "foo "sv);
    BOOST_TEST(ltrimCopy("foo"sv) == "foo"sv);

    BOOST_TEST(ltrimCopy(""s) == ""s);
    BOOST_TEST(ltrimCopy(" \t\n\v\f\r"s) == ""s);
    BOOST_TEST(ltrimCopy(" \t\n\v\f\rfoo "s) == "foo "s);
    BOOST_TEST(ltrimCopy("foo"s) == "foo"s);
}

BOOST_AUTO_TEST_CASE(RtrimCopyCase)
{
    BOOST_TEST(rtrimCopy(""sv) == ""sv);
    BOOST_TEST(rtrimCopy(" \t\n\v\f\r"sv) == ""sv);
    BOOST_TEST(rtrimCopy(" foo \t\n\v\f\r"sv) == " foo"sv);
    BOOST_TEST(rtrimCopy("foo"sv) == "foo"sv);

    BOOST_TEST(rtrimCopy(""s) == ""s);
    BOOST_TEST(rtrimCopy(" \t\n\v\f\r"s) == ""s);
    BOOST_TEST(rtrimCopy(" foo \t\n\v\f\r"s) == " foo"s);
    BOOST_TEST(rtrimCopy("foo"s) == "foo"s);
}

BOOST_AUTO_TEST_CASE(TrimCopyCase)
{
    BOOST_TEST(trimCopy(""sv) == ""sv);
    BOOST_TEST(trimCopy(" \t\n\v\f\r"sv) == ""sv);
    BOOST_TEST(trimCopy(" \t\n\v\f\rfoo \t\n\v\f\r"sv) == "foo"sv);
    BOOST_TEST(trimCopy("foo"sv) == "foo"sv);

    BOOST_TEST(trimCopy(""s) == ""s);
    BOOST_TEST(trimCopy(" \t\n\v\f\r"s) == ""s);
    BOOST_TEST(trimCopy(" \t\n\v\f\rfoo \t\n\v\f\r"s) == "foo"s);
    BOOST_TEST(trimCopy("foo"s) == "foo"s);
}

BOOST_AUTO_TEST_CASE(SplitPairCase)
{
    BOOST_TEST(splitPair(""sv, '=') == make_pair(""sv, ""sv));
    BOOST_TEST(splitPair("="sv, '=') == make_pair(""sv, ""sv));
    BOOST_TEST(splitPair("a"sv, '=') == make_pair("a"sv, ""sv));
    BOOST_TEST(splitPair("a="sv, '=') == make_pair("a"sv, ""sv));
    BOOST_TEST(splitPair("=b"sv, '=') == make_pair(""sv, "b"sv));
    BOOST_TEST(splitPair("a=b"sv, '=') == make_pair("a"sv, "b"sv));
    BOOST_TEST(splitPair("a:b"sv, ':') == make_pair("a"sv, "b"sv));
    BOOST_TEST(splitPair(" a = b "sv, '=') == make_pair(" a "sv, " b "sv));

    BOOST_TEST(splitPair(""s, '=') == make_pair(""s, ""s));
    BOOST_TEST(splitPair("="s, '=') == make_pair(""s, ""s));
    BOOST_TEST(splitPair("a"s, '=') == make_pair("a"s, ""s));
    BOOST_TEST(splitPair("a="s, '=') == make_pair("a"s, ""s));
    BOOST_TEST(splitPair("=b"s, '=') == make_pair(""s, "b"s));
    BOOST_TEST(splitPair("a=b"s, '=') == make_pair("a"s, "b"s));
    BOOST_TEST(splitPair("a:b"s, ':') == make_pair("a"s, "b"s));
    BOOST_TEST(splitPair(" a = b "s, '=') == make_pair(" a "s, " b "s));
}

BOOST_AUTO_TEST_SUITE_END()
