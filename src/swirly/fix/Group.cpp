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
#include "Group.hpp"

#include "Lexer.hpp"

#include <swirly/app/Exception.hpp>

namespace swirly {
inline namespace fix {
using namespace std;

void parse_group(FixLexer& lex, MdEntry& grp)
{
    constexpr string_view ErrMsg{"missing MdEntryType(269)"};
    if (lex.empty()) {
        throw ProtocolException{ErrMsg};
    }
    const auto [t, v] = lex.next();
    if (t != MdEntryType::Tag) {
        throw ProtocolException{ErrMsg};
    }
    grp.type = v[0];
    while (!lex.empty()) {
        const auto [t, v] = lex.top();
        switch (t) {
        case MdEntryPx::Tag:
            grp.px = v;
            break;
        case MdEntrySize::Tag:
            grp.size = v;
            break;
        default:
            // End of group.
            return;
        }
        lex.pop();
    }
}

} // namespace fix
} // namespace swirly
