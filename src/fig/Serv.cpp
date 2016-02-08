/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#include <swirly/fig/Serv.hpp>

#include <swirly/fig/Response.hpp>
#include <swirly/fig/TraderSess.hpp>

#include <swirly/elm/DateUtil.hpp>
#include <swirly/elm/Exception.hpp>
#include <swirly/elm/Journ.hpp>
#include <swirly/elm/MarketBook.hpp>
#include <swirly/elm/Model.hpp>

#include <swirly/ash/Finally.hpp>
#include <swirly/ash/JulianDay.hpp>

#include "Match.hpp"
#include "ServFactory.hpp"
#include "TraderSessSet.hpp"

#include <regex>

using namespace std;

namespace swirly {
namespace {
static const regex MNEM_PATTERN{"(\\d{4}[- ]){3}\\d{4}"};
} // anonymous

struct Serv::Impl {

    using MarketBookPtr = unique_ptr<MarketBook, default_delete<Market>>;
    using TraderSessPtr = unique_ptr<TraderSess, default_delete<Trader>>;

    Journ& journ;
    ServFactory factory;
    AssetSet assets;
    ContrSet contrs;
    MarketSet markets;
    TraderSet traders;
    TraderSessSet emailIdx;

    Impl(const Model& model, Journ& journ, Millis now) noexcept : journ{journ}
    {
    }
    MarketBookPtr newMarket(const StringView& mnem, const StringView& display,
                            const StringView& contr, Jday settlDay, Jday expiryDay,
                            MarketState state) const
    {
        if (!regex_match(mnem.begin(), mnem.end(), MNEM_PATTERN))
            throwException<InvalidException>("invalid mnem '%.*s'", SWIRLY_STR(mnem));
        auto up = factory.newMarket(mnem, display, contr, settlDay, expiryDay, state);
        return {static_cast<MarketBook*>(up.release()), std::move(up.get_deleter())};
    }
    TraderSessPtr newTrader(const StringView& mnem, const StringView& display,
                            const StringView& email) const
    {
        if (!regex_match(mnem.begin(), mnem.end(), MNEM_PATTERN))
            throwException<InvalidException>("invalid mnem '%.*s'", SWIRLY_STR(mnem));
        auto up = factory.newTrader(mnem, display, email);
        return {static_cast<TraderSess*>(up.release()), std::move(up.get_deleter())};
    }
    ExecPtr newExec(MarketBook& book, const Order& order, Millis now) const
    {
        return factory.newExec(order, book.allocExecId(), now);
    }
    void matchOrders(const TraderSess& takerSess, const MarketBook& book, const Order& takerOrder,
                     Millis now, vector<Match>& matches) const
    {
        // FIXME: not implemented.
    }
    // Assumes that maker lots have not been reduced since matching took place.
    void commitMatches(const TraderSess& taker, const MarketBook& book,
                       const vector<Match>& matches, const Posn& takerPosn, Millis now,
                       Response& resp)
    {
        // FIXME: not implemented.
    }
};

Serv::Serv(const Model& model, Journ& journ, Millis now)
    : impl_{make_unique<Impl>(model, journ, now)}
{
    impl_->assets = model.readAsset(impl_->factory);
    impl_->contrs = model.readContr(impl_->factory);
    impl_->markets = model.readMarket(impl_->factory);
    impl_->traders = model.readTrader(impl_->factory);

    for (auto& rec : impl_->traders) {
        auto& trader = static_cast<TraderSess&>(rec);
        impl_->emailIdx.insert(trader);
    }
}

Serv::~Serv() noexcept = default;

Serv::Serv(Serv&&) = default;

Serv& Serv::operator=(Serv&&) = default;

const AssetSet& Serv::assets() const noexcept
{
    return impl_->assets;
}

const ContrSet& Serv::contrs() const noexcept
{
    return impl_->contrs;
}

const MarketSet& Serv::markets() const noexcept
{
    return impl_->markets;
}

const TraderSet& Serv::traders() const noexcept
{
    return impl_->traders;
}

const MarketBook& Serv::createMarket(const StringView& mnem, const StringView& display,
                                     const StringView& contr, Jday settlDay, Jday expiryDay,
                                     MarketState state, Millis now)
{
    auto it
        = impl_->markets.insert(impl_->newMarket(mnem, display, contr, settlDay, expiryDay, state));
    const auto& market = static_cast<const MarketBook&>(*it);
    return market;
}

const MarketBook& Serv::updateMarket(const StringView& mnem, const StringView& display,
                                     MarketState state, Millis now)
{
    auto it = impl_->markets.find(mnem);
    if (it == impl_->markets.end())
        throwException<MarketNotFoundException>("market '%.*s' does not exist", SWIRLY_STR(mnem));
    auto& market = static_cast<MarketBook&>(*it);
    market.setDisplay(display);
    market.setState(state);
    return market;
}

const MarketBook& Serv::market(const StringView& mnem) const
{
    auto it = impl_->markets.find(mnem);
    if (it == impl_->markets.end())
        throwException<MarketNotFoundException>("market '%.*s' does not exist", SWIRLY_STR(mnem));
    const auto& market = static_cast<const MarketBook&>(*it);
    return market;
}

const TraderSess& Serv::createTrader(const StringView& mnem, const StringView& display,
                                     const StringView& email)
{
    auto it = impl_->traders.insert(impl_->newTrader(mnem, display, email));
    auto& trader = static_cast<TraderSess&>(*it);
    return trader;
}

const TraderSess& Serv::updateTrader(const StringView& mnem, const StringView& display)
{
    auto it = impl_->traders.find(mnem);
    if (it == impl_->traders.end())
        throwException<TraderNotFoundException>("trader '%.*s' does not exist", SWIRLY_STR(mnem));
    auto& trader = static_cast<TraderSess&>(*it);
    trader.setDisplay(display);
    return trader;
}

const TraderSess& Serv::trader(const StringView& mnem) const
{
    auto it = impl_->traders.find(mnem);
    if (it == impl_->traders.end())
        throwException<TraderNotFoundException>("trader '%.*s' does not exist", SWIRLY_STR(mnem));
    const auto& trader = static_cast<const TraderSess&>(*it);
    return trader;
}

const TraderSess* Serv::findTraderByEmail(const StringView& email) const
{
    const TraderSess* trader{nullptr};
    auto it = impl_->emailIdx.find(email);
    if (it != impl_->emailIdx.end())
        trader = &*it;
    return trader;
}

void Serv::createOrder(TraderSess& sess, MarketBook& book, const StringView& ref, Side side,
                       Lots lots, Ticks ticks, Lots minLots, Millis now, Response& resp)
{
    const auto busDay = getBusDay(now);
    if (book.expiryDay() != 0_jd && book.expiryDay() < busDay) {
        throwException<MarketClosedException>("market for '%.*s' in '%d' has expired",
                                              SWIRLY_STR(book.contr()),
                                              maybeJdToIso(book.settlDay()));
    }
    if (lots == 0_lts || lots < minLots) {
        throwException<InvalidLotsException>("invalid lots '%d'", lots);
    }
    const auto orderId = book.allocOrderId();
    auto order = impl_->factory.newOrder(sess.mnem(), book.mnem(), book.contr(), book.settlDay(),
                                         orderId, ref, side, lots, ticks, minLots, now);
    auto exec = impl_->newExec(book, *order, now);
    resp.reset(book, order, exec);

    vector<Match> matches;
    // Order fields are updated on match.
    impl_->matchOrders(sess, book, *order, now, matches);
    // Place incomplete order in market. N.B. done() is sufficient here because the order cannot be
    // pending cancellation.
    if (!order->done()) {
        // This may fail if level cannot be allocated.
        book.insertOrder(order);
    }
    {
        // TODO: IOC orders would need an additional revision for the unsolicited cancellation of
        // any unfilled quantity.
        bool success{false};
        auto finally = makeFinally([&book, &order, &success]() {
            if (!success && !order->done()) {
                // Undo market insertion.
                book.removeOrder(*order);
            }
        });

        const size_t len{1 + matches.size() * 2};
        Exec* execs[len];

        execs[0] = exec.get();
        int i{1};
        for (const auto& match : matches) {
            execs[i] = match.makerTrade.get();
            execs[i + 1] = match.takerTrade.get();
            i += 2;
        }
        impl_->journ.createExec(book.mnem(), makeArrayView(execs, len));
        success = true;
    }
    // Avoid allocating position when there are no matches.
    PosnPtr posn;
    if (!matches.empty()) {
        // Avoid allocating position when there are no matches.
        // N.B. before commit phase, because this may fail.
        posn = sess.lazyPosn(book.contr(), book.settlDay());
    }

    // Commit phase.

    sess.insertOrder(order);

    // Commit matches.
    if (!matches.empty()) {
        assert(posn);
        impl_->commitMatches(sess, book, matches, *posn, now, resp);
    }
}

void Serv::reviseOrder(TraderSess& sess, MarketBook& book, Iden id, Lots lots, Millis now,
                       Response& resp)
{
}

void Serv::reviseOrder(TraderSess& sess, MarketBook& book, const StringView& ref, Lots lots,
                       Millis now, Response& resp)
{
}

void Serv::reviseOrder(TraderSess& sess, MarketBook& book, const IdenView& ids, Lots lots,
                       Millis now, Response& resp)
{
}

void Serv::cancelOrder(TraderSess& sess, MarketBook& book, Order& order, Millis now, Response& resp)
{
}

void Serv::cancelOrder(TraderSess& sess, MarketBook& book, Iden id, Millis now, Response& resp)
{
}

void Serv::cancelOrder(TraderSess& sess, MarketBook& book, const StringView& ref, Millis now,
                       Response& resp)
{
}

void Serv::cancelOrder(TraderSess& sess, MarketBook& book, const IdenView& ids, Millis now,
                       Response& resp)
{
}

void Serv::cancelOrder(TraderSess& sess, Millis now)
{
}

void Serv::cancelOrder(MarketBook& book, Millis now)
{
}

void Serv::archiveOrder(TraderSess& sess, Order& order, Millis now)
{
}

void Serv::archiveOrder(TraderSess& sess, const StringView& market, Iden id, Millis now)
{
}

void Serv::archiveOrder(TraderSess& sess, Millis now)
{
}

void Serv::archiveOrder(TraderSess& sess, const StringView& market, const IdenView& ids, Millis now)
{
}

ExecPtr Serv::createTrade(TraderSess& sess, MarketBook& book, const StringView& ref, Side side,
                          Lots lots, Ticks ticks, Role role, const StringView& cpty, Millis created)
{
    return {};
}

void Serv::archiveTrade(TraderSess& sess, Exec& trade, Millis now)
{
}

void Serv::archiveTrade(TraderSess& sess, const StringView& market, Iden id, Millis now)
{
}

void Serv::archiveTrade(TraderSess& sess, Millis now)
{
}

void Serv::archiveTrade(TraderSess& sess, const StringView& market, const IdenView& ids, Millis now)
{
}

void Serv::expireEndOfDay(Millis now)
{
}

void Serv::settlEndOfDay(Millis now)
{
}

void Serv::poll(Millis now)
{
}

Millis Serv::getTimeout() const noexcept
{
    return 0_ms;
}

} // swirly
