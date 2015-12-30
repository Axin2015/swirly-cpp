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
#ifndef SWIRLY_ELM_TYPES_HPP
#define SWIRLY_ELM_TYPES_HPP

#include <swirly/ash/String.hpp>

namespace swirly {

/**
 * @addtogroup Domain
 * @{
 */

using Incs = long;

/**
 * Milliseconds since Unix epoch.
 */
using Millis = long;

using Lots = Incs;

/**
 * Unit representing the minimum price increment.
 */
using Ticks = Incs;

/**
 * Sum of lots and ticks.
 */
using Cost = Incs;

/**
 * Bitfield representing the state of a Market.
 */
using MarketState = unsigned;

/**
 * Maximum display characters.
 */
constexpr std::size_t DISPLAY_MAX = 64;

/**
 * Maximum email characters.
 */
constexpr std::size_t EMAIL_MAX = 64;

/**
 * Maximum mnemonic characters.
 */
constexpr std::size_t MNEM_MAX = 16;

/**
 * Maximum reference characters.
 */
constexpr std::size_t REF_MAX = 64;

/**
 * Description suitable for display on user-interface.
 */
using Display = StringBuf<DISPLAY_MAX>;

/**
 * Email address.
 */
using Email = StringBuf<EMAIL_MAX>;

/**
 * Memorable identifier.
 */
using Mnem = StringBuf<MNEM_MAX>;

/**
 * Reference.
 */
using Ref = StringBuf<REF_MAX>;

enum class AssetType {
    COMMODITY = 1,
    CORPORATE,
    CURRENCY,
    EQUITY,
    GOVERNMENT,
    INDEX
};

SWIRLY_API const char* enumToString(AssetType type);

enum class Direct {
    /**
     * Aggressor buys. I.e. taker lifts the offer.
     */
    PAID = 1,
    /**
     * Aggressor sells. I.e. taker hits the bid.
     */
    GIVEN = -1
};

SWIRLY_API const char* enumToString(Direct direct);

enum class RecType {
    /**
     * Asset.
     */
    ASSET = 1,
    /**
     * Contract.
     */
    CONTR,
    /**
     * Market.
     */
    MARKET,
    /**
     * Trader.
     */
    TRADER
};

SWIRLY_API const char* enumToString(RecType type);

enum class Role {
    /**
     * Passive buyer or seller that receives the spread.
     */
    MAKER = 1,
    /**
     * Aggressive buyer or seller that crosses the market and pays the spread.
     */
    TAKER
};

SWIRLY_API const char* enumToString(Role role);

enum class Side {
    BUY = 1,
    SELL = -1
};

SWIRLY_API const char* enumToString(Side side);

enum class State {
    NONE = 0,
    /**
     * Initial state of a resting order placed in the order-book.
     */
    NEW,
    /**
     * State of a resting order that has been revised.
     */
    REVISE,
    /**
     * State of a resting order that has been cancelled.
     */
    CANCEL,
    /**
     * State of an order that has been partially or fully filled.
     */
    TRADE,
    /**
     * State of a resting order that is pending cancel.
     */
    PECAN
};

SWIRLY_API const char* enumToString(State state);

/** @} */

} // swirly

#endif // SWIRLY_ELM_TYPES_HPP
