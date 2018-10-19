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
        transform(first, last, OStreamJoiner{out, ','},
                  [](const auto& ptr) -> const auto& { return *ptr; });
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

void RestImpl::get_ref_data(CyclTime now, EntitySet es, ostream& out) const
{
    int i{0};
    out << '{';
    // FIXME: validate entities.
    if (es.asset()) {
        out << "\"assets\":";
        get_asset(now, out);
        ++i;
    }
    if (es.product()) {
        if (i > 0) {
            out << ',';
        }
        out << "\"products\":";
        get_product(now, out);
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

void RestImpl::get_asset(CyclTime now, ostream& out) const
{
    const auto& assets = lob_app_.assets();
    out << '[';
    copy(assets.begin(), assets.end(), OStreamJoiner{out, ','});
    out << ']';
}

void RestImpl::get_asset(CyclTime now, Symbol symbol, ostream& out) const
{
    const auto& assets = lob_app_.assets();
    auto it = assets.find(symbol);
    if (it == assets.end()) {
        throw NotFoundException{err_msg() << "asset '" << symbol << "' does not exist"};
    }
    out << *it;
}

void RestImpl::get_product(CyclTime now, ostream& out) const
{
    const auto& products = lob_app_.products();
    out << '[';
    copy(products.begin(), products.end(), OStreamJoiner{out, ','});
    out << ']';
}

void RestImpl::get_product(CyclTime now, Symbol symbol, ostream& out) const
{
    const auto& products = lob_app_.products();
    auto it = products.find(symbol);
    if (it == products.end()) {
        throw NotFoundException{err_msg() << "product '" << symbol << "' does not exist"};
    }
    out << *it;
}

void RestImpl::get_sess(CyclTime now, Symbol accnt, EntitySet es, Page page, ostream& out) const
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

void RestImpl::get_market(CyclTime now, std::ostream& out) const
{
    const auto& markets = lob_app_.markets();
    out << '[';
    copy(markets.begin(), markets.end(), OStreamJoiner{out, ','});
    out << ']';
}

void RestImpl::get_market(CyclTime now, Symbol product, std::ostream& out) const
{
    const auto& markets = lob_app_.markets();
    out << '[';
    copy_if(markets.begin(), markets.end(), OStreamJoiner{out, ','},
            [product](const auto& market) { return market.product() == product; });
    out << ']';
}

void RestImpl::get_market(CyclTime now, Symbol product, IsoDate settl_date, std::ostream& out) const
{
    const auto id = to_market_id(lob_app_.product(product).id(), settl_date);
    out << lob_app_.market(id);
}

void RestImpl::get_order(CyclTime now, Symbol accnt, std::ostream& out) const
{
    do_get_order(lob_app_.sess(accnt), out);
}

void RestImpl::get_order(CyclTime now, Symbol accnt, Symbol product, ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& orders = sess.orders();
    out << '[';
    copy_if(orders.begin(), orders.end(), OStreamJoiner{out, ','},
            [product](const auto& order) { return order.product() == product; });
    out << ']';
}

void RestImpl::get_order(CyclTime now, Symbol accnt, Symbol product_symbol, IsoDate settl_date,
                         ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& product = lob_app_.product(product_symbol);
    const auto market_id = to_market_id(product.id(), settl_date);
    const auto& orders = sess.orders();
    out << '[';
    copy_if(orders.begin(), orders.end(), OStreamJoiner{out, ','},
            [market_id](const auto& order) { return order.market_id() == market_id; });
    out << ']';
}

void RestImpl::get_order(CyclTime now, Symbol accnt, Symbol product_symbol, IsoDate settl_date,
                         Id64 id, ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& product = lob_app_.product(product_symbol);
    const auto market_id = to_market_id(product.id(), settl_date);
    const auto& orders = sess.orders();
    auto it = orders.find(market_id, id);
    if (it == orders.end()) {
        throw OrderNotFoundException{err_msg() << "order '" << id << "' does not exist"};
    }
    out << *it;
}

void RestImpl::get_exec(CyclTime now, Symbol accnt, Page page, std::ostream& out) const
{
    do_get_exec(lob_app_.sess(accnt), page, out);
}

void RestImpl::get_trade(CyclTime now, Symbol accnt, std::ostream& out) const
{
    do_get_trade(lob_app_.sess(accnt), out);
}

void RestImpl::get_trade(CyclTime now, Symbol accnt, Symbol product, std::ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& trades = sess.trades();
    out << '[';
    copy_if(trades.begin(), trades.end(), OStreamJoiner{out, ','},
            [product](const auto& trade) { return trade.product() == product; });
    out << ']';
}

void RestImpl::get_trade(CyclTime now, Symbol accnt, Symbol product_symbol, IsoDate settl_date,
                         std::ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& product = lob_app_.product(product_symbol);
    const auto market_id = to_market_id(product.id(), settl_date);
    const auto& trades = sess.trades();
    out << '[';
    copy_if(trades.begin(), trades.end(), OStreamJoiner{out, ','},
            [market_id](const auto& trade) { return trade.market_id() == market_id; });
    out << ']';
}

void RestImpl::get_trade(CyclTime now, Symbol accnt, Symbol product_symbol, IsoDate settl_date,
                         Id64 id, std::ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& product = lob_app_.product(product_symbol);
    const auto market_id = to_market_id(product.id(), settl_date);
    const auto& trades = sess.trades();
    auto it = trades.find(market_id, id);
    if (it == trades.end()) {
        throw NotFoundException{err_msg() << "trade '" << id << "' does not exist"};
    }
    out << *it;
}

void RestImpl::get_posn(CyclTime now, Symbol accnt, std::ostream& out) const
{
    do_get_posn(lob_app_.sess(accnt), out);
}

void RestImpl::get_posn(CyclTime now, Symbol accnt, Symbol product, std::ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& posns = sess.posns();
    out << '[';
    copy_if(posns.begin(), posns.end(), OStreamJoiner{out, ','},
            [product](const auto& posn) { return posn.product() == product; });
    out << ']';
}

void RestImpl::get_posn(CyclTime now, Symbol accnt, Symbol product_symbol, IsoDate settl_date,
                        std::ostream& out) const
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& product = lob_app_.product(product_symbol);
    const auto market_id = to_market_id(product.id(), settl_date);
    const auto& posns = sess.posns();
    auto it = posns.find(market_id);
    if (it == posns.end()) {
        throw NotFoundException{err_msg() << "posn for '" << product << "' on " << settl_date
                                          << " does not exist"};
    }
    out << *it;
}

void RestImpl::post_market(CyclTime now, Symbol product_symbol, IsoDate settl_date,
                           MarketState state, std::ostream& out)
{
    const auto& product = lob_app_.product(product_symbol);
    const auto settl_day = maybe_iso_to_jd(settl_date);
    const auto& market = lob_app_.create_market(now, product, settl_day, state);
    out << market;
}

void RestImpl::put_market(CyclTime now, Symbol product_symbol, IsoDate settl_date,
                          MarketState state, std::ostream& out)
{
    const auto& product = lob_app_.product(product_symbol);
    const auto id = to_market_id(product.id(), settl_date);
    const auto& market = lob_app_.market(id);
    lob_app_.update_market(now, market, state);
    out << market;
}

void RestImpl::post_order(CyclTime now, Symbol accnt, Symbol product_symbol, IsoDate settl_date,
                          std::string_view ref, Side side, Lots lots, Ticks ticks, Lots min_lots,
                          std::ostream& out)
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& product = lob_app_.product(product_symbol);
    const auto market_id = to_market_id(product.id(), settl_date);
    const auto& market = lob_app_.market(market_id);
    Response resp;
    lob_app_.create_order(now, sess, market, ref, side, lots, ticks, min_lots, resp);
    out << resp;
}

