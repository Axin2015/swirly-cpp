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
#include "Msg.hpp"

namespace swirly {
inline namespace fix {
using namespace std;

void parse_body(FixLexer& lex, ExecutionReport& body)
{
    parse_fix_list(lex, body);
}

void parse_body(FixLexer& lex, Logon& body)
{
    parse_fix_list(lex, body);
}

void parse_body(FixLexer& lex, MarketDataSnapshot& body)
{
    using NoMdEntries = FixValueType<Tag::NoMdEntries>;

    while (!lex.empty()) {
        const auto [t, v] = lex.next();
        if (t == +Tag::NoMdEntries) {
            const auto n = parse_fix_value<Tag::NoMdEntries>(v);
            body.md_entries.reserve(body.md_entries.size() + n);
            for (NoMdEntries i{0}; i < n; ++i) {
                body.md_entries.emplace_back();
                parse_group(lex, body.md_entries.back());
            }
        } else {
            body.set(box<Tag>(t), v);
        }
    }
}

void parse_body(FixLexer& lex, TradingSessionStatus& body)
{
    parse_fix_list(lex, body);
}

} // namespace fix
} // namespace swirly
