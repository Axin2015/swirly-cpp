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
#include <swirly/elm/Factory.hpp>

#include <swirly/elm/Asset.hpp>
#include <swirly/elm/Contr.hpp>
#include <swirly/elm/Exec.hpp>
#include <swirly/elm/Market.hpp>
#include <swirly/elm/Order.hpp>
#include <swirly/elm/Posn.hpp>
#include <swirly/elm/Trader.hpp>

using namespace std;

namespace swirly {

Factory::~Factory() noexcept = default;

std::unique_ptr<Asset> Factory::newAsset(const StringView& mnem, const StringView& display,
                                         AssetType type) const
{
    return doNewAsset(mnem, display, type);
}

std::unique_ptr<Contr> Factory::newContr(const StringView& mnem, const StringView& display,
                                         const StringView& asset, const StringView& ccy,
                                         int lotNumer, int lotDenom, int tickNumer, int tickDenom,
                                         int pipDp, Lots minLots, Lots maxLots) const
{
    return doNewContr(mnem, display, asset, ccy, lotNumer, lotDenom, tickNumer, tickDenom, pipDp,
                      minLots, maxLots);
}

std::unique_ptr<Market> Factory::newMarket(const StringView& mnem, const StringView& display,
                                           const StringView& contr, Jday settlDay, Jday expiryDay,
                                           MarketState state, Lots lastLots, Ticks lastTicks,
                                           Millis lastTime, Iden maxOrderId, Iden maxExecId) const
{
    return doNewMarket(mnem, display, contr, settlDay, expiryDay, state, lastLots, lastTicks,
                       lastTime, maxOrderId, maxExecId);
}

std::unique_ptr<Market> Factory::newMarket(const StringView& mnem, const StringView& display,
                                           const StringView& contr, Jday settlDay, Jday expiryDay,
                                           MarketState state) const
{
    return doNewMarket(mnem, display, contr, settlDay, expiryDay, state, 0_lts, 0_tks, 0_ms, 0_id,
                       0_id);
}

std::unique_ptr<Trader> Factory::newTrader(const StringView& mnem, const StringView& display,
                                           const StringView& email) const
{
    return doNewTrader(mnem, display, email);
}

OrderPtr Factory::newOrder(const StringView& trader, const StringView& market,
                           const StringView& contr, Jday settlDay, Iden id, const StringView& ref,
                           State state, Side side, Lots lots, Ticks ticks, Lots resd, Lots exec,
                           Cost cost, Lots lastLots, Ticks lastTicks, Lots minLots, Millis created,
                           Millis modified) const
{
    return doNewOrder(trader, market, contr, settlDay, id, ref, state, side, lots, ticks, resd,
                      exec, cost, lastLots, lastTicks, minLots, created, modified);
}

OrderPtr Factory::newOrder(const StringView& trader, const StringView& market,
                           const StringView& contr, Jday settlDay, Iden id, const StringView& ref,
                           Side side, Lots lots, Ticks ticks, Lots minLots, Millis created) const
{
    return doNewOrder(trader, market, contr, settlDay, id, ref, State::NEW, side, lots, ticks, lots,
                      0_lts, 0_cst, 0_lts, 0_tks, minLots, created, created);
}

ExecPtr Factory::newExec(const StringView& trader, const StringView& market,
                         const StringView& contr, Jday settlDay, Iden id, const StringView& ref,
                         Iden orderId, State state, Side side, Lots lots, Ticks ticks, Lots resd,
                         Lots exec, Cost cost, Lots lastLots, Ticks lastTicks, Lots minLots,
                         Iden matchId, Role role, const StringView& cpty, Millis created) const
{
    return doNewExec(trader, market, contr, settlDay, id, ref, orderId, state, side, lots, ticks,
                     resd, exec, cost, lastLots, lastTicks, minLots, matchId, role, cpty, created);
}

ExecPtr Factory::newExec(const Order& order, Iden id, Millis created) const
{
    return doNewExec(order.trader(), order.market(), order.contr(), order.settlDay(), id,
                     order.ref(), order.id(), order.state(), order.side(), order.lots(),
                     order.ticks(), order.resd(), order.exec(), order.cost(), order.lastLots(),
                     order.lastTicks(), order.minLots(), 0_id, Role::NONE, StringView{}, created);
}

PosnPtr Factory::newPosn(const StringView& trader, const StringView& contr, Jday settlDay,
                         Lots buyLots, Cost buyCost, Lots sellLots, Cost sellCost) const
{
    return doNewPosn(trader, contr, settlDay, buyLots, buyCost, sellLots, sellCost);
}

PosnPtr Factory::newPosn(const StringView& trader, const StringView& contr, Jday settlDay) const
{
    return doNewPosn(trader, contr, settlDay, 0_lts, 0_cst, 0_lts, 0_cst);
}

unique_ptr<Asset> BasicFactory::doNewAsset(const StringView& mnem, const StringView& display,
                                           AssetType type) const
{
    return make_unique<Asset>(mnem, display, type);
}

unique_ptr<Contr> BasicFactory::doNewContr(const StringView& mnem, const StringView& display,
                                           const StringView& asset, const StringView& ccy,
                                           int lotNumer, int lotDenom, int tickNumer, int tickDenom,
                                           int pipDp, Lots minLots, Lots maxLots) const
{
    return make_unique<Contr>(mnem, display, asset, ccy, lotNumer, lotDenom, tickNumer, tickDenom,
                              pipDp, minLots, maxLots);
}

unique_ptr<Market> BasicFactory::doNewMarket(const StringView& mnem, const StringView& display,
                                             const StringView& contr, Jday settlDay, Jday expiryDay,
                                             MarketState state, Lots lastLots, Ticks lastTicks,
                                             Millis lastTime, Iden maxOrderId, Iden maxExecId) const
{
    // Note that the last six arguments are unused in this base implementation.
    return make_unique<Market>(mnem, display, contr, settlDay, expiryDay, state);
}

unique_ptr<Trader> BasicFactory::doNewTrader(const StringView& mnem, const StringView& display,
                                             const StringView& email) const
{
    return make_unique<Trader>(mnem, display, email);
}

OrderPtr BasicFactory::doNewOrder(const StringView& trader, const StringView& market,
                                  const StringView& contr, Jday settlDay, Iden id,
                                  const StringView& ref, State state, Side side, Lots lots,
                                  Ticks ticks, Lots resd, Lots exec, Cost cost, Lots lastLots,
                                  Ticks lastTicks, Lots minLots, Millis created,
                                  Millis modified) const
{
    return makeRefCounted<Order>(trader, market, contr, settlDay, id, ref, state, side, lots, ticks,
                                 resd, exec, cost, lastLots, lastTicks, minLots, created, modified);
}

ExecPtr BasicFactory::doNewExec(const StringView& trader, const StringView& market,
                                const StringView& contr, Jday settlDay, Iden id,
                                const StringView& ref, Iden orderId, State state, Side side,
                                Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost,
                                Lots lastLots, Ticks lastTicks, Lots minLots, Iden matchId,
                                Role role, const StringView& cpty, Millis created) const
{
    return makeRefCounted<Exec>(trader, market, contr, settlDay, id, ref, orderId, state, side,
                                lots, ticks, resd, exec, cost, lastLots, lastTicks, minLots,
                                matchId, role, cpty, created);
}

PosnPtr BasicFactory::doNewPosn(const StringView& trader, const StringView& contr, Jday settlDay,
                                Lots buyLots, Cost buyCost, Lots sellLots, Cost sellCost) const
{
    return makeRefCounted<Posn>(trader, contr, settlDay, buyLots, buyCost, sellLots, sellCost);
}

BasicFactory::~BasicFactory() noexcept = default;

} // swirly
