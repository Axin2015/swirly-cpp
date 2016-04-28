/*
 * The Restful Matching-Engine.
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

OrderPtr Factory::newOrder(string_view trader, string_view market, string_view contr, Jday settlDay,
                           Iden id, string_view ref, State state, Side side, Lots lots, Ticks ticks,
                           Lots resd, Lots exec, Cost cost, Lots lastLots, Ticks lastTicks,
                           Lots minLots, Millis created, Millis modified) const
{
  return doNewOrder(trader, market, contr, settlDay, id, ref, state, side, lots, ticks, resd, exec,
                    cost, lastLots, lastTicks, minLots, created, modified);
}

OrderPtr Factory::newOrder(string_view trader, string_view market, string_view contr, Jday settlDay,
                           Iden id, string_view ref, Side side, Lots lots, Ticks ticks,
                           Lots minLots, Millis created) const
{
  return doNewOrder(trader, market, contr, settlDay, id, ref, State::New, side, lots, ticks, lots,
                    0_lts, 0_cst, 0_lts, 0_tks, minLots, created, created);
}

ExecPtr Factory::newExec(string_view trader, string_view market, string_view contr, Jday settlDay,
                         Iden id, string_view ref, Iden orderId, State state, Side side, Lots lots,
                         Ticks ticks, Lots resd, Lots exec, Cost cost, Lots lastLots,
                         Ticks lastTicks, Lots minLots, Iden matchId, Role role, string_view cpty,
                         Millis created) const
{
  return doNewExec(trader, market, contr, settlDay, id, ref, orderId, state, side, lots, ticks,
                   resd, exec, cost, lastLots, lastTicks, minLots, matchId, role, cpty, created);
}

ExecPtr Factory::newExec(const Order& order, Iden id, Millis created) const
{
  return doNewExec(order.trader(), order.market(), order.contr(), order.settlDay(), id, order.ref(),
                   order.id(), order.state(), order.side(), order.lots(), order.ticks(),
                   order.resd(), order.exec(), order.cost(), order.lastLots(), order.lastTicks(),
                   order.minLots(), 0_id, Role::None, string_view{}, created);
}

PosnPtr Factory::newPosn(string_view trader, string_view contr, Jday settlDay, Lots buyLots,
                         Cost buyCost, Lots sellLots, Cost sellCost) const
{
  return doNewPosn(trader, contr, settlDay, buyLots, buyCost, sellLots, sellCost);
}

PosnPtr Factory::newPosn(string_view trader, string_view contr, Jday settlDay) const
{
  return doNewPosn(trader, contr, settlDay, 0_lts, 0_cst, 0_lts, 0_cst);
}

std::unique_ptr<Asset> Factory::newAsset(string_view mnem, string_view display,
                                         AssetType type) const
{
  return doNewAsset(mnem, display, type);
}

std::unique_ptr<Contr> Factory::newContr(string_view mnem, string_view display, string_view asset,
                                         string_view ccy, int lotNumer, int lotDenom, int tickNumer,
                                         int tickDenom, int pipDp, Lots minLots, Lots maxLots) const
{
  return doNewContr(mnem, display, asset, ccy, lotNumer, lotDenom, tickNumer, tickDenom, pipDp,
                    minLots, maxLots);
}

std::unique_ptr<Market> Factory::newMarket(string_view mnem, string_view display, string_view contr,
                                           Jday settlDay, Jday expiryDay, MarketState state,
                                           Lots lastLots, Ticks lastTicks, Millis lastTime,
                                           Iden maxOrderId, Iden maxExecId) const
{
  return doNewMarket(mnem, display, contr, settlDay, expiryDay, state, lastLots, lastTicks,
                     lastTime, maxOrderId, maxExecId);
}

std::unique_ptr<Market> Factory::newMarket(string_view mnem, string_view display, string_view contr,
                                           Jday settlDay, Jday expiryDay, MarketState state) const
{
  return doNewMarket(mnem, display, contr, settlDay, expiryDay, state, 0_lts, 0_tks, 0_ms, 0_id,
                     0_id);
}

std::unique_ptr<Trader> Factory::newTrader(string_view mnem, string_view display,
                                           string_view email) const
{
  return doNewTrader(mnem, display, email);
}

BasicFactory::~BasicFactory() noexcept = default;

unique_ptr<Asset> BasicFactory::doNewAsset(string_view mnem, string_view display,
                                           AssetType type) const
{
  return make_unique<Asset>(mnem, display, type);
}

unique_ptr<Contr> BasicFactory::doNewContr(string_view mnem, string_view display, string_view asset,
                                           string_view ccy, int lotNumer, int lotDenom,
                                           int tickNumer, int tickDenom, int pipDp, Lots minLots,
                                           Lots maxLots) const
{
  return make_unique<Contr>(mnem, display, asset, ccy, lotNumer, lotDenom, tickNumer, tickDenom,
                            pipDp, minLots, maxLots);
}

unique_ptr<Market> BasicFactory::doNewMarket(string_view mnem, string_view display,
                                             string_view contr, Jday settlDay, Jday expiryDay,
                                             MarketState state, Lots lastLots, Ticks lastTicks,
                                             Millis lastTime, Iden maxOrderId, Iden maxExecId) const
{
  // Note that the last six arguments are unused in this base implementation.
  return make_unique<Market>(mnem, display, contr, settlDay, expiryDay, state);
}

unique_ptr<Trader> BasicFactory::doNewTrader(string_view mnem, string_view display,
                                             string_view email) const
{
  return make_unique<Trader>(mnem, display, email);
}

OrderPtr BasicFactory::doNewOrder(string_view trader, string_view market, string_view contr,
                                  Jday settlDay, Iden id, string_view ref, State state, Side side,
                                  Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost,
                                  Lots lastLots, Ticks lastTicks, Lots minLots, Millis created,
                                  Millis modified) const
{
  return makeRefCounted<Order>(trader, market, contr, settlDay, id, ref, state, side, lots, ticks,
                               resd, exec, cost, lastLots, lastTicks, minLots, created, modified);
}

ExecPtr BasicFactory::doNewExec(string_view trader, string_view market, string_view contr,
                                Jday settlDay, Iden id, string_view ref, Iden orderId, State state,
                                Side side, Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost,
                                Lots lastLots, Ticks lastTicks, Lots minLots, Iden matchId,
                                Role role, string_view cpty, Millis created) const
{
  return makeRefCounted<Exec>(trader, market, contr, settlDay, id, ref, orderId, state, side, lots,
                              ticks, resd, exec, cost, lastLots, lastTicks, minLots, matchId, role,
                              cpty, created);
}

PosnPtr BasicFactory::doNewPosn(string_view trader, string_view contr, Jday settlDay, Lots buyLots,
                                Cost buyCost, Lots sellLots, Cost sellCost) const
{
  return makeRefCounted<Posn>(trader, contr, settlDay, buyLots, buyCost, sellLots, sellCost);
}

} // swirly
