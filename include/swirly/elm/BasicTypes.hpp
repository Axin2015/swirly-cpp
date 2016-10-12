/*
 * The Restful Matching-Engine.
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
#ifndef SWIRLY_ELM_BASICTYPES_HPP
#define SWIRLY_ELM_BASICTYPES_HPP

#include <swirly/ash/IntWrapper.hpp>

#include <ostream>

namespace swirly {

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

enum class AssetType { Commodity = 1, Corporate, Currency, Equity, Government, Index };

SWIRLY_API const char* enumString(AssetType type);

inline std::ostream& operator<<(std::ostream& os, AssetType type)
{
  return os << enumString(type);
}

enum class Direct {
  /**
   * Aggressor bought. Taker lifted the offer resulting in a market uptick.
   */
  Paid = 1,
  /**
   * Aggressor sold. Taker hit the bid resulting in a market dow
   */
  Given = -1
};

SWIRLY_API const char* enumString(Direct direct);

inline std::ostream& operator<<(std::ostream& os, Direct direct)
{
  return os << enumString(direct);
}

enum class LiqInd {
  /**
   * No liqInd.
   */
  None = 0,
  /**
   * Passive buyer or seller that receives the spread.
   */
  Maker,
  /**
   * Aggressive buyer or seller that crosses the market and pays the spread.
   */
  Taker
};

SWIRLY_API const char* enumString(LiqInd liqInd);

constexpr LiqInd inverse(LiqInd liqInd) noexcept
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

inline std::ostream& operator<<(std::ostream& os, LiqInd liqInd)
{
  return os << enumString(liqInd);
}

enum class Side { Buy = 1, Sell = -1 };

SWIRLY_API const char* enumString(Side side);

constexpr Side inverse(Side side) noexcept
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

inline std::ostream& operator<<(std::ostream& os, Side side)
{
  return os << enumString(side);
}

/**
 * Order states.
 * @image html OrderState.png
 */
enum class State {
  None = 0,
  /**
   * Initial state of a resting order placed in the order-book.
   */
  New,
  /**
   * State of a resting order that has been revised.
   */
  Revise,
  /**
   * State of a resting order that has been cancelled.
   */
  Cancel,
  /**
   * State of an order that has been partially or fully filled.
   */
  Trade
};

SWIRLY_API const char* enumString(State state);

inline std::ostream& operator<<(std::ostream& os, State state)
{
  return os << enumString(state);
}

} // swirly

#endif // SWIRLY_ELM_BASICTYPES_HPP
