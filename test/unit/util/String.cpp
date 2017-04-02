/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#include <swirly/util/String.hpp>

#include <swirly/unit/Test.hpp>

using namespace std;
using namespace swirly;

namespace {
constexpr string_view Name = "string_view"_sv;
} // anonymous

SWIRLY_TEST_CASE(StringEmpty)
{
    String<3> sb;
    SWIRLY_CHECK(sb.empty());
    SWIRLY_CHECK(sb.size() == 0UL);
    SWIRLY_CHECK(sb.compare(""_sv) == 0);
    SWIRLY_CHECK(sb == String<3>{""_sv});
}

SWIRLY_TEST_CASE(StringNonEmpty)
{
    String<3> sb{"Foo"_sv};
    SWIRLY_CHECK(!sb.empty());
    SWIRLY_CHECK(sb.size() == 3UL);
    SWIRLY_CHECK(sb.compare("Foo"_sv) == 0);
    SWIRLY_CHECK(sb == String<3>{"Foo"_sv});
}

SWIRLY_TEST_CASE(StringUpperBound)
{
    String<3> sb{"Foox"_sv};
    SWIRLY_CHECK(sb.size() == 3UL);
    SWIRLY_CHECK(sb == "Foo"_sv);

    sb = "Barx"_sv;
    SWIRLY_CHECK(sb.size() == 3UL);
    SWIRLY_CHECK(sb == "Bar"_sv);
}

SWIRLY_TEST_CASE(StringClear)
{
    String<3> sb{"Foo"_sv};
    sb.clear();
    SWIRLY_CHECK(sb.empty());
}

SWIRLY_TEST_CASE(StringAssign)
{
    String<3> sb;
    sb = "Foo"_sv;
    SWIRLY_CHECK(sb.size() == 3UL);
    SWIRLY_CHECK(sb == "Foo"_sv);

    sb.clear();
    sb = "Foox"_sv;
    SWIRLY_CHECK(sb.size() == 3UL);
    SWIRLY_CHECK(sb == "Foo"_sv);
}

SWIRLY_TEST_CASE(StringAppend)
{
    String<6> sb;
    sb = "Foo"_sv;
    sb += "Bar"_sv;
    SWIRLY_CHECK(sb.size() == 6UL);
    SWIRLY_CHECK(sb == "FooBar"_sv);

    sb = "Foo"_sv;
    sb += "Barx"_sv;
    SWIRLY_CHECK(sb.size() == 6UL);
    SWIRLY_CHECK(sb == "FooBar"_sv);
}

SWIRLY_TEST_CASE(StringToString)
{
    SWIRLY_CHECK(toString("foo"_sv) == "foo"s);
    SWIRLY_CHECK(stoi(toString(12345)) == 12345);
    SWIRLY_CHECK(stod(toString(12345.67)) == 12345.67);
}

SWIRLY_TEST_CASE(Stou64)
{
    SWIRLY_CHECK(stou64(""_sv) == 0UL);
    SWIRLY_CHECK(stou64("1"_sv) == 1UL);
    SWIRLY_CHECK(stou64("123"_sv) == 123UL);
    SWIRLY_CHECK(stou64(" "_sv) == 0UL);
    SWIRLY_CHECK(stou64(" 1 "_sv) == 1UL);
    SWIRLY_CHECK(stou64(" 123 "_sv) == 123UL);
    SWIRLY_CHECK(stou64("x"_sv) == 0UL);
    SWIRLY_CHECK(stou64(" 1x"_sv) == 1UL);
    SWIRLY_CHECK(stou64(" 123x"_sv) == 123UL);
    SWIRLY_CHECK(stou64("x1 "_sv) == 0UL);
    SWIRLY_CHECK(stou64("x123 "_sv) == 0UL);
    SWIRLY_CHECK(stou64("18446744073709551615"_sv) == 18446744073709551615ULL);
}

SWIRLY_TEST_CASE(Stob)
{
    SWIRLY_CHECK(stob(""_sv, false) == false);
    SWIRLY_CHECK(stob(""_sv, true) == true);

    SWIRLY_CHECK(stob("0"_sv, true) == false);
    SWIRLY_CHECK(stob("F"_sv, true) == false);
    SWIRLY_CHECK(stob("N"_sv, true) == false);
    SWIRLY_CHECK(stob("f"_sv, true) == false);
    SWIRLY_CHECK(stob("n"_sv, true) == false);

    SWIRLY_CHECK(stob("1"_sv, false) == true);
    SWIRLY_CHECK(stob("T"_sv, false) == true);
    SWIRLY_CHECK(stob("Y"_sv, false) == true);
    SWIRLY_CHECK(stob("t"_sv, false) == true);
    SWIRLY_CHECK(stob("y"_sv, false) == true);

    SWIRLY_CHECK(stob("NO"_sv, true) == false);
    SWIRLY_CHECK(stob("No"_sv, true) == false);
    SWIRLY_CHECK(stob("no"_sv, true) == false);

    SWIRLY_CHECK(stob("ON"_sv, false) == true);
    SWIRLY_CHECK(stob("On"_sv, false) == true);
    SWIRLY_CHECK(stob("on"_sv, false) == true);

    SWIRLY_CHECK(stob("OFF"_sv, true) == false);
    SWIRLY_CHECK(stob("Off"_sv, true) == false);
    SWIRLY_CHECK(stob("off"_sv, true) == false);

    SWIRLY_CHECK(stob("YES"_sv, false) == true);
    SWIRLY_CHECK(stob("Yes"_sv, false) == true);
    SWIRLY_CHECK(stob("yes"_sv, false) == true);

    SWIRLY_CHECK(stob("TRUE"_sv, false) == true);
    SWIRLY_CHECK(stob("True"_sv, false) == true);
    SWIRLY_CHECK(stob("true"_sv, false) == true);

    SWIRLY_CHECK(stob("FALSE"_sv, true) == false);
    SWIRLY_CHECK(stob("False"_sv, true) == false);
    SWIRLY_CHECK(stob("false"_sv, true) == false);
}

