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
#include "RestImpl.hxx"

#include <swirly/lob/App.hpp>
#include <swirly/lob/Response.hpp>
#include <swirly/lob/Sess.hpp>

#include <swirly/app/Exception.hpp>

#include <swirly/util/Date.hpp>

#include <algorithm>

namespace swirly {
using namespace std;
namespace {

void do_get_order(const Sess& sess, ostream& out)
{
    const auto& orders = sess.orders();
    out << '[';
    copy(orders.begin(), orders.end(), OStreamJoiner{out, ','});
    out << ']';
}

void do_get_exec(const Sess& sess, Page page, ostream& out)
{
    const auto& execs = sess.execs();
    out << '[';
    const auto size = execs.size();
    if (page.offset < size) {
        auto first = execs.begin();
        advance(first, page.offset);
        decltype(first) last;
        if (page.limit && *page.limit < size - page.offset) {
            last = first;
            advance(last, *page.limit);
        } else {
            last = execs.end();
        }
        transform(first, last,
                  OStreamJoiner{out, ','}, [](const auto& ptr) -> const auto& { return *ptr; });
    }
    out << ']';
}

void do_get_trade(const Sess& sess, ostream& out)
{
    const auto& trades = sess.trades();
    out << '[';
    copy(trades.begin(), trades.end(), OStreamJoiner{out, ','});
    out << ']';
}

void do_get_posn(const Sess& sess, ostream& out)
{
    const auto& posns = sess.posns();
    out << '[';
    copy(posns.begin(), posns.end(), OStreamJoiner{out, ','});
    out << ']';
}

} // namespace

RestImpl::~RestImpl() = default;

void RestImpl::get_ref_data(WallTime now, EntitySet es, ostream& out) const
{
    int i{0};
    out << '{';
    // FIXME: validate entities.
    if (es.asset()) {
        out << "\"assets\":";
        get_asset(now, out);
        ++i;
    }
    if (es.instr()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"instrs\":";
        get_instr(now, out);
        ++i;
    }
    if (es.market()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"markets\":";
        get_market(now, out);
        ++i;
    }
    out << '}';
}

void RestImpl::get_asset(WallTime now, ostream& out) const
{
    const auto& assets = lob_app_.assets();
    out << '[';
    copy(assets.begin(), assets.end(), OStreamJoiner{out, ','});
    out << ']';
}

void RestImpl::get_asset(WallTime now, Symbol symbol, ostream& out) const
{
    const auto& assets = lob_app_.assets();
    auto it = assets.find(symbol);
    if (it == assets.end()) {
        throw NotFoundException{err_msg() << "asset '" << symbol << "' does not exist"};
    }
    out << *it;
}

void RestImpl::get_instr(WallTime now, ostream& out) const
{
    const auto& instrs = lob_app_.instrs();
    out << '[';
    copy(instrs.begin(), instrs.end(), OStreamJoiner{out, ','});
    out << ']';
}

void RestImpl::get_instr(WallTime now, Symbol symbol, ostream& out) const
{
    const auto& instrs = lob_app_.instrs();
    auto it = instrs.find(symbol);
    if (it == instrs.end()) {
        throw NotFoundException{err_msg() << "instr '" << symbol << "' does not exist"};
    }
    out << *it;
}

void RestImpl::get_sess(WallTime now, Symbol accnt, EntitySet es, Page page, ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    int i{0};
    out << '{';
    if (es.market()) {
        out << "\"markets\":";
        get_market(now, out);
        ++i;
    }
    if (es.order()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"orders\":";
        do_get_order(sess, out);
        ++i;
    }
    if (es.exec()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"execs\":";
        do_get_exec(sess, page, out);
        ++i;
    }
    if (es.trade()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"trades\":";
        do_get_trade(sess, out);
        ++i;
    }
    if (es.posn()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"posns\":";
        do_get_posn(sess, out);
        ++i;
    }
    out << '}';
}

void RestImpl::get_market(WallTime now, std::ostream& out) const
{
    const auto& markets = lob_app_.markets();
    out << '[';
    copy(markets.begin(), markets.end(), OStreamJoiner{out, ','});
    out << ']';
}

void RestImpl::get_market(WallTime now, Symbol instr, std::ostream& out) const
{
    const auto& markets = lob_app_.markets();
    out << '[';
    copy_if(markets.begin(), markets.end(), OStreamJoiner{out, ','},
            [instr](const auto& market) { return market.instr() == instr; });
    out << ']';
}

void RestImpl::get_market(WallTime now, Symbol instr, IsoDate settl_date, std::ostream& out) const
{
    const auto id = to_market_id(lob_app_.instr(instr).id(), settl_date);
    out << lob_app_.market(id);
}

void RestImpl::get_order(WallTime now, Symbol accnt, std::ostream& out) const
{
    do_get_order(lob_app_.sess(accnt), out);
}

void RestImpl::get_order(WallTime now, Symbol accnt, Symbol instr, ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& orders = sess.orders();
    out << '[';
    copy_if(orders.begin(), orders.end(), OStreamJoiner{out, ','},
            [instr](const auto& order) { return order.instr() == instr; });
    out << ']';
}

void RestImpl::get_order(WallTime now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                         ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& instr = lob_app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& orders = sess.orders();
    out << '[';
    copy_if(orders.begin(), orders.end(), OStreamJoiner{out, ','},
            [market_id](const auto& order) { return order.market_id() == market_id; });
    out << ']';
}

void RestImpl::get_order(WallTime now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                         Id64 id, ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& instr = lob_app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& orders = sess.orders();
    auto it = orders.find(market_id, id);
    if (it == orders.end()) {
        throw OrderNotFoundException{err_msg() << "order '" << id << "' does not exist"};
    }
    out << *it;
}

void RestImpl::get_exec(WallTime now, Symbol accnt, Page page, std::ostream& out) const
{
    do_get_exec(lob_app_.sess(accnt), page, out);
}

void RestImpl::get_trade(WallTime now, Symbol accnt, std::ostream& out) const
{
    do_get_trade(lob_app_.sess(accnt), out);
}

void RestImpl::get_trade(WallTime now, Symbol accnt, Symbol instr, std::ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& trades = sess.trades();
    out << '[';
    copy_if(trades.begin(), trades.end(), OStreamJoiner{out, ','},
            [instr](const auto& trade) { return trade.instr() == instr; });
    out << ']';
}

void RestImpl::get_trade(WallTime now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                         std::ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& instr = lob_app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& trades = sess.trades();
    out << '[';
    copy_if(trades.begin(), trades.end(), OStreamJoiner{out, ','},
            [market_id](const auto& trade) { return trade.market_id() == market_id; });
    out << ']';
}

void RestImpl::get_trade(WallTime now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                         Id64 id, std::ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& instr = lob_app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& trades = sess.trades();
    auto it = trades.find(market_id, id);
    if (it == trades.end()) {
        throw NotFoundException{err_msg() << "trade '" << id << "' does not exist"};
    }
    out << *it;
}

void RestImpl::get_posn(WallTime now, Symbol accnt, std::ostream& out) const
{
    do_get_posn(lob_app_.sess(accnt), out);
}

void RestImpl::get_posn(WallTime now, Symbol accnt, Symbol instr, std::ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& posns = sess.posns();
    out << '[';
    copy_if(posns.begin(), posns.end(), OStreamJoiner{out, ','},
            [instr](const auto& posn) { return posn.instr() == instr; });
    out << ']';
}

void RestImpl::get_posn(WallTime now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                        std::ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& instr = lob_app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& posns = sess.posns();
    auto it = posns.find(market_id);
    if (it == posns.end()) {
        throw NotFoundException{err_msg() << "posn for '" << instr << "' on " << settl_date
                                          << " does not exist"};
    }
    out << *it;
}

void RestImpl::post_market(WallTime now, Symbol instr_symbol, IsoDate settl_date, MarketState state,
                           std::ostream& out)
{
    const auto& instr = lob_app_.instr(instr_symbol);
    const auto settl_day = maybe_iso_to_jd(settl_date);
    const auto& market = lob_app_.create_market(now, instr, settl_day, state);
    out << market;
}

void RestImpl::put_market(WallTime now, Symbol instr_symbol, IsoDate settl_date, MarketState state,
                          std::ostream& out)
{
    const auto& instr = lob_app_.instr(instr_symbol);
    const auto id = to_market_id(instr.id(), settl_date);
    const auto& market = lob_app_.market(id);
    lob_app_.update_market(now, market, state);
    out << market;
}

void RestImpl::post_order(WallTime now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                          std::string_view ref, Side side, Lots lots, Ticks ticks, Lots min_lots,
                          std::ostream& out)
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& instr = lob_app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& market = lob_app_.market(market_id);
    Response resp;
    lob_app_.create_order(now, sess, market, ref, side, lots, ticks, min_lots, resp);
    out << resp;
}

