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

#include <swirly/lob/Accnt.hpp>
#include <swirly/lob/Response.hpp>

#include <swirly/fin/Exception.hpp>

#include <swirly/util/Date.hpp>

#include <algorithm>

namespace swirly {
inline namespace web {
using namespace std;
namespace detail {
namespace {

void get_order(const Accnt& accnt, ostream& out)
{
    const auto& orders = accnt.orders();
    out << '[';
    copy(orders.begin(), orders.end(), OStreamJoiner{out, ','});
    out << ']';
}

void get_exec(const Accnt& accnt, Page page, ostream& out)
{
    const auto& execs = accnt.execs();
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

void get_trade(const Accnt& accnt, ostream& out)
{
    const auto& trades = accnt.trades();
    out << '[';
    copy(trades.begin(), trades.end(), OStreamJoiner{out, ','});
    out << ']';
}

void get_posn(const Accnt& accnt, ostream& out)
{
    const auto& posns = accnt.posns();
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
        out << "\"assets\":"sv;
        get_asset(now, out);
        ++i;
    }
    if (es.instr()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"instrs\":"sv;
        get_instr(now, out);
        ++i;
    }
    if (es.market()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"markets\":"sv;
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
        throw NotFoundException{err_msg() << "asset '"sv << symbol << "' does not exist"sv};
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
        throw NotFoundException{err_msg() << "instr '"sv << symbol << "' does not exist"sv};
    }
    out << *it;
}

void Rest::get_accnt(Symbol symbol, EntitySet es, Page page, Time now, ostream& out) const
{
    const auto& accnt = serv_.accnt(symbol);
    int i{0};
    out << '{';
    if (es.market()) {
        out << "\"markets\":"sv;
        get_market(now, out);
        ++i;
    }
    if (es.order()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"orders\":"sv;
        detail::get_order(accnt, out);
        ++i;
    }
    if (es.exec()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"execs\":"sv;
        detail::get_exec(accnt, page, out);
        ++i;
    }
    if (es.trade()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"trades\":"sv;
        detail::get_trade(accnt, out);
        ++i;
    }
    if (es.posn()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"posns\":"sv;
        detail::get_posn(accnt, out);
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

void Rest::get_market(Symbol instr_symbol, Time now, std::ostream& out) const
{
    const auto& markets = serv_.markets();
    out << '[';
    copy_if(markets.begin(), markets.end(), OStreamJoiner{out, ','},
            [instr_symbol](const auto& market) { return market.instr() == instr_symbol; });
    out << ']';
}

void Rest::get_market(Symbol instr_symbol, IsoDate settl_date, Time now, std::ostream& out) const
{
    const auto id = to_market_id(serv_.instr(instr_symbol).id(), settl_date);
    out << serv_.market(id);
}

void Rest::get_order(Symbol accnt_symbol, Time now, ostream& out) const
{
    detail::get_order(serv_.accnt(accnt_symbol), out);
}

void Rest::get_order(Symbol accnt_symbol, Symbol instr_symbol, Time now, ostream& out) const
{
    const auto& accnt = serv_.accnt(accnt_symbol);
    const auto& orders = accnt.orders();
    out << '[';
    copy_if(orders.begin(), orders.end(), OStreamJoiner{out, ','},
            [instr_symbol](const auto& order) { return order.instr() == instr_symbol; });
    out << ']';
}

void Rest::get_order(Symbol accnt_symbol, Symbol instr_symbol, IsoDate settl_date, Time now,
                     ostream& out) const
{
    const auto& accnt = serv_.accnt(accnt_symbol);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& orders = accnt.orders();
    out << '[';
    copy_if(orders.begin(), orders.end(), OStreamJoiner{out, ','},
            [market_id](const auto& order) { return order.market_id() == market_id; });
    out << ']';
}

void Rest::get_order(Symbol accnt_symbol, Symbol instr_symbol, IsoDate settl_date, Id64 id,
                     Time now, ostream& out) const
{
    const auto& accnt = serv_.accnt(accnt_symbol);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& orders = accnt.orders();
    auto it = orders.find(market_id, id);
    if (it == orders.end()) {
        throw OrderNotFoundException{err_msg() << "order '"sv << id << "' does not exist"sv};
    }
    out << *it;
}

void Rest::get_exec(Symbol accnt_symbol, Page page, Time now, ostream& out) const
{
    detail::get_exec(serv_.accnt(accnt_symbol), page, out);
}

void Rest::get_trade(Symbol accnt_symbol, Time now, ostream& out) const
{
    detail::get_trade(serv_.accnt(accnt_symbol), out);
}

void Rest::get_trade(Symbol accnt_symbol, Symbol instr_symbol, Time now, std::ostream& out) const
{
    const auto& accnt = serv_.accnt(accnt_symbol);
    const auto& trades = accnt.trades();
    out << '[';
    copy_if(trades.begin(), trades.end(), OStreamJoiner{out, ','},
            [instr_symbol](const auto& trade) { return trade.instr() == instr_symbol; });
    out << ']';
}

void Rest::get_trade(Symbol accnt_symbol, Symbol instr_symbol, IsoDate settl_date, Time now,
                     ostream& out) const
{
    const auto& accnt = serv_.accnt(accnt_symbol);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& trades = accnt.trades();
    out << '[';
    copy_if(trades.begin(), trades.end(), OStreamJoiner{out, ','},
            [market_id](const auto& trade) { return trade.market_id() == market_id; });
    out << ']';
}

void Rest::get_trade(Symbol accnt_symbol, Symbol instr_symbol, IsoDate settl_date, Id64 id,
                     Time now, ostream& out) const
{
    const auto& accnt = serv_.accnt(accnt_symbol);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& trades = accnt.trades();
    auto it = trades.find(market_id, id);
    if (it == trades.end()) {
        throw NotFoundException{err_msg() << "trade '"sv << id << "' does not exist"sv};
    }
    out << *it;
}

void Rest::get_posn(Symbol accnt_symbol, Time now, ostream& out) const
{
    detail::get_posn(serv_.accnt(accnt_symbol), out);
}

void Rest::get_posn(Symbol accnt_symbol, Symbol instr_symbol, Time now, ostream& out) const
{
    const auto& accnt = serv_.accnt(accnt_symbol);
    const auto& posns = accnt.posns();
    out << '[';
    copy_if(posns.begin(), posns.end(), OStreamJoiner{out, ','},
            [instr_symbol](const auto& posn) { return posn.instr() == instr_symbol; });
    out << ']';
}

void Rest::get_posn(Symbol accnt_symbol, Symbol instr_symbol, IsoDate settl_date, Time now,
                    ostream& out) const
{
    const auto& accnt = serv_.accnt(accnt_symbol);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& posns = accnt.posns();
    auto it = posns.find(market_id);
    if (it == posns.end()) {
        throw NotFoundException{err_msg() << "posn for '"sv << instr_symbol << "' on "sv
                                          << settl_date << " does not exist"sv};
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

void Rest::post_order(Symbol accnt_symbol, Symbol instr_symbol, IsoDate settl_date, string_view ref,
                      Side side, Lots lots, Ticks ticks, Lots min_lots, Time now, ostream& out)
{
    const auto& accnt = serv_.accnt(accnt_symbol);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& market = serv_.market(market_id);
    Response resp;
    serv_.create_order(accnt, market, ref, side, lots, ticks, min_lots, now, resp);
    out << resp;
}

void Rest::put_order(Symbol accnt_symbol, Symbol instr_symbol, IsoDate settl_date,
                     ArrayView<Id64> ids, Lots lots, Time now, ostream& out)
{
    const auto& accnt = serv_.accnt(accnt_symbol);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& market = serv_.market(market_id);
    Response resp;
    if (lots > 0_lts) {
        if (ids.size() == 1) {
            serv_.revise_order(accnt, market, ids[0], lots, now, resp);
        } else {
            serv_.revise_order(accnt, market, ids, lots, now, resp);
        }
    } else {
        if (ids.size() == 1) {
            serv_.cancel_order(accnt, market, ids[0], now, resp);
        } else {
            serv_.cancel_order(accnt, market, ids, now, resp);
        }
    }
    out << resp;
}

void Rest::post_trade(Symbol accnt_symbol, Symbol instr_symbol, IsoDate settl_date, string_view ref,
                      Side side, Lots lots, Ticks ticks, LiqInd liq_ind, Symbol cpty, Time now,
                      ostream& out)
{
    const auto& accnt = serv_.accnt(accnt_symbol);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    const auto& market = serv_.market(market_id);
    auto trades = serv_.create_trade(accnt, market, ref, side, lots, ticks, liq_ind, cpty, now);
    out << '[' << *trades.first;
    if (trades.second) {
        out << ',' << *trades.second;
    }
    out << ']';
}

void Rest::delete_trade(Symbol accnt_symbol, Symbol instr_symbol, IsoDate settl_date,
                        ArrayView<Id64> ids, Time now)
{
    const auto& accnt = serv_.accnt(accnt_symbol);
    const auto& instr = serv_.instr(instr_symbol);
    const auto market_id = to_market_id(instr.id(), settl_date);
    serv_.archive_trade(accnt, market_id, ids, now);
}

} // namespace web
} // namespace swirly