SWIRLY_TEST_CASE(LtrimCopy)
{
    SWIRLY_CHECK(ltrimCopy(""_sv) == ""_sv);
    SWIRLY_CHECK(ltrimCopy(" \t\n\v\f\r"_sv) == ""_sv);
    SWIRLY_CHECK(ltrimCopy(" \t\n\v\f\rfoo "_sv) == "foo "_sv);
    SWIRLY_CHECK(ltrimCopy("foo"_sv) == "foo"_sv);

    SWIRLY_CHECK(ltrimCopy(""s) == ""s);
    SWIRLY_CHECK(ltrimCopy(" \t\n\v\f\r"s) == ""s);
    SWIRLY_CHECK(ltrimCopy(" \t\n\v\f\rfoo "s) == "foo "s);
    SWIRLY_CHECK(ltrimCopy("foo"s) == "foo"s);
}

SWIRLY_TEST_CASE(RtrimCopy)
{
    SWIRLY_CHECK(rtrimCopy(""_sv) == ""_sv);
    SWIRLY_CHECK(rtrimCopy(" \t\n\v\f\r"_sv) == ""_sv);
    SWIRLY_CHECK(rtrimCopy(" foo \t\n\v\f\r"_sv) == " foo"_sv);
    SWIRLY_CHECK(rtrimCopy("foo"_sv) == "foo"_sv);

    SWIRLY_CHECK(rtrimCopy(""s) == ""s);
    SWIRLY_CHECK(rtrimCopy(" \t\n\v\f\r"s) == ""s);
    SWIRLY_CHECK(rtrimCopy(" foo \t\n\v\f\r"s) == " foo"s);
    SWIRLY_CHECK(rtrimCopy("foo"s) == "foo"s);
}

SWIRLY_TEST_CASE(TrimCopy)
{
    SWIRLY_CHECK(trimCopy(""_sv) == ""_sv);
    SWIRLY_CHECK(trimCopy(" \t\n\v\f\r"_sv) == ""_sv);
    SWIRLY_CHECK(trimCopy(" \t\n\v\f\rfoo \t\n\v\f\r"_sv) == "foo"_sv);
    SWIRLY_CHECK(trimCopy("foo"_sv) == "foo"_sv);

    SWIRLY_CHECK(trimCopy(""s) == ""s);
    SWIRLY_CHECK(trimCopy(" \t\n\v\f\r"s) == ""s);
    SWIRLY_CHECK(trimCopy(" \t\n\v\f\rfoo \t\n\v\f\r"s) == "foo"s);
    SWIRLY_CHECK(trimCopy("foo"s) == "foo"s);
}

SWIRLY_TEST_CASE(SplitPair)
{
    SWIRLY_CHECK(splitPair(""_sv, '=') == make_pair(""_sv, ""_sv));
    SWIRLY_CHECK(splitPair("="_sv, '=') == make_pair(""_sv, ""_sv));
    SWIRLY_CHECK(splitPair("a"_sv, '=') == make_pair("a"_sv, ""_sv));
    SWIRLY_CHECK(splitPair("a="_sv, '=') == make_pair("a"_sv, ""_sv));
    SWIRLY_CHECK(splitPair("=b"_sv, '=') == make_pair(""_sv, "b"_sv));
    SWIRLY_CHECK(splitPair("a=b"_sv, '=') == make_pair("a"_sv, "b"_sv));
    SWIRLY_CHECK(splitPair("a:b"_sv, ':') == make_pair("a"_sv, "b"_sv));
    SWIRLY_CHECK(splitPair(" a = b "_sv, '=') == make_pair(" a "_sv, " b "_sv));

    SWIRLY_CHECK(splitPair(""s, '=') == make_pair(""s, ""s));
    SWIRLY_CHECK(splitPair("="s, '=') == make_pair(""s, ""s));
    SWIRLY_CHECK(splitPair("a"s, '=') == make_pair("a"s, ""s));
    SWIRLY_CHECK(splitPair("a="s, '=') == make_pair("a"s, ""s));
    SWIRLY_CHECK(splitPair("=b"s, '=') == make_pair(""s, "b"s));
    SWIRLY_CHECK(splitPair("a=b"s, '=') == make_pair("a"s, "b"s));
    SWIRLY_CHECK(splitPair("a:b"s, ':') == make_pair("a"s, "b"s));
    SWIRLY_CHECK(splitPair(" a = b "s, '=') == make_pair(" a "s, " b "s));
}
