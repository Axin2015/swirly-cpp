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
#ifndef SWIRLY_ELM_MARKET_HPP
#define SWIRLY_ELM_MARKET_HPP

#include <swirly/elm/Rec.hpp>

#include <swirly/ash/Types.hpp>

namespace swirly {

/**
 * @addtogroup Entity
 * @{
 */

/**
 * A place where buyers and sellers come together to exchange goods or services.
 */
class SWIRLY_API Market : public Rec {
    const Mnem contr_;
    const Jday settlDay_;
    const Jday expiryDay_;
    MarketState state_;

 public:
    boost::intrusive::set_member_hook<> mnemHook_;

    Market(const StringView& mnem, const StringView& display, const StringView& contr,
           Jday settlDay, Jday expiryDay, MarketState state) noexcept;

    ~Market() noexcept override;

    // Copy.
    Market(const Market&);
    Market& operator=(const Market&) = delete;

    // Move.
    Market(Market&&);
    Market& operator=(Market&&) = delete;

    void setState(MarketState state) noexcept
    {
        state_ = state;
    }
    StringView contr() const noexcept
    {
        return contr_.view();
    }
    Jday settlDay() const noexcept
    {
        return settlDay_;
    }
    Jday expiryDay() const noexcept
    {
        return expiryDay_;
    }
    MarketState state() const noexcept
    {
        return state_;
    }
};

using MarketSet = RecSet<Market>;

/** @} */

} // swirly

#endif // SWIRLY_ELM_MARKET_HPP
