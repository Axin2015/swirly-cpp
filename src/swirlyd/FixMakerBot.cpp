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

    std::uniform_int_distribution<int> dist{0, 10};
    md_tmr_ = reactor_.timer(now.mono_time() + Millis{dist(rd_) * 100}, 1s, Priority::Low,
                             bind<&FixMakerBot::on_market_data>(this));
    stat_tmr_ = reactor_.timer(now.mono_time() + Seconds{dist(rd_)}, 10s, Priority::Low,
                               bind<&FixMakerBot::on_status>(this));
}

void FixMakerBot::do_on_logout(CyclTime now, FixConn& conn, const FixSessId& sess_id,
                               Disconnect disconnect) noexcept
{
    if (disconnect == Disconnect::No) {
        SWIRLY_INFO << sess_id << " <MakerBot> on_logout";
    } else {
        SWIRLY_WARNING << sess_id << " <MakerBot> on_logout";
    }
    count_ = 0;
    stat_tmr_.cancel();
    md_tmr_.cancel();
    conn_ = nullptr;
    conn.logon(now, sess_id);
}

void FixMakerBot::do_on_message(CyclTime now, FixConn& conn, string_view msg, size_t body_off,
                                Version ver, const FixHeader& hdr)
{
    SWIRLY_INFO << conn.sess_id() << " <MakerBot> on_message: " << hdr.msg_type.value;
    if (hdr.msg_type.value == "8") {
        msg.remove_prefix(body_off);
        msg.remove_suffix(CheckSumLen);
        conn_->send(now, "8"sv, msg);
    }
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
        conn_->send(now, msg_type, msg);
    }
}

void FixMakerBot::on_market_data(CyclTime now, Timer& tmr)
{
    assert(conn_);
    auto fn = [this](CyclTime now, ostream& os) {
        const auto [bid, offer] = bbo_(12345);
        // clang-format off
        os << SymbolField{"EURUSD"sv}
           << NoMdEntries{6}
           << MdEntryType{'0'}
           << MdEntryPx{bid - 2}
           << MdEntrySize{3000}
           << MdEntryType{'0'}
           << MdEntryPx{bid - 1}
           << MdEntrySize{2000}
           << MdEntryType{'0'}
           << MdEntryPx{bid}
           << MdEntrySize{1000}
           << MdEntryType{'1'}
           << MdEntryPx{offer}
           << MdEntrySize{1000}
           << MdEntryType{'1'}
           << MdEntryPx{offer + 1}
           << MdEntrySize{2000}
           << MdEntryType{'1'}
           << MdEntryPx{offer + 2}
           << MdEntrySize{3000};
        // clang-format on
    };
    conn_->send(now, "W"sv, fn);
}

void FixMakerBot::on_status(CyclTime now, Timer& tmr)
{
    assert(conn_);
    if (count_++ < 3) {
        auto fn = [](CyclTime now, ostream& os) {
            os << TradingSessionId::View{"OPEN"} << TradSesStatus{2};
        };
        conn_->send(now, "h"sv, fn);
    } else {
        conn_->logout(now);
    }
}

} // namespace swirly
