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
#include <swirly/elm/MarketBook.hpp>

#include <cassert>

using namespace std;

namespace swirly {

MarketBook::MarketBook(const string_view& mnem, const string_view& display,
                       const string_view& contr, Jday settlDay, Jday expiryDay, MarketState state,
                       Lots lastLots, Ticks lastTicks, Millis lastTime, Iden maxOrderId,
                       Iden maxExecId) noexcept
  : Market{mnem, display, contr, settlDay, expiryDay, state},
    lastLots_{lastLots},
    lastTicks_{lastTicks},
    lastTime_{lastTime},
    view_{mnem, contr, settlDay, lastLots, lastTicks, lastTime},
    maxOrderId_{maxOrderId},
    maxExecId_{maxExecId}
{
}

MarketBook::~MarketBook() noexcept = default;

} // swirly
