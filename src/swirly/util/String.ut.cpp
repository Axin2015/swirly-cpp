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

BOOST_AUTO_TEST_CASE(StringViewCase)
{
    // Make sure the string is not optimised away.
    const auto s = to_string(time(nullptr));

    // Ensure that std::string_view iterators are equivalent across copies of the std::string_view.
    const string_view a{s};
    const string_view b{a};

    // Best effort to detect broken assumption.
    BOOST_TEST(a.begin() == b.begin());
    BOOST_TEST(a.end() == b.end());
}

BOOST_AUTO_TEST_CASE(ToStringCase)
{
    BOOST_TEST(to_string("foo"sv) == "foo"s);
    BOOST_TEST(stoi(to_string(12345)) == 12345);
    BOOST_TEST(stod(to_string(12345.67)) == 12345.67);
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
    BOOST_TEST(ltrim_copy(""sv) == ""sv);
    BOOST_TEST(ltrim_copy(" \t\n\v\f\r"sv) == ""sv);
    BOOST_TEST(ltrim_copy(" \t\n\v\f\rfoo "sv) == "foo "sv);
    BOOST_TEST(ltrim_copy("foo"sv) == "foo"sv);

    BOOST_TEST(ltrim_copy(""s) == ""s);
    BOOST_TEST(ltrim_copy(" \t\n\v\f\r"s) == ""s);
    BOOST_TEST(ltrim_copy(" \t\n\v\f\rfoo "s) == "foo "s);
    BOOST_TEST(ltrim_copy("foo"s) == "foo"s);
}

BOOST_AUTO_TEST_CASE(RtrimCopyCase)
{
    BOOST_TEST(rtrim_copy(""sv) == ""sv);
    BOOST_TEST(rtrim_copy(" \t\n\v\f\r"sv) == ""sv);
    BOOST_TEST(rtrim_copy(" foo \t\n\v\f\r"sv) == " foo"sv);
    BOOST_TEST(rtrim_copy("foo"sv) == "foo"sv);

    BOOST_TEST(rtrim_copy(""s) == ""s);
    BOOST_TEST(rtrim_copy(" \t\n\v\f\r"s) == ""s);
    BOOST_TEST(rtrim_copy(" foo \t\n\v\f\r"s) == " foo"s);
    BOOST_TEST(rtrim_copy("foo"s) == "foo"s);
}

BOOST_AUTO_TEST_CASE(TrimCopyCase)
{
    BOOST_TEST(trim_copy(""sv) == ""sv);
    BOOST_TEST(trim_copy(" \t\n\v\f\r"sv) == ""sv);
    BOOST_TEST(trim_copy(" \t\n\v\f\rfoo \t\n\v\f\r"sv) == "foo"sv);
    BOOST_TEST(trim_copy("foo"sv) == "foo"sv);

    BOOST_TEST(trim_copy(""s) == ""s);
    BOOST_TEST(trim_copy(" \t\n\v\f\r"s) == ""s);
    BOOST_TEST(trim_copy(" \t\n\v\f\rfoo \t\n\v\f\r"s) == "foo"s);
    BOOST_TEST(trim_copy("foo"s) == "foo"s);
}

BOOST_AUTO_TEST_CASE(SplitPairCase)
{
    BOOST_TEST(split_pair(""sv, '=') == make_pair(""sv, ""sv));
    BOOST_TEST(split_pair("="sv, '=') == make_pair(""sv, ""sv));
    BOOST_TEST(split_pair("a"sv, '=') == make_pair("a"sv, ""sv));
    BOOST_TEST(split_pair("a="sv, '=') == make_pair("a"sv, ""sv));
    BOOST_TEST(split_pair("=b"sv, '=') == make_pair(""sv, "b"sv));
    BOOST_TEST(split_pair("a=b"sv, '=') == make_pair("a"sv, "b"sv));
    BOOST_TEST(split_pair("a:b"sv, ':') == make_pair("a"sv, "b"sv));
    BOOST_TEST(split_pair(" a = b "sv, '=') == make_pair(" a "sv, " b "sv));

    BOOST_TEST(split_pair(""s, '=') == make_pair(""s, ""s));
    BOOST_TEST(split_pair("="s, '=') == make_pair(""s, ""s));
    BOOST_TEST(split_pair("a"s, '=') == make_pair("a"s, ""s));
    BOOST_TEST(split_pair("a="s, '=') == make_pair("a"s, ""s));
    BOOST_TEST(split_pair("=b"s, '=') == make_pair(""s, "b"s));
    BOOST_TEST(split_pair("a=b"s, '=') == make_pair("a"s, "b"s));
    BOOST_TEST(split_pair("a:b"s, ':') == make_pair("a"s, "b"s));
    BOOST_TEST(split_pair(" a = b "s, '=') == make_pair(" a "s, " b "s));
}

