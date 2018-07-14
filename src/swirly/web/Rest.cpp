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
#include "Rest.hpp"

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

Rest::~Rest() = default;

Rest::Rest(Rest&&) = default;

Rest& Rest::operator=(Rest&&) = default;

void Rest::get_ref_data(EntitySet es, Time now, ostream& out) const
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

void Rest::get_asset(Time now, ostream& out) const
{
    const auto& assets = serv_.assets();
    out << '[';
    copy(assets.begin(), assets.end(), OStreamJoiner{out, ','});
    out << ']';
}

void Rest::get_asset(Symbol symbol, Time now, ostream& out) const
{
    const auto& assets = serv_.assets();
    auto it = assets.find(symbol);
    if (it == assets.end()) {
        throw NotFoundException{err_msg() << "asset '" << symbol << "' does not exist"};
    }
    out << *it;
}

void Rest::get_instr(Time now, ostream& out) const
{
    const auto& instrs = serv_.instrs();
    out << '[';
    copy(instrs.begin(), instrs.end(), OStreamJoiner{out, ','});
    out << ']';
}

void Rest::get_instr(Symbol symbol, Time now, ostream& out) const
{
    const auto& instrs = serv_.instrs();
    auto it = instrs.find(symbol);
    if (it == instrs.end()) {
        throw NotFoundException{err_msg() << "instr '" << symbol << "' does not exist"};
    }
    out << *it;
}

