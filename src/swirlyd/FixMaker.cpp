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
#include "FixMaker.hxx"

#include <swirly/lob/App.hpp>
#include <swirly/lob/Sess.hpp>

#include <swirly/fin/MarketId.hpp>

#include <swirly/fix/Conn.hpp>

#include <swirly/util/Log.hpp>

namespace swirly {
using namespace std;

FixMaker::FixMaker(CyclTime now, Reactor& r, const FixSessId& sess_id, const Config& config,
                   LobApp& lob_app)
: lob_app_(lob_app)
, sess_(lob_app_.sess(config.get<string_view>("accnt")))
{
    lob_app_.set_trade_slot(sess_, bind<&FixMaker::on_trade>(this));
}

FixMaker::~FixMaker()
{
    lob_app_.set_trade_slot(sess_, {});
}

void FixMaker::do_on_logon(CyclTime now, FixConn& conn, const FixSessId& sess_id)
{
    SWIRLY_INFO << sess_id << " <Maker> on_logon";
    conn_ = &conn;
}

void FixMaker::do_on_logout(CyclTime now, FixConn& conn, const FixSessId& sess_id,
                            Disconnect disconnect) noexcept
{
    if (disconnect == Disconnect::No) {
        SWIRLY_INFO << sess_id << " <Maker> on_logout";
    } else {
        SWIRLY_WARNING << sess_id << " <Maker> on_logout";
    }
    conn_ = nullptr;
}

void FixMaker::do_on_message(CyclTime now, FixConn& conn, string_view msg, size_t body_off,
                             Version ver, const FixHeader& hdr)
{
    if (hdr.msg_type.value == "W") {
        on_market_data_snapshot(now, conn, msg, body_off, ver, hdr);
    } else {
        SWIRLY_INFO << conn.sess_id() << " <Maker> on_message: " << hdr.msg_type.value;
    }
}

void FixMaker::do_on_error(CyclTime now, const FixConn& conn, const std::exception& e) noexcept
{
    SWIRLY_ERROR << conn.sess_id() << " <Maker> on_error: " << e.what();
}

void FixMaker::do_on_timeout(CyclTime now, const FixConn& conn) noexcept
{
    SWIRLY_WARNING << conn.sess_id() << " <Maker> on_timeout";
}

void FixMaker::do_prepare(CyclTime now, const FixSessId& sess_id, const FixHandlerMap& handler_map)
{
    SWIRLY_INFO << sess_id << " <Maker> prepare";
}

void FixMaker::do_send(CyclTime now, string_view msg_type, string_view msg)
{
    if (conn_) {
        auto fn = [msg](CyclTime now, ostream& os) { os << msg; };
        conn_->send(now, msg_type, fn);
    }
}

void FixMaker::on_market_data_snapshot(CyclTime now, FixConn& conn, string_view msg,
                                       size_t body_off, Version ver, const FixHeader& hdr)
{
    mds_.clear();

    parse_body(msg, body_off, mds_);

    const auto& instr = lob_app_.instr(mds_.symbol.value);
    const auto market_id = to_market_id(instr.id(), mds_.maturity_date.value);
    const auto& market = lob_app_.market(market_id);
    auto& orders = order_map_[market_id];

    for (const auto& order : orders) {
        lob_app_.try_cancel_quote(now, sess_, market, *order);
    }
    orders.clear();
    for (const auto& md_entry : mds_.md_entries) {
        Side side;
        switch (md_entry.type.value) {
        case '0':
            side = Side::Buy;
            break;
        case '1':
            side = Side::Sell;
            break;
        default:
            continue;
        }
        const Lots lots{md_entry.size.value};
        const Ticks ticks{md_entry.px.value};
        orders.push_back(lob_app_.create_quote(now, sess_, market, ""sv, side, lots, ticks, 1_lts));
    }
}

void FixMaker::on_trade(CyclTime now, const Sess& sess, const ExecPtr& trade)
{
    SWIRLY_INFO << sess.accnt() << " <Maker> trade: " << trade->id();
    lob_app_.archive_trade(now, sess, *trade);
}

} // namespace swirly
