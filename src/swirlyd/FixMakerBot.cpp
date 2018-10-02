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
#include "FixMakerBot.hxx"

#include <swirly/fix/Conn.hpp>

#include <swirly/util/Log.hpp>

namespace swirly {
using namespace std;

FixMakerBot::~FixMakerBot() = default;

void FixMakerBot::do_on_logon(CyclTime now, FixConn& conn, const FixSessId& sess_id)
{
    SWIRLY_INFO << sess_id << " <MakerBot> on_logon";
    conn_ = &conn;
    md_tmr_ = reactor_.timer(now.mono_time() + 1s, 1s, Priority::Low,
                             bind<&FixMakerBot::on_market_data>(this));
    stat_tmr_ = reactor_.timer(now.mono_time() + 10s, 10s, Priority::Low,
                               bind<&FixMakerBot::on_status>(this));
}

void FixMakerBot::do_on_logout(CyclTime now, FixConn& conn, const FixSessId& sess_id,
                               bool disconnect) noexcept
{
    if (!disconnect) {
        SWIRLY_INFO << sess_id << " <MakerBot> on_logout";
    } else {
        SWIRLY_WARNING << sess_id << " <MakerBot> on_logout";
    }
    conn_ = nullptr;
}

void FixMakerBot::do_on_message(CyclTime now, FixConn& conn, string_view msg, size_t body_off,
                                Version ver, const FixHeader& hdr)
{
    SWIRLY_INFO << conn.sess_id() << " <MakerBot> on_message: " << hdr.msg_type.value;
}

void FixMakerBot::do_on_error(CyclTime now, const FixConn& conn, const std::exception& e) noexcept
{
    SWIRLY_ERROR << conn.sess_id() << " <MakerBot> on_error: " << e.what();
}

void FixMakerBot::do_on_timeout(CyclTime now, const FixConn& conn) noexcept
{
    SWIRLY_WARNING << conn.sess_id() << " <MakerBot> on_timeout";
}

void FixMakerBot::do_prepare(CyclTime now, const FixSessId& sess_id,
                             const FixHandlerMap& handler_map)
{
    SWIRLY_INFO << sess_id << " <MakerBot> prepare";
}

void FixMakerBot::do_send(CyclTime now, string_view msg_type, string_view msg)
{
    if (conn_) {
        auto fn = [msg](CyclTime now, ostream& os) { os << msg; };
        conn_->send(now, msg_type, fn);
    }
}

void FixMakerBot::on_market_data(CyclTime now, Timer& tmr)
{
    assert(conn_);
    auto fn = [this](CyclTime now, ostream& os) {
        const auto [bid, offer] = bbo_(12345);
        // clang-format off
            os << NoMdEntries{4}
               << MdEntryType{byte{0}}
               << MdEntryPx{bid - 1}
               << MdEntrySize{2000}
               << MdEntryType{byte{0}}
               << MdEntryPx{bid}
               << MdEntrySize{1000}
               << MdEntryType{byte{1}}
               << MdEntryPx{offer}
               << MdEntrySize{1000}
               << MdEntryType{byte{1}}
               << MdEntryPx{offer + 1}
               << MdEntrySize{2000};
        // clang-format on
    };
    conn_->send(now, "W"sv, fn);
}

void FixMakerBot::on_status(CyclTime now, Timer& tmr)
{
    assert(conn_);
    auto fn = [](CyclTime now, ostream& os) {
        os << TradingSessionId::View{"OPEN"} << TradSesStatus{2};
    };
    conn_->send(now, "h"sv, fn);
}

} // namespace swirly
