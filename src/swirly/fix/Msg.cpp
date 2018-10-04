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

#include "Lexer.hpp"

#include <swirly/app/Exception.hpp>

namespace swirly {
inline namespace fix {
using namespace std;

void parse_body(FixLexer& lex, Logon& body)
{
    while (!lex.empty()) {
        const auto [t, v] = lex.next();
        switch (t) {
        case EncryptMethod::Tag:
            body.encrypt_method = from_string<EncryptMethod::Type>(v);
            break;
        case HeartBtInt::Tag:
            body.heart_bt_int = from_string<HeartBtInt::Type>(v);
            break;
        }
    }
}

void parse_body(string_view msg, size_t body_off, Logon& body)
{
    msg.remove_suffix(CheckSumLen);
    FixLexer lex{msg, body_off};
    parse_body(lex, body);
}

void parse_body(FixLexer& lex, MdEntry& grp)
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
            grp.px = from_string<MdEntryPx::Type>(v);
            break;
        case MdEntrySize::Tag:
            grp.size = from_string<MdEntrySize::Type>(v);
            break;
        default:
            // End of group.
            return;
        }
        lex.pop();
    }
}

void parse_body(FixLexer& lex, MarketDataSnapshot& body)
{
    while (!lex.empty()) {
        const auto [t, v] = lex.next();
        switch (t) {
        case SymbolField::Tag:
            body.symbol = v;
            break;
        case MaturityDate::Tag:
            body.maturity_date = from_string<IsoDate>(v);
            break;
        case NoMdEntries::Tag: {
            const auto n = from_string<NoMdEntries::Type>(v);
            body.md_entries.reserve(body.md_entries.size() + n);
            for (NoMdEntries::Type i{0}; i < n; ++i) {
                body.md_entries.emplace_back();
                parse_body(lex, body.md_entries.back());
            }
            break;
        }
        }
    }
}

void parse_body(string_view msg, size_t body_off, MarketDataSnapshot& body)
{
    msg.remove_suffix(CheckSumLen);
    FixLexer lex{msg, body_off};
    parse_body(lex, body);
}

void parse_body(FixLexer& lex, TradingSessionStatus& body)
{
    while (!lex.empty()) {
        const auto [t, v] = lex.next();
        switch (t) {
        case TradingSessionId::Tag:
            body.trading_session_id = v;
            break;
        case TradSesStatus::Tag:
            body.trad_ses_status = from_string<TradSesStatus::Type>(v);
            break;
        }
    }
}

void parse_body(string_view msg, size_t body_off, TradingSessionStatus& body)
{
    msg.remove_suffix(CheckSumLen);
    FixLexer lex{msg, body_off};
    parse_body(lex, body);
}

} // namespace fix
} // namespace swirly
