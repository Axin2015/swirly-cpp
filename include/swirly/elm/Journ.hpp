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
#ifndef SWIRLY_ELM_JOURN_HPP
#define SWIRLY_ELM_JOURN_HPP

#include <swirly/elm/Types.hpp>

#include <swirly/ash/Types.hpp>

namespace swirly {

/**
 * @addtogroup IO
 * @{
 */

class SWIRLY_API Journ {
protected:
    virtual void doCreateMarket(const StringView& mnem, const StringView& display,
                                const StringView& contr, Jd settlDay, Jd expiryDay,
                                MarketState state) = 0;

public:
    Journ() noexcept = default;
    virtual ~Journ() noexcept;

    // Copy.
    constexpr Journ(const Journ&) noexcept = default;
    Journ& operator =(const Journ&) noexcept = default;

    // Move.
    constexpr Journ(Journ&&) noexcept = default;
    Journ& operator =(Journ&&) noexcept = default;

    void createMarket(const StringView& mnem, const StringView& display,
                      const StringView& contr, Jd settlDay, Jd expiryDay,
                      MarketState state)
    {
        return doCreateMarket(mnem, display, contr, settlDay, expiryDay, state);
    }
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_JOURN_HPP
