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
#include "ServFactory.hpp"

#include <swirly/elm/MarketBook.hpp>

using namespace std;

namespace swirly {

ServFactory::~ServFactory() noexcept = default;

MarketPtr ServFactory::doNewMarket(Mnem mnem, string_view display, Mnem contr, Jday settlDay,
                                   Jday expiryDay, MarketState state, Lots lastLots,
                                   Ticks lastTicks, Millis lastTime, Iden maxOrderId,
                                   Iden maxExecId) const
{
  return MarketBook::make(mnem, display, contr, settlDay, expiryDay, state, lastLots, lastTicks,
                          lastTime, maxOrderId, maxExecId);
}

} // swirly
