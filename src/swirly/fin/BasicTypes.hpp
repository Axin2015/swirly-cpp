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
    return fbs::EnumNameAssetType(type);
}

using Direct = fbs::Direct;

inline const char* enumString(Direct direct) noexcept
{
    return fbs::EnumNameDirect(direct);
}

using LiqInd = fbs::LiqInd;

inline const char* enumString(LiqInd liqInd) noexcept
{
    return fbs::EnumNameLiqInd(liqInd);
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
    return fbs::EnumNameSide(side);
}

constexpr Side opposite(Side side) noexcept
{
    switch (side) {
    case Side::None:
        break;
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
    return fbs::EnumNameState(state);
}

/**
 * This overload is used for presentation purposes only. There is no inverse operation.
 */
inline const char* enumString(State state, Lots resd) noexcept
{
    if (state == State::Trade) {
        return resd == 0_lts ? "Complete" : "Partial";
    }
    return fbs::EnumNameState(state);
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
