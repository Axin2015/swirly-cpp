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

#include <swirly/elm/Market.hpp>

using namespace std;

namespace swirly {

Factory::~Factory() noexcept = default;

MarketPtr Factory::newMarket(Mnem mnem, string_view display, Mnem contr, Jday settlDay,
                             Jday expiryDay, MarketState state, Lots lastLots, Ticks lastTicks,
                             Millis lastTime, Iden maxOrderId, Iden maxExecId) const
{
  return doNewMarket(mnem, display, contr, settlDay, expiryDay, state, lastLots, lastTicks,
                     lastTime, maxOrderId, maxExecId);
}

BasicFactory::~BasicFactory() noexcept = default;

MarketPtr BasicFactory::doNewMarket(Mnem mnem, string_view display, Mnem contr, Jday settlDay,
                                    Jday expiryDay, MarketState state, Lots lastLots,
                                    Ticks lastTicks, Millis lastTime, Iden maxOrderId,
                                    Iden maxExecId) const
{
  // Note that the last six arguments are unused in this base implementation.
  return Market::make(mnem, display, contr, settlDay, expiryDay, state);
}

} // swirly
