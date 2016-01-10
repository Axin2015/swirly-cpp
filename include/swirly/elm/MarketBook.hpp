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
#ifndef SWIRLY_ELM_MARKETBOOK_HPP
#define SWIRLY_ELM_MARKETBOOK_HPP

#include <swirly/elm/BookSide.hpp>
#include <swirly/elm/Market.hpp>
#include <swirly/elm/MarketView.hpp>

namespace swirly {

/**
 * @addtogroup Book
 * @{
 */

class SWIRLY_API MarketBook : public Market {

    Lots lastLots_;
    Ticks lastTicks_;
    Millis lastTime_;
    // Two sides constitute the book.
    BookSide bidSide_;
    BookSide offerSide_;
    MarketView view_;
    Iden maxOrderId_;
    Iden maxExecId_;
    Iden maxQuoteId_;

 public:
    MarketBook(const StringView& mnem, const StringView& display, const StringView& contr,
               Jd settlDay, Jd expiryDay, MarketState state, Lots lastLots, Ticks lastTicks,
               Millis lastTime, Iden maxOrderId, Iden maxExecId, Iden maxQuoteId) noexcept;

    ~MarketBook() noexcept override;

    // Copy.
    MarketBook(const MarketBook&) = default;
    MarketBook& operator =(const MarketBook&) = default;

    // Move.
    MarketBook(MarketBook&&) = default;
    MarketBook& operator =(MarketBook&&) = default;

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
    const BookSide& bidSide() const noexcept
    {
        return bidSide_;
    }
    const BookSide& offerSide() const noexcept
    {
        return offerSide_;
    }
    const MarketView& view() const noexcept
    {
        return view_;
    }
    Iden maxOrderId() const noexcept
    {
        return maxOrderId_;
    }
    Iden maxExecId() const noexcept
    {
        return maxExecId_;
    }
    Iden maxQuoteId() const noexcept
    {
        return maxQuoteId_;
    }
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_MARKETBOOK_HPP
