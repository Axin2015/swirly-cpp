/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
#ifndef SWIRLY_FIN_BASICTYPES_HPP
#define SWIRLY_FIN_BASICTYPES_HPP

#include <swirly/util/IntWrapper.hpp>

#include <swirly/fbs/Types_generated.h>

#include <ostream>

namespace swirly {
inline namespace fin {

using Incs = std::int64_t;
using IncsPolicy = IntPolicy<Incs>;

struct LotsPolicy : IncsPolicy {
};
struct TicksPolicy : IncsPolicy {
};
struct CostPolicy : IncsPolicy {
};

using Lots = IntWrapper<LotsPolicy>;

constexpr Lots operator""_lts(unsigned long long val) noexcept
{
    return Lots{val};
}

/**
 * Unit representing the minimum price increment.
 */
using Ticks = IntWrapper<TicksPolicy>;

constexpr Ticks operator""_tks(unsigned long long val) noexcept
{
    return Ticks{val};
}

/**
 * Sum of lots and ticks.
 */
using Cost = IntWrapper<CostPolicy>;

constexpr Cost operator""_cst(unsigned long long val) noexcept
{
    return Cost{val};
}

/**
 * Bitfield representing the state of a Market.
 */
using MarketState = unsigned;
using AssetType = fbs::AssetType;

inline const char* enumString(AssetType type) noexcept
{
    switch (type) {
    case AssetType::None:
        return "NONE";
    case AssetType::Cmdty:
        return "CMDTY";
    case AssetType::Corp:
        return "CORP";
    case AssetType::Ccy:
        return "CCY";
    case AssetType::Eqty:
        return "EQTY";
    case AssetType::Govt:
        return "GOVT";
    case AssetType::Index:
        return "INDEX";
    }
    std::terminate();
}

using Direct = fbs::Direct;

inline const char* enumString(Direct direct) noexcept
{
    switch (direct) {
    case Direct::Paid:
        return "PAID";
    case Direct::Given:
        return "GIVEN";
    }
    std::terminate();
}

using LiqInd = fbs::LiqInd;

inline const char* enumString(LiqInd liqInd) noexcept
{
    switch (liqInd) {
    case LiqInd::None:
        return "NONE";
    case LiqInd::Maker:
        return "MAKER";
    case LiqInd::Taker:
        return "TAKER";
    }
    std::terminate();
}

constexpr LiqInd opposite(LiqInd liqInd) noexcept
{
    switch (liqInd) {
    case LiqInd::None:
        break;
    case LiqInd::Maker:
        liqInd = LiqInd::Taker;
        break;
    case LiqInd::Taker:
        liqInd = LiqInd::Maker;
        break;
    }
    return liqInd;
}

using Side = fbs::Side;

inline const char* enumString(Side side) noexcept
{
    switch (side) {
    case Side::Buy:
        return "BUY";
    case Side::Sell:
        return "SELL";
    }
    std::terminate();
}

constexpr Side opposite(Side side) noexcept
{
    switch (side) {
    case Side::Buy:
        side = Side::Sell;
        break;
    case Side::Sell:
        side = Side::Buy;
        break;
    }
    return side;
}

/**
 * Order states.
 * @image html OrderState.png
 */
using State = fbs::State;

inline const char* enumString(State state) noexcept
{
    switch (state) {
    case State::None:
        return "NONE";
    case State::New:
        return "NEW";
    case State::Revise:
        return "REVISE";
    case State::Cancel:
        return "CANCEL";
    case State::Trade:
        return "TRADE";
    }
    std::terminate();
}

inline const char* enumString(State state, Lots resd) noexcept
{
    switch (state) {
    case State::None:
        return "NONE";
    case State::New:
        return "NEW";
    case State::Revise:
        return "REVISE";
    case State::Cancel:
        return "CANCEL";
    case State::Trade:
        return resd == 0_lts ? "COMPLETE" : "PARTIAL";
    }
    std::terminate();
}

} // namespace fin

namespace fbs {

inline std::ostream& operator<<(std::ostream& os, AssetType type)
{
    return os << enumString(type);
}

inline std::ostream& operator<<(std::ostream& os, Direct direct)
{
    return os << enumString(direct);
}

inline std::ostream& operator<<(std::ostream& os, LiqInd liqInd)
{
    return os << enumString(liqInd);
}

inline std::ostream& operator<<(std::ostream& os, Side side)
{
    return os << enumString(side);
}

inline std::ostream& operator<<(std::ostream& os, State state)
{
    return os << enumString(state);
}

} // namespace fbs
} // namespace swirly

#endif // SWIRLY_FIN_BASICTYPES_HPP
