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
#include "ServFactory.hpp"

#include <swirly/fig/TraderSess.hpp>

#include <swirly/elm/MarketBook.hpp>

using namespace std;

namespace swirly {

unique_ptr<Market> ServFactory::doNewMarket(const StringView& mnem, const StringView& display,
                                            const StringView& contr, Jday settlDay,
                                            Jday expiryDay, MarketState state, Lots lastLots,
                                            Ticks lastTicks, Millis lastTime, Iden maxOrderId,
                                            Iden maxExecId, Iden maxQuoteId) const
{
    return make_unique<MarketBook>(mnem, display, contr, settlDay, expiryDay, state, lastLots,
                                   lastTicks, lastTime, maxOrderId, maxExecId, maxQuoteId);
}

unique_ptr<Trader> ServFactory::doNewTrader(const StringView& mnem, const StringView& display,
                                            const StringView& email) const
{
    return make_unique<TraderSess>(mnem, display, email);
}

ServFactory::~ServFactory() noexcept = default;

} // swirly
