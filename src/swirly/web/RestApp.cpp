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
#include "RestApp.hpp"

#include <swirly/lob/Response.hpp>
#include <swirly/lob/Sess.hpp>

#include <swirly/fin/Exception.hpp>

#include <swirly/util/Date.hpp>

#include <algorithm>

namespace swirly {
inline namespace web {
using namespace std;
namespace detail {
namespace {

void get_order(const Sess& sess, ostream& out)
{
    const auto& orders = sess.orders();
    out << '[';
    copy(orders.begin(), orders.end(), OStreamJoiner{out, ','});
    out << ']';
}

void get_exec(const Sess& sess, Page page, ostream& out)
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

void get_trade(const Sess& sess, ostream& out)
{
    const auto& trades = sess.trades();
    out << '[';
    copy(trades.begin(), trades.end(), OStreamJoiner{out, ','});
    out << ']';
}

void get_posn(const Sess& sess, ostream& out)
{
    const auto& posns = sess.posns();
    out << '[';
    copy(posns.begin(), posns.end(), OStreamJoiner{out, ','});
    out << ']';
}

} // namespace
} // namespace detail

RestApp::~RestApp() = default;

RestApp::RestApp(RestApp&&) = default;

RestApp& RestApp::operator=(RestApp&&) = default;

void RestApp::get_ref_data(Time now, EntitySet es, ostream& out) const
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

void RestApp::get_asset(Time now, ostream& out) const
{
    const auto& assets = app_.assets();
    out << '[';
    copy(assets.begin(), assets.end(), OStreamJoiner{out, ','});
    out << ']';
}

void RestApp::get_asset(Time now, Symbol symbol, ostream& out) const
{
    const auto& assets = app_.assets();
    auto it = assets.find(symbol);
    if (it == assets.end()) {
        throw NotFoundException{err_msg() << "asset '" << symbol << "' does not exist"};
    }
    out << *it;
}

void RestApp::get_instr(Time now, ostream& out) const
{
    const auto& instrs = app_.instrs();
    out << '[';
    copy(instrs.begin(), instrs.end(), OStreamJoiner{out, ','});
    out << ']';
}

void RestApp::get_instr(Time now, Symbol symbol, ostream& out) const
{
    const auto& instrs = app_.instrs();
    auto it = instrs.find(symbol);
    if (it == instrs.end()) {
        throw NotFoundException{err_msg() << "instr '" << symbol << "' does not exist"};
    }
    out << *it;
}

void RestApp::get_sess(Time now, Symbol accnt, EntitySet es, Page page, ostream& out) const
{
    const auto& sess = app_.sess(accnt);
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
        detail::get_order(sess, out);
        ++i;
    }
    if (es.exec()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"execs\":";
        detail::get_exec(sess, page, out);
        ++i;
    }
    if (es.trade()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"trades\":";
        detail::get_trade(sess, out);
        ++i;
    }
    if (es.posn()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"posns\":";
        detail::get_posn(sess, out);
        ++i;
    }
    out << '}';
}

void RestApp::get_market(Time now, std::ostream& out) const
{
    const auto& markets = app_.markets();
    out << '[';
    copy(markets.begin(), markets.end(), OStreamJoiner{out, ','});
    out << ']';
}

void RestApp::get_market(Time now, Symbol instr, std::ostream& out) const
{
    const auto& markets = app_.markets();
    out << '[';
    copy_if(markets.begin(), markets.end(), OStreamJoiner{out, ','},
            [instr](const auto& market) { return market.instr() == instr; });
    out << ']';
}

void RestApp::get_market(Time now, Symbol instr, IsoDate settl_date, std::ostream& out) const
{
    const auto id = to_market_id(app_.instr(instr).id(), settl_date);
    out << app_.market(id);
}

void RestApp::get_order(Time now, Symbol accnt, std::ostream& out) const
{
    detail::get_order(app_.sess(accnt), out);
}

void RestApp::get_order(Time now, Symbol accnt, Symbol instr, ostream& out) const
{
    const auto& sess = app_.sess(accnt);
    const auto& orders = sess.orders();
    out << '[';
    copy_if(orders.begin(), orders.end(), OStreamJoiner{out, ','},
            [instr](const auto& order) { return order.instr() == instr; });
    out << ']';
}

void RestApp::get_order(Time now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                        ostream& out) const
{
    const auto& sess = app_.sess(accnt);
    const auto& instr = app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& orders = sess.orders();
    out << '[';
    copy_if(orders.begin(), orders.end(), OStreamJoiner{out, ','},
            [market_id](const auto& order) { return order.market_id() == market_id; });
    out << ']';
}

void RestApp::get_order(Time now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date, Id64 id,
                        ostream& out) const
{
    const auto& sess = app_.sess(accnt);
    const auto& instr = app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& orders = sess.orders();
    auto it = orders.find(market_id, id);
    if (it == orders.end()) {
        throw OrderNotFoundException{err_msg() << "order '" << id << "' does not exist"};
    }
    out << *it;
}

void RestApp::get_exec(Time now, Symbol accnt, Page page, std::ostream& out) const
{
    detail::get_exec(app_.sess(accnt), page, out);
}

void RestApp::get_trade(Time now, Symbol accnt, std::ostream& out) const
{
    detail::get_trade(app_.sess(accnt), out);
}

void RestApp::get_trade(Time now, Symbol accnt, Symbol instr, std::ostream& out) const
{
    const auto& sess = app_.sess(accnt);
    const auto& trades = sess.trades();
    out << '[';
    copy_if(trades.begin(), trades.end(), OStreamJoiner{out, ','},
            [instr](const auto& trade) { return trade.instr() == instr; });
    out << ']';
}

void RestApp::get_trade(Time now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                        std::ostream& out) const
{
    const auto& sess = app_.sess(accnt);
    const auto& instr = app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& trades = sess.trades();
    out << '[';
    copy_if(trades.begin(), trades.end(), OStreamJoiner{out, ','},
            [market_id](const auto& trade) { return trade.market_id() == market_id; });
    out << ']';
}

void RestApp::get_trade(Time now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date, Id64 id,
                        std::ostream& out) const
{
    const auto& sess = app_.sess(accnt);
    const auto& instr = app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& trades = sess.trades();
    auto it = trades.find(market_id, id);
    if (it == trades.end()) {
        throw NotFoundException{err_msg() << "trade '" << id << "' does not exist"};
    }
    out << *it;
}

void RestApp::get_posn(Time now, Symbol accnt, std::ostream& out) const
{
    detail::get_posn(app_.sess(accnt), out);
}

void RestApp::get_posn(Time now, Symbol accnt, Symbol instr, std::ostream& out) const
{
    const auto& sess = app_.sess(accnt);
    const auto& posns = sess.posns();
    out << '[';
    copy_if(posns.begin(), posns.end(), OStreamJoiner{out, ','},
            [instr](const auto& posn) { return posn.instr() == instr; });
    out << ']';
}

void RestApp::get_posn(Time now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                       std::ostream& out) const
{
    const auto& sess = app_.sess(accnt);
    const auto& instr = app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& posns = sess.posns();
    auto it = posns.find(market_id);
    if (it == posns.end()) {
        throw NotFoundException{err_msg() << "posn for '" << instr << "' on " << settl_date
                                          << " does not exist"};
    }
    out << *it;
}

void RestApp::post_market(Time now, Symbol instr_symbol, IsoDate settl_date, MarketState state,
                          std::ostream& out)
{
    const auto& instr = app_.instr(instr_symbol);
    const auto settl_day = maybe_iso_to_jd(settl_date);
    const auto& market = app_.create_market(now, instr, settl_day, state);
    out << market;
}

void RestApp::put_market(Time now, Symbol instr_symbol, IsoDate settl_date, MarketState state,
                         std::ostream& out)
{
    const auto& instr = app_.instr(instr_symbol);
    const auto id = to_market_id(instr.id(), settl_date);
    const auto& market = app_.market(id);
    app_.update_market(now, market, state);
    out << market;
}

void RestApp::post_order(Time now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                         std::string_view ref, Side side, Lots lots, Ticks ticks, Lots min_lots,
                         std::ostream& out)
{
    const auto& sess = app_.sess(accnt);
    const auto& instr = app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& market = app_.market(market_id);
    Response resp;
    app_.create_order(now, sess, market, ref, side, lots, ticks, min_lots, resp);
    out << resp;
}

void RestApp::put_order(Time now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                        ArrayView<Id64> ids, Lots lots, std::ostream& out)
{
    const auto& sess = app_.sess(accnt);
    const auto& instr = app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& market = app_.market(market_id);
    Response resp;
    if (lots > 0_lts) {
        if (ids.size() == 1) {
            app_.revise_order(now, sess, market, ids[0], lots, resp);
        } else {
            app_.revise_order(now, sess, market, ids, lots, resp);
        }
    } else {
        if (ids.size() == 1) {
            app_.cancel_order(now, sess, market, ids[0], resp);
        } else {
            app_.cancel_order(now, sess, market, ids, resp);
        }
    }
    out << resp;
}

void RestApp::post_trade(Time now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                         std::string_view ref, Side side, Lots lots, Ticks ticks, LiqInd liq_ind,
                         Symbol cpty, std::ostream& out)
{
    const auto& sess = app_.sess(accnt);
    const auto& instr = app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& market = app_.market(market_id);
    auto trades = app_.create_trade(now, sess, market, ref, side, lots, ticks, liq_ind, cpty);
    out << '[' << *trades.first;
    if (trades.second) {
        out << ',' << *trades.second;
    }
    out << ']';
}

void RestApp::delete_trade(Time now, Symbol accnt, Symbol instr_symbol, IsoDate settl_date,
                           ArrayView<Id64> ids)
{
    const auto& sess = app_.sess(accnt);
    const auto& instr = app_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    app_.archive_trade(now, sess, market_id, ids);
}

} // namespace web
} // namespace swirly
