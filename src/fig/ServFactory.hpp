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
#ifndef SWIRLY_FIG_SERVFACTORY_HPP
#define SWIRLY_FIG_SERVFACTORY_HPP

#include <swirly/elm/Factory.hpp>

namespace swirly {

/**
 * @addtogroup App
 * @{
 */

class SWIRLY_API ServFactory : public BasicFactory {
 protected:
    std::unique_ptr<Market> doNewMarket(const StringView& mnem, const StringView& display,
                                        const StringView& contr, Jday settlDay, Jday expiryDay,
                                        MarketState state, Lots lastLots, Ticks lastTicks,
                                        Millis lastTime, Iden maxOrderId,
                                        Iden maxExecId) const override;

    std::unique_ptr<Trader> doNewTrader(const StringView& mnem, const StringView& display,
                                        const StringView& email) const override;

 public:
    ServFactory() noexcept = default;
    ~ServFactory() noexcept override;

    // Copy.
    ServFactory(const ServFactory&) = default;
    ServFactory& operator=(const ServFactory&) = default;

    // Move.
    ServFactory(ServFactory&&) = default;
    ServFactory& operator=(ServFactory&&) = default;
};

/** @} */

} // swirly

#endif // SWIRLY_FIG_SERVFACTORY_HPP