void RestImpl::put_order(WallTime now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                         ArrayView<Id64> ids, Lots lots, std::ostream& out)
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& instr = lob_app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& market = lob_app_.market(market_id);
    Response resp;
    if (lots > 0_lts) {
        if (ids.size() == 1) {
            lob_app_.revise_order(now, sess, market, ids[0], lots, resp);
        } else {
            lob_app_.revise_order(now, sess, market, ids, lots, resp);
        }
    } else {
        if (ids.size() == 1) {
            lob_app_.cancel_order(now, sess, market, ids[0], resp);
        } else {
            lob_app_.cancel_order(now, sess, market, ids, resp);
        }
    }
    out << resp;
}

void RestImpl::post_trade(WallTime now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                          std::string_view ref, Side side, Lots lots, Ticks ticks, LiqInd liq_ind,
                          Symbol cpty, std::ostream& out)
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& instr = lob_app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& market = lob_app_.market(market_id);
    auto trades = lob_app_.create_trade(now, sess, market, ref, side, lots, ticks, liq_ind, cpty);
    out << '[' << *trades.first;
    if (trades.second) {
        out << ',' << *trades.second;
    }
    out << ']';
}

void RestImpl::delete_trade(WallTime now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                            ArrayView<Id64> ids)
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& instr = lob_app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    lob_app_.archive_trade(now, sess, market_id, ids);
}

} // namespace swirly
