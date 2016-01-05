/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
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

using namespace std;

namespace swirly {

Factory::~Factory() noexcept = default;

unique_ptr<Asset> BasicFactory::doNewAsset(const StringView& mnem, const StringView& display,
                                           AssetType type) const
{
    return make_unique<Asset>(mnem, display, type);
}

unique_ptr<Contr> BasicFactory::doNewContr(const StringView& mnem, const StringView& display,
                                           const StringView& asset, const StringView& ccy,
                                           int lotNumer, int lotDenom,
                                           int tickNumer, int tickDenom, int pipDp,
                                           Lots minLots, Lots maxLots) const
{
    return make_unique<Contr>(mnem, display, asset, ccy, lotNumer, lotDenom, tickNumer, tickDenom,
                              pipDp, minLots, maxLots);
}

unique_ptr<Market> BasicFactory::doNewMarket(const StringView& mnem, const StringView& display,
                                             const StringView& contr, Jd settlDay,
                                             Jd expiryDay, MarketState state, Lots lastLots,
                                             Ticks lastTicks, Millis lastTime, Iden maxOrderId,
                                             Iden maxExecId, Iden maxQuoteId) const
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
                                  const StringView& contr, Jd settlDay, Iden id,
                                  const StringView& ref, Iden quoteId, State state, Side side,
                                  Lots lots, Ticks ticks, Lots resd, Lots exec, Cost cost,
                                  Lots lastLots, Ticks lastTicks, Lots minLots, bool pecan,
                                  Millis created, Millis modified) const
{
    return makeRefCounted<Order>(trader, market, contr, settlDay, id, ref, quoteId, state, side,
                                 lots, ticks, resd, exec, cost, lastLots, lastTicks, minLots,
                                 pecan, created, modified);
}

BasicFactory::~BasicFactory() noexcept = default;

} // swirly
