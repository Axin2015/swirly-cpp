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
            body.maturity_date = v;
            break;
        case ExecId::Tag:
            body.exec_id = v;
            break;
        case OrderId::Tag:
            body.order_id = v;
            break;
        case ExecType::Tag:
            body.exec_type = v;
            break;
        case OrdStatus::Tag:
            body.ord_status = v;
            break;
        case SideField::Tag:
            body.side = v;
            break;
        case OrderQty::Tag:
            body.order_qty = v;
            break;
        case Price::Tag:
            body.price = v;
            break;
        case LeavesQty::Tag:
            body.leaves_qty = v;
            break;
        case CumQty::Tag:
            body.cum_qty = v;
            break;
        case AvgPx::Tag:
            body.avg_px = v;
            break;
        case LastQty::Tag:
            body.last_qty = v;
            break;
        case LastPx::Tag:
            body.last_px = v;
            break;
        case MinQty::Tag:
            body.min_qty = v;
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
            body.encrypt_method = v;
            break;
        case HeartBtInt::Tag:
            body.heart_bt_int = v;
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
            body.maturity_date = v;
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
            body.trad_ses_status = v;
            break;
        }
    }
}

} // namespace fix
} // namespace swirly
