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
#include "Header.hpp"

#include "Lexer.hpp"

#include <swirly/app/Exception.hpp>

#include <swirly/util/Date.hpp>

#include <algorithm>

namespace swirly {
inline namespace fix {
using namespace std;
namespace {

// Sorted array of FIX header tags.
// See https://www.onixs.biz/fix-dictionary/5.0/compBlock_StandardHeader.html
constexpr std::array<int, 32> HeaderTags{8,   9,   34,  35,  43,  49,  50,  52,  56,   57,  90,
                                         91,  97,  115, 116, 122, 128, 129, 142, 143,  144, 145,
                                         212, 213, 347, 369, 627, 628, 629, 630, 1128, 1129};
static_assert(!HeaderTags.empty());

inline bool is_header_tag(int tag) noexcept
{
    return binary_search(HeaderTags.begin(), HeaderTags.end(), tag);
}

template <typename HeaderT>
size_t do_parse_header(string_view msg, size_t msg_type_off, HeaderT& hdr)
{
    FixLexer lex{msg, msg_type_off};
    assert(!lex.empty());
    do {
        const auto [t, v] = lex.top();
        if (!is_header_tag(t)) {
            break;
        }
        hdr.set(box<Tag>(t), v);
        lex.pop();
    } while (!lex.empty());
    return lex.offset();
}

} // namespace

size_t parse_header(string_view msg, size_t msg_type_off, FixHeader& hdr)
{
    return do_parse_header(msg, msg_type_off, hdr);
}

size_t parse_header(string_view msg, size_t msg_type_off, FixHeaderView& hdr)
{
    return do_parse_header(msg, msg_type_off, hdr);
}

} // namespace fix
} // namespace swirly
