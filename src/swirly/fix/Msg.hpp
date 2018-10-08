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
#ifndef SWIRLY_FIX_MSG_HPP
#define SWIRLY_FIX_MSG_HPP

#include <swirly/fix/Group.hpp>
#include <swirly/fix/Lexer.hpp>

namespace swirly {
inline namespace fix {

struct ExecutionReport {
    void clear() { order_id.clear(); }
    SymbolField symbol;
    MaturityDate maturity_date;
    ExecId exec_id;
    OrderId order_id;
};

template <typename StreamT>
StreamT& operator<<(StreamT& os, const ExecutionReport& body)
{
    os << body.symbol << body.maturity_date << body.exec_id << body.order_id;
    return os;
}

SWIRLY_API void parse_body(FixLexer& lex, ExecutionReport& body);

struct Logon {
    void clear()
    {
        encrypt_method.clear();
        heart_bt_int.clear();
    }
    EncryptMethod encrypt_method;
    HeartBtInt heart_bt_int;
};

template <typename StreamT>
StreamT& operator<<(StreamT& os, const Logon& body)
{
    os << body.encrypt_method << body.heart_bt_int;
    return os;
}

SWIRLY_API void parse_body(FixLexer& lex, Logon& body);

struct MarketDataSnapshot {
    void clear()
    {
        symbol.clear();
        maturity_date.clear();
        md_entries.clear();
    }
    SymbolField symbol;
    MaturityDate maturity_date;
    std::vector<MdEntry> md_entries;
};

template <typename StreamT>
StreamT& operator<<(StreamT& os, const MarketDataSnapshot& body)
{
    os << body.symbol << body.maturity_date << NoMdEntries{body.md_entries.size()};
    for (const auto& md_entry : body.md_entries) {
        os << md_entry;
    }
    return os;
}

SWIRLY_API void parse_body(FixLexer& lex, MarketDataSnapshot& body);

struct TradingSessionStatus {
    void clear()
    {
        trading_session_id.clear();
        trad_ses_status.clear();
    }
    TradingSessionId::View trading_session_id;
    TradSesStatus trad_ses_status;
};

template <typename StreamT>
StreamT& operator<<(StreamT& os, const TradingSessionStatus& body)
{
    os << body.trading_session_id << body.trad_ses_status;
    return os;
}

SWIRLY_API void parse_body(FixLexer& lex, TradingSessionStatus& body);

template <typename BodyT>
void parse_body(std::string_view msg, std::size_t body_off, BodyT& body)
{
    msg.remove_suffix(CheckSumLen);
    FixLexer lex{msg, body_off};
    parse_body(lex, body);
}

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_MSG_HPP