BOOST_AUTO_TEST_CASE(PstrlenCase)
{
    constexpr char ZeroPad[] = "foo";
    BOOST_TEST(pstrlen<'\0'>(ZeroPad, ~0) == 3UL);
    BOOST_TEST(pstrlen<'\0'>(ZeroPad, 2) == 2UL);
    BOOST_TEST(pstrlen<'\0'>(ZeroPad) == 3UL);

    constexpr char SpacePad[] = "foo ";
    BOOST_TEST(pstrlen<' '>(SpacePad, ~0) == 3UL);
    BOOST_TEST(pstrlen<' '>(SpacePad, 2) == 2UL);
    BOOST_TEST(pstrlen<' '>(SpacePad) == 3UL);
}

BOOST_AUTO_TEST_CASE(PstrcpyCase)
{
    struct SWIRLY_PACKED {
        char data[8];
        char canary;
    } buf;

    auto clear = [&]() { memset(buf.data, '#', sizeof(buf.data) + 1); };

    clear();
    BOOST_TEST(memcmp(buf.data, "########", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    // Zero / C-String / Explicit Length.
    clear();
    BOOST_TEST(pstrcpy<'\0'>(buf.data, "foobar", sizeof(buf.data)) == 6UL);
    BOOST_TEST(memcmp(buf.data, "foobar\0\0", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    clear();
    BOOST_TEST(pstrcpy<'\0'>(buf.data, "foobarbaz", sizeof(buf.data)) == 8UL);
    BOOST_TEST(memcmp(buf.data, "foobarba", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    // Zero / C-String / Implied Length.
    clear();
    BOOST_TEST(pstrcpy<'\0'>(buf.data, "foobar") == 6UL);
    BOOST_TEST(memcmp(buf.data, "foobar\0\0", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    clear();
    BOOST_TEST(pstrcpy<'\0'>(buf.data, "foobarbaz") == 8UL);
    BOOST_TEST(memcmp(buf.data, "foobarba", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    // Zero / String View / Explicit Length.
    clear();
    BOOST_TEST(pstrcpy<'\0'>(buf.data, "foobar"sv, sizeof(buf.data)) == 6UL);
    BOOST_TEST(memcmp(buf.data, "foobar\0\0", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    clear();
    BOOST_TEST(pstrcpy<'\0'>(buf.data, "foobarbaz"sv, sizeof(buf.data)) == 8UL);
    BOOST_TEST(memcmp(buf.data, "foobarba", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    // Zero / String View / Implied Length.
    clear();
    BOOST_TEST(pstrcpy<'\0'>(buf.data, "foobar"sv) == 6UL);
    BOOST_TEST(memcmp(buf.data, "foobar\0\0", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    clear();
    BOOST_TEST(pstrcpy<'\0'>(buf.data, "foobarbaz"sv) == 8UL);
    BOOST_TEST(memcmp(buf.data, "foobarba", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    // Space / C-String / Explicit Length.
    clear();
    BOOST_TEST(pstrcpy<' '>(buf.data, "foobar", sizeof(buf.data)) == 6UL);
    BOOST_TEST(memcmp(buf.data, "foobar  ", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    clear();
    BOOST_TEST(pstrcpy<' '>(buf.data, "foobarbaz", sizeof(buf.data)) == 8UL);
    BOOST_TEST(memcmp(buf.data, "foobarba", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    // Space / C-String / Implied Length.
    clear();
    BOOST_TEST(pstrcpy<' '>(buf.data, "foobar") == 6UL);
    BOOST_TEST(memcmp(buf.data, "foobar  ", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    clear();
    BOOST_TEST(pstrcpy<' '>(buf.data, "foobarbaz") == 8UL);
    BOOST_TEST(memcmp(buf.data, "foobarba", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    // Space / String View / Explicit Length.
    clear();
    BOOST_TEST(pstrcpy<' '>(buf.data, "foobar"sv, sizeof(buf.data)) == 6UL);
    BOOST_TEST(memcmp(buf.data, "foobar  ", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    clear();
    BOOST_TEST(pstrcpy<' '>(buf.data, "foobarbaz"sv, sizeof(buf.data)) == 8UL);
    BOOST_TEST(memcmp(buf.data, "foobarba", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    // Space / String View / Implied Length.
    clear();
    BOOST_TEST(pstrcpy<' '>(buf.data, "foobar"sv) == 6UL);
    BOOST_TEST(memcmp(buf.data, "foobar  ", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');

    clear();
    BOOST_TEST(pstrcpy<' '>(buf.data, "foobarbaz"sv) == 8UL);
    BOOST_TEST(memcmp(buf.data, "foobarba", sizeof(buf.data)) == 0);
    BOOST_TEST(buf.canary == '#');
}

BOOST_AUTO_TEST_SUITE_END()
