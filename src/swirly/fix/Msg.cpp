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
    while (!lex.empty()) {
        const auto [t, v] = lex.next();
        switch (t) {
        case SymbolField::Tag:
            body.symbol = v;
            break;
        case MaturityDate::Tag:
            body.maturity_date = from_string<IsoDate>(v);
            break;
        case ExecId::Tag:
            body.exec_id = from_string<ExecId::Type>(v);
            break;
        case OrderId::Tag:
            body.order_id = from_string<OrderId::Type>(v);
            break;
        }
    }
}

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
                parse_group(lex, body.md_entries.back());
            }
            break;
        }
        }
    }
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

} // namespace fix
} // namespace swirly
