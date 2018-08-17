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

namespace swirly {
inline namespace util {
using namespace std;

namespace {
constexpr char Space[] = " \t\n\v\f\r";
} // namespace

void ltrim(string_view& s) noexcept
{
    const auto pos = s.find_first_not_of(Space);
    s.remove_prefix(pos != string_view::npos ? pos : s.size());
}

void ltrim(string& s) noexcept
{
    const auto pos = s.find_first_not_of(Space);
    s.erase(0, pos != string_view::npos ? pos : s.size());
}

void rtrim(string_view& s) noexcept
{
    const auto pos = s.find_last_not_of(Space);
    s.remove_suffix(s.size() - (pos != string_view::npos ? pos + 1 : 0));
}

void rtrim(string& s) noexcept
{
    const auto pos = s.find_last_not_of(Space);
    s.erase(pos != string_view::npos ? pos + 1 : 0);
}

pair<string_view, string_view> split_pair(string_view s, char delim) noexcept
{
    const auto pos = s.find_first_of(delim);
    string_view key, val;
    if (pos == string_view::npos) {
        key = s;
    } else {
        key = s.substr(0, pos);
        val = s.substr(pos + 1);
    }
    return {key, val};
}

pair<string, string> split_pair(const string& s, char delim)
{
    const auto pos = s.find_first_of(delim);
    string key, val;
    if (pos == string::npos) {
        key = s;
    } else {
        key = s.substr(0, pos);
        val = s.substr(pos + 1);
    }
    return {key, val};
}

} // namespace util
} // namespace swirly
