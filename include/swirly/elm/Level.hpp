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
#ifndef SWIRLY_ELM_LEVEL_HPP
#define SWIRLY_ELM_LEVEL_HPP

#include <swirly/elm/Order.hpp>

namespace swirly {

/**
 * @addtogroup Book
 * @{
 */

using LevelKey = Ticks;

namespace detail {

/**
 * Synthetic level key.
 */
inline constexpr LevelKey composeKey(Side side, Ticks ticks)
{
    return side == Side::BUY ? -ticks : ticks;
}

} // detail

/**
 * Price level.
 *
 * A price level is an aggregation of orders by price. I.e. the sum of all orders in the book at the
 * same price.
 */
class SWIRLY_API Level : public Comparable<Level> {
    const Order* firstOrder_;
    const LevelKey key_;
    const Ticks ticks_;
    /**
     * Must be greater than zero.
     */
    Lots resd_;
    Lots quotd_;
    /**
     * Must be greater than zero.
     */
    int count_;
 public:
    explicit Level(const Order& firstOrder) noexcept
    :   firstOrder_{&firstOrder},
        key_{detail::composeKey(firstOrder.side(), firstOrder.ticks())},
        ticks_{firstOrder.ticks()},
        resd_{firstOrder.resd()},
        quotd_{firstOrder.quotd()},
        count_{1}
    {
    }

    ~Level() noexcept;

    // Copy.
    Level(const Level&) = default;
    Level& operator =(const Level&) = default;

    // Move.
    Level(Level&&) = default;
    Level& operator =(Level&&) = default;

    int compare(const Level& rhs) const noexcept
    {
        return swirly::compare(key_, rhs.key_);
    }
    const Order& firstOrder() const noexcept
    {
        return *firstOrder_;
    }
    LevelKey key() const noexcept
    {
        return key_;
    }
    Ticks ticks() const noexcept
    {
        return ticks_;
    }
    Lots resd() const noexcept
    {
        return resd_;
    }
    Lots quotd() const noexcept
    {
        return quotd_;
    }
    int count() const noexcept
    {
        return count_;
    }
};

/** @} */

} // swirly

#endif // SWIRLY_ELM_LEVEL_HPP