void RestImpl::put_order(CyclTime now, Symbol accnt, Symbol product_symbol, IsoDate settl_date,
                         ArrayView<Id64> ids, Lots lots, std::ostream& out)
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& product = lob_app_.product(product_symbol);
    const auto market_id = to_market_id(product.id(), settl_date);
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

void RestImpl::post_trade(CyclTime now, Symbol accnt, Symbol product_symbol, IsoDate settl_date,
                          std::string_view ref, Side side, Lots lots, Ticks ticks, LiqInd liq_ind,
                          Symbol cpty, std::ostream& out)
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& product = lob_app_.product(product_symbol);
    const auto market_id = to_market_id(product.id(), settl_date);
    const auto& market = lob_app_.market(market_id);
    auto trades = lob_app_.create_trade(now, sess, market, ref, side, lots, ticks, liq_ind, cpty);
    out << '[' << *trades.first;
    if (trades.second) {
        out << ',' << *trades.second;
    }
    out << ']';
}

void RestImpl::delete_trade(CyclTime now, Symbol accnt, Symbol product_symbol, IsoDate settl_date,
                            ArrayView<Id64> ids)
{
    const auto& sess = lob_app_.sess(accnt);
    const auto& product = lob_app_.product(product_symbol);
    const auto market_id = to_market_id(product.id(), settl_date);
    lob_app_.archive_trade(now, sess, market_id, ids);
}

} // namespace swirly
