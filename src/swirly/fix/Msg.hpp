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

#include <swirly/fix/Field.hpp>

namespace swirly {
inline namespace fix {
class FixLexer;

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
SWIRLY_API void parse_body(std::string_view msg, std::size_t body_off, Logon& body);

struct MdEntry {
    MdEntryType type;
    MdEntryPx px;
    MdEntrySize size;
};

template <typename StreamT>
StreamT& operator<<(StreamT& os, const MdEntry& grp)
{
    os << grp.type << grp.px << grp.size;
    return os;
}

SWIRLY_API void parse_body(FixLexer& lex, MdEntry& grp);

struct MarketDataSnapshot {
    void clear()
    {
        symbol.clear();
        maturity_date.clear();
        md_entries.clear();
    }
    SymbolField::View symbol;
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
SWIRLY_API void parse_body(std::string_view msg, std::size_t body_off, MarketDataSnapshot& body);

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
SWIRLY_API void parse_body(std::string_view msg, std::size_t body_off, TradingSessionStatus& body);

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_MSG_HPP
