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
#ifndef SWIRLY_ELM_MARKETVIEW_HPP
#define SWIRLY_ELM_MARKETVIEW_HPP

#include <swirly/elm/MarketData.hpp>
#include <swirly/elm/Types.hpp>

#include <swirly/ash/Types.hpp>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

/**
 * A flattened view of a market.
 */
class SWIRLY_API MarketView {

    const Mnem market_;
    const Mnem contr_;
    const Jday settlDay_;
    Lots lastLots_;
    Ticks lastTicks_;
    Millis lastTime_;
    const MarketData data_;

 public:
    MarketView(const StringView& market, const StringView& contr, Jday settlDay, Lots lastLots,
               Ticks lastTicks, Millis lastTime, const MarketData& data) noexcept;

    MarketView(const StringView& market, const StringView& contr, Jday settlDay, Lots lastLots,
               Ticks lastTicks, Millis lastTime) noexcept;

    ~MarketView() noexcept;

    // Copy.
    MarketView(const MarketView&);
    MarketView& operator=(const MarketView&) = delete;

    // Move.
    MarketView(MarketView&&);
    MarketView& operator=(MarketView&&) = delete;

    StringView market() const noexcept
    {
        return market_.view();
    }
    StringView contr() const noexcept
    {
        return contr_.view();
    }
    Jday settlDay() const noexcept
    {
        return settlDay_;
    }
    Lots lastLots() const noexcept
    {
        return lastLots_;
    }
    Ticks lastTicks() const noexcept
    {
        return lastTicks_;
    }
    Millis lastTime() const noexcept
    {
        return lastTime_;
    }
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_MARKETVIEW_HPP
