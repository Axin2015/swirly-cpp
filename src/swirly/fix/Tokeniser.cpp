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

#include <swirly/fin/Exception.hpp>

#include <cassert>
#include <cctype>

namespace swirly {
inline namespace fix {
using namespace std;
pair<string_view, bool> find_tag(string_view msg, int tag) noexcept
{
    assert(tag >= 0);

    auto pos = msg.find('=');
    if (pos == string_view::npos) {
        return {{}, false};
    }
    ++pos;

    // \10123456789=
    char arr[12];
    auto* end = &arr[sizeof(arr) - 1];

    // Format tag plus equals sign.
    auto* it = end - 1;
    *it = '=';
    do {
        const auto d = tag % 10;
        *--it = '0' + d;
        tag /= 10;
    } while (tag > 0);

    // Compare first field.
    size_t len = end - it;
    if (len != pos || msg.compare(0, pos, it, len) != 0) {

        // Otherwise, search remaining fields.
        *--it = '\1';
        ++len;
        pos = msg.find({it, len}, pos);
        if (pos == string_view::npos) {
            return {{}, false};
        }
        pos += len;
    }
    if ((len = msg.find('\1', pos)) != string_view::npos) {
        len -= pos;
    }
    return {msg.substr(pos, len), true};
}

int FixTokeniser::tag(string_view::const_iterator& it) const
{
    int tag{0};
    assert(it != msg_.cend());
    if (isdigit(*it)) {
        tag += *it++ - '0';
        for (;;) {
            if (it == msg_.cend()) {
                throw ProtocolException{"partial FIX tag"};
            }
            if (!isdigit(*it)) {
                break;
            }
            tag *= 10;
            tag += *it++ - '0';
        }
    }
    // Verify that first non-digit charactor was '=' delimiter.
    if (*it != '=') {
        throw ProtocolException{"invalid FIX tag"};
    }
    // Skip delimiter.
    ++it;
    return tag;
}

string_view FixTokeniser::value(string_view::const_iterator& it) const noexcept
{
    const auto begin = it;
    auto pos = string_view::npos;
    for (; it != msg_.cend(); ++it) {
        if (*it == '\1') {
            pos = it++ - begin;
            break;
        }
    }
    return msg_.substr(begin - msg_.cbegin(), pos);
}
} // namespace fix
} // namespace swirly
