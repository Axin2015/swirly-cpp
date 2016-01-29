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

#include <swirly/ash/Enum.hpp>
#include <swirly/ash/String.hpp>

#include <boost/intrusive_ptr.hpp>

#include <cstdint>

namespace swirly {

/**
 * @addtogroup Domain
 * @{
 */

enum class Iden : int64_t { };

constexpr Iden operator ""_id(unsigned long long val) noexcept
{
    return box<Iden>(val);
}

using Incs = int64_t;

enum class Lots : Incs { };

constexpr Lots operator ""_lts(unsigned long long val) noexcept
{
    return box<Lots>(val);
}

/**
 * Unit representing the minimum price increment.
 */
enum class Ticks : Incs { };

constexpr Ticks operator ""_tks(unsigned long long val) noexcept
{
    return box<Ticks>(val);
}

/**
 * Sum of lots and ticks.
 */
enum class Cost : Incs { };

constexpr Cost operator ""_cst(unsigned long long val) noexcept
{
    return box<Cost>(val);
}

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

SWIRLY_API const char* enumString(AssetType type);

template <>
struct EnumTraits<AssetType> {
    static void print(std::ostream& os, AssetType val) noexcept
    {
        os << enumString(val);
    }
};

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

SWIRLY_API const char* enumString(Direct direct);

template <>
struct EnumTraits<Direct> {
    static void print(std::ostream& os, Direct val) noexcept
    {
        os << enumString(val);
    }
};

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

SWIRLY_API const char* enumString(RecType type);

template <>
struct EnumTraits<RecType> {
    static void print(std::ostream& os, RecType val) noexcept
    {
        os << enumString(val);
    }
};

enum class Role {
    /**
     * No role.
     */
    NONE = 0,
    /**
     * Passive buyer or seller that receives the spread.
     */
    MAKER,
    /**
     * Aggressive buyer or seller that crosses the market and pays the spread.
     */
    TAKER
};

SWIRLY_API const char* enumString(Role role);

template <>
struct EnumTraits<Role> {
    static void print(std::ostream& os, Role val) noexcept
    {
        os << enumString(val);
    }
};

enum class Side {
    BUY = 1,
    SELL = -1
};

SWIRLY_API const char* enumString(Side side);

template <>
struct EnumTraits<Side> {
    static void print(std::ostream& os, Side val) noexcept
    {
        os << enumString(val);
    }
};

/**
 * Order states.
 * @image html OrderState.png
 */
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
    TRADE
};

SWIRLY_API const char* enumString(State state);

template <>
struct EnumTraits<State> {
    static void print(std::ostream& os, State val) noexcept
    {
        os << enumString(val);
    }
};

/** @} */

/**
 * @addtogroup Entity
 * @{
 */

class Asset;
class Contr;
class Market;
class Trader;

class Exec;
class Order;
class Posn;
class Quote;

using ExecPtr = boost::intrusive_ptr<Exec>;
using OrderPtr = boost::intrusive_ptr<Order>;
using PosnPtr = boost::intrusive_ptr<Posn>;
using QuotePtr = boost::intrusive_ptr<Quote>;

/** @} */

} // swirly

#endif // SWIRLY_ELM_TYPES_HPP
