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
#ifndef SWIRLY_ELM_TYPES_HPP
#define SWIRLY_ELM_TYPES_HPP

#include <swirly/ash/Enum.hpp>
#include <swirly/ash/String.hpp>

#include <boost/intrusive_ptr.hpp>

#include <cstdint>
#include <memory>

namespace swirly {

enum class Iden : int64_t {};

constexpr Iden operator""_id(unsigned long long val) noexcept
{
  return box<Iden>(val);
}

using Incs = int64_t;

enum class Lots : Incs {};

constexpr Lots operator""_lts(unsigned long long val) noexcept
{
  return box<Lots>(val);
}

/**
 * Unit representing the minimum price increment.
 */
enum class Ticks : Incs {};

constexpr Ticks operator""_tks(unsigned long long val) noexcept
{
  return box<Ticks>(val);
}

/**
 * Sum of lots and ticks.
 */
enum class Cost : Incs {};

constexpr Cost operator""_cst(unsigned long long val) noexcept
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
constexpr std::size_t MaxDisplay{64};

/**
 * Maximum reference characters.
 */
constexpr std::size_t MaxRef{64};

/**
 * Description suitable for display on user-interface.
 */
using Display = String<MaxDisplay>;

/**
 * Reference.
 */
using Ref = String<MaxRef>;

enum class AssetType { Commodity = 1, Corporate, Currency, Equity, Government, Index };

SWIRLY_API const char* enumString(AssetType type);

template <>
struct EnumTraits<AssetType> {
  static void print(std::ostream& os, AssetType val) { os << enumString(val); }
};

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

template <>
struct EnumTraits<Direct> {
  static void print(std::ostream& os, Direct val) { os << enumString(val); }
};

enum class RecType {
  /**
   * Asset.
   */
  Asset = 1,
  /**
   * Contract.
   */
  Contr,
  /**
   * Market.
   */
  Market
};

SWIRLY_API const char* enumString(RecType type);

template <>
struct EnumTraits<RecType> {
  static void print(std::ostream& os, RecType val) { os << enumString(val); }
};

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

template <>
struct EnumTraits<LiqInd> {
  static void print(std::ostream& os, LiqInd val) { os << enumString(val); }
};

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

template <>
struct EnumTraits<Side> {
  static void print(std::ostream& os, Side val) { os << enumString(val); }
};

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

template <>
struct EnumTraits<State> {
  static void print(std::ostream& os, State val) { os << enumString(val); }
};

class Asset;
using AssetPtr = std::unique_ptr<Asset>;
using ConstAssetPtr = std::unique_ptr<const Asset>;

class Contr;
using ContrPtr = std::unique_ptr<Contr>;
using ConstContrPtr = std::unique_ptr<const Contr>;

class Market;
using MarketPtr = std::unique_ptr<Market>;
using ConstMarketPtr = std::unique_ptr<const Market>;

class MarketBook;
using MarketBookPtr = std::unique_ptr<MarketBook>;
using ConstMarketBookPtr = std::unique_ptr<const MarketBook>;

class Request;
using RequestPtr = boost::intrusive_ptr<Request>;
using ConstRequestPtr = boost::intrusive_ptr<const Request>;

class Exec;
using ExecPtr = boost::intrusive_ptr<Exec>;
using ConstExecPtr = boost::intrusive_ptr<const Exec>;

class Order;
using OrderPtr = boost::intrusive_ptr<Order>;
using ConstOrderPtr = boost::intrusive_ptr<const Order>;

class Posn;
using PosnPtr = boost::intrusive_ptr<Posn>;
using ConstPosnPtr = boost::intrusive_ptr<const Posn>;

enum class More : int { No, Yes };

} // swirly

#endif // SWIRLY_ELM_TYPES_HPP
