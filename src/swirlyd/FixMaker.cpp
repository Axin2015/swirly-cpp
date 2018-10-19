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
namespace {
inline Ticks to_avg_ticks(Lots lots, Cost cost) noexcept
{
    Ticks ticks = 0_tks;
    if (lots != 0_lts) {
        ticks = Ticks{fract_to_real(cost.count(), lots.count()) + 0.5};
    }
    return ticks;
}
} // namespace

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
    // Cancel all orders on logout.
    const auto& markets = lob_app_.markets(); // noexcept
    for (auto& [market_id, orders] : order_map_) {
        const auto it = markets.find(market_id);
        if (it == markets.end()) {
            // Defensive: this should not happen.
            SWIRLY_WARNING << sess_id << " <Maker> market not found: " << market_id;
            continue;
        }
        for (const auto& order : orders) {
            lob_app_.try_cancel_quote(now, sess_, *it, *order); // noexcept
        }
    }
    conn_ = nullptr;
}

void FixMaker::do_on_message(CyclTime now, FixConn& conn, string_view msg, size_t body_off,
                             Version ver, const FixHeaderView& hdr)
{
    const auto& msg_type = get<Tag::MsgType>(hdr);
    if (msg_type == "8"sv) {
        on_execution_report(now, conn, msg, body_off, ver, hdr);
    } else if (msg_type == "W"sv) {
        on_market_data_snapshot(now, conn, msg, body_off, ver, hdr);
    } else {
        SWIRLY_INFO << conn.sess_id() << " <Maker> on_message: " << msg_type;
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
        conn_->send(now, msg_type, msg);
    }
}

void FixMaker::on_execution_report(CyclTime now, FixConn& conn, string_view msg, size_t body_off,
                                   Version ver, const FixHeaderView& hdr)
{
    SWIRLY_INFO << conn.sess_id() << " <Maker> on_execution_report";

    ExecutionReport er;
    parse_body(msg, body_off, er);

    // Archive on trade acknowledgement.

    const auto& instr = lob_app_.instr(get<Tag::Symbol>(er));
    const auto market_id = to_market_id(instr.id(), get<Tag::MaturityDate>(er));
    lob_app_.archive_trade(now, sess_, market_id, get<Tag::ExecId>(er));
}

void FixMaker::on_market_data_snapshot(CyclTime now, FixConn& conn, string_view msg,
                                       size_t body_off, Version ver, const FixHeaderView& hdr)
{
    mds_.clear();
    parse_body(msg, body_off, mds_);

    const auto& instr = lob_app_.instr(get<Tag::Symbol>(mds_));
    const auto market_id = to_market_id(instr.id(), get<Tag::MaturityDate>(mds_));
    const auto& market = lob_app_.market(market_id);

    auto& orders = order_map_[market_id];
    for (const auto& order : orders) {
        lob_app_.try_cancel_quote(now, sess_, market, *order); // noexcept
    }
    orders.clear();
    for (const auto& md_entry : mds_.md_entries) {
        const Side side{get<Tag::MdEntryType>(md_entry).side};
        const Lots lots{get<Tag::MdEntrySize>(md_entry)};
        const Ticks ticks{get<Tag::MdEntryPx>(md_entry)};
        orders.push_back(lob_app_.create_quote(now, sess_, market, ""sv, side, lots, ticks, 1_lts));
    }
}

void FixMaker::on_trade(CyclTime now, const Sess& sess, const ExecPtr& trade)
{
    if (conn_) {
        auto fn = [&trade](CyclTime now, ostream& os) {
            // clang-format off
            os << put_fix<Tag::Symbol>(trade->instr())
               << put_fix<Tag::MaturityDate>(maybe_jd_to_iso(trade->settl_day()))
               << put_fix<Tag::ExecId>(trade->id())
               << put_fix<Tag::OrderId>(trade->order_id())
               << put_fix<Tag::ExecType>(trade->state())
               << put_fix<Tag::OrdStatus>(OrdStatus{trade->state(), trade->resd_lots() == 0_lts})
               << put_fix<Tag::Side>(trade->side())
               << put_fix<Tag::OrderQty>(trade->lots())
               << put_fix<Tag::Price>(trade->ticks())
               << put_fix<Tag::LeavesQty>(trade->resd_lots())
               << put_fix<Tag::CumQty>(trade->exec_lots())
               << put_fix<Tag::AvgPx>(to_avg_ticks(trade->exec_lots(), trade->exec_cost()));
            // clang-format off
            if (trade->last_lots() != 0_lts) {
                os << put_fix<Tag::LastPx>(trade->last_ticks())
                   << put_fix<Tag::LastQty>(trade->last_lots());
            }
            os << put_fix<Tag::MinQty>(trade->min_lots());
        };
        conn_->send(now, "8"sv, fn);
    }
}

} // namespace swirly