void Rest::get_sess(Symbol accnt, EntitySet es, Page page, Time now, ostream& out) const
{
    const auto& sess = serv_.sess(accnt);
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

void Rest::get_market(Time now, std::ostream& out) const
{
    const auto& markets = serv_.markets();
    out << '[';
    copy(markets.begin(), markets.end(), OStreamJoiner{out, ','});
    out << ']';
}

void Rest::get_market(Symbol instr, Time now, std::ostream& out) const
{
    const auto& markets = serv_.markets();
    out << '[';
    copy_if(markets.begin(), markets.end(), OStreamJoiner{out, ','},
            [instr](const auto& market) { return market.instr() == instr; });
    out << ']';
}

void Rest::get_market(Symbol instr, IsoDate settl_date, Time now, std::ostream& out) const
{
    const auto id = to_market_id(serv_.instr(instr).id(), settl_date);
    out << serv_.market(id);
}

void Rest::get_order(Symbol accnt, Time now, ostream& out) const
{
    detail::get_order(serv_.sess(accnt), out);
}

void Rest::get_order(Symbol accnt, Symbol instr, Time now, ostream& out) const
{
    const auto& sess = serv_.sess(accnt);
    const auto& orders = sess.orders();
    out << '[';
    copy_if(orders.begin(), orders.end(), OStreamJoiner{out, ','},
            [instr](const auto& order) { return order.instr() == instr; });
    out << ']';
}

void Rest::get_order(Symbol accnt, Symbol instr_symbol, IsoDate settl_date, Time now,
                     ostream& out) const
{
    const auto& sess = serv_.sess(accnt);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& orders = sess.orders();
    out << '[';
    copy_if(orders.begin(), orders.end(), OStreamJoiner{out, ','},
            [market_id](const auto& order) { return order.market_id() == market_id; });
    out << ']';
}

void Rest::get_order(Symbol accnt, Symbol instr_symbol, IsoDate settl_date, Id64 id, Time now,
                     ostream& out) const
{
    const auto& sess = serv_.sess(accnt);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& orders = sess.orders();
    auto it = orders.find(market_id, id);
    if (it == orders.end()) {
        throw OrderNotFoundException{err_msg() << "order '" << id << "' does not exist"};
    }
    out << *it;
}

void Rest::get_exec(Symbol accnt, Page page, Time now, ostream& out) const
{
    detail::get_exec(serv_.sess(accnt), page, out);
}

void Rest::get_trade(Symbol accnt, Time now, ostream& out) const
{
    detail::get_trade(serv_.sess(accnt), out);
}

void Rest::get_trade(Symbol accnt, Symbol instr, Time now, std::ostream& out) const
{
    const auto& sess = serv_.sess(accnt);
    const auto& trades = sess.trades();
    out << '[';
    copy_if(trades.begin(), trades.end(), OStreamJoiner{out, ','},
            [instr](const auto& trade) { return trade.instr() == instr; });
    out << ']';
}

void Rest::get_trade(Symbol accnt, Symbol instr_symbol, IsoDate settl_date, Time now,
                     ostream& out) const
{
    const auto& sess = serv_.sess(accnt);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& trades = sess.trades();
    out << '[';
    copy_if(trades.begin(), trades.end(), OStreamJoiner{out, ','},
            [market_id](const auto& trade) { return trade.market_id() == market_id; });
    out << ']';
}

void Rest::get_trade(Symbol accnt, Symbol instr_symbol, IsoDate settl_date, Id64 id, Time now,
                     ostream& out) const
{
    const auto& sess = serv_.sess(accnt);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& trades = sess.trades();
    auto it = trades.find(market_id, id);
    if (it == trades.end()) {
        throw NotFoundException{err_msg() << "trade '" << id << "' does not exist"};
    }
    out << *it;
}

void Rest::get_posn(Symbol accnt, Time now, ostream& out) const
{
    detail::get_posn(serv_.sess(accnt), out);
}

void Rest::get_posn(Symbol accnt, Symbol instr, Time now, ostream& out) const
{
    const auto& sess = serv_.sess(accnt);
    const auto& posns = sess.posns();
    out << '[';
    copy_if(posns.begin(), posns.end(), OStreamJoiner{out, ','},
            [instr](const auto& posn) { return posn.instr() == instr; });
    out << ']';
}

void Rest::get_posn(Symbol accnt, Symbol instr_symbol, IsoDate settl_date, Time now,
                    ostream& out) const
{
    const auto& sess = serv_.sess(accnt);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& posns = sess.posns();
    auto it = posns.find(market_id);
    if (it == posns.end()) {
        throw NotFoundException{err_msg() << "posn for '" << instr << "' on " << settl_date
                                          << " does not exist"};
    }
    out << *it;
}

void Rest::post_market(Symbol instr_symbol, IsoDate settl_date, MarketState state, Time now,
                       ostream& out)
{
    const auto& instr = serv_.instr(instr_symbol);
    const auto settl_day = maybe_iso_to_jd(settl_date);
    const auto& market = serv_.create_market(instr, settl_day, state, now);
    out << market;
}

void Rest::put_market(Symbol instr_symbol, IsoDate settl_date, MarketState state, Time now,
                      ostream& out)
{
    const auto& instr = serv_.instr(instr_symbol);
    const auto id = to_market_id(instr.id(), settl_date);
    const auto& market = serv_.market(id);
    serv_.update_market(market, state, now);
    out << market;
}

void Rest::post_order(Symbol accnt, Symbol instr_symbol, IsoDate settl_date, string_view ref,
                      Side side, Lots lots, Ticks ticks, Lots min_lots, Time now, ostream& out)
{
    const auto& sess = serv_.sess(accnt);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& market = serv_.market(market_id);
    Response resp;
    serv_.create_order(sess, market, ref, side, lots, ticks, min_lots, now, resp);
    out << resp;
}

void Rest::put_order(Symbol accnt, Symbol instr_symbol, IsoDate settl_date, ArrayView<Id64> ids,
                     Lots lots, Time now, ostream& out)
{
    const auto& sess = serv_.sess(accnt);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& market = serv_.market(market_id);
    Response resp;
    if (lots > 0_lts) {
        if (ids.size() == 1) {
            serv_.revise_order(sess, market, ids[0], lots, now, resp);
        } else {
            serv_.revise_order(sess, market, ids, lots, now, resp);
        }
    } else {
        if (ids.size() == 1) {
            serv_.cancel_order(sess, market, ids[0], now, resp);
        } else {
            serv_.cancel_order(sess, market, ids, now, resp);
        }
    }
    out << resp;
}

void Rest::post_trade(Symbol accnt, Symbol instr_symbol, IsoDate settl_date, string_view ref,
                      Side side, Lots lots, Ticks ticks, LiqInd liq_ind, Symbol cpty, Time now,
                      ostream& out)
{
    const auto& sess = serv_.sess(accnt);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& market = serv_.market(market_id);
    auto trades = serv_.create_trade(sess, market, ref, side, lots, ticks, liq_ind, cpty, now);
    out << '[' << *trades.first;
    if (trades.second) {
        out << ',' << *trades.second;
    }
    out << ']';
}

void Rest::delete_trade(Symbol accnt, Symbol instr_symbol, IsoDate settl_date, ArrayView<Id64> ids,
                        Time now)
{
    const auto& sess = serv_.sess(accnt);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    serv_.archive_trade(sess, market_id, ids, now);
}

} // namespace web
} // namespace swirly
