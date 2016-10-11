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
#ifndef SWIRLY_ELM_MARKET_HPP
#define SWIRLY_ELM_MARKET_HPP

#include <swirly/elm/MarketSide.hpp>

#include <swirly/ash/Date.hpp>
#include <swirly/ash/Set.hpp>

namespace swirly {

using MarketPtr = boost::intrusive_ptr<Market>;
using ConstMarketPtr = boost::intrusive_ptr<const Market>;

class SWIRLY_API Market : public RefCounted, public Comparable<Market> {
 public:
  Market(Id64 id, Mnem contr, JDay settlDay, MarketState state, Lots lastLots = 0_lts,
         Ticks lastTicks = 0_tks, Time lastTime = {}, Id64 maxId = 0_id64) noexcept
    : id_{id},
      contr_{contr},
      settlDay_{settlDay},
      state_{state},
      lastLots_{lastLots},
      lastTicks_{lastTicks},
      lastTime_{lastTime},
      maxId_{maxId}
  {
  }
  ~Market() noexcept override;

  // Copy.
  Market(const Market&) = delete;
  Market& operator=(const Market&) = delete;

  // Move.
  Market(Market&&);
  Market& operator=(Market&&) = delete;

  template <typename... ArgsT>
  static MarketPtr make(ArgsT&&... args)
  {
    return makeRefCounted<Market>(std::forward<ArgsT>(args)...);
  }

  void toJson(std::ostream& os) const;

  int compare(const Market& rhs) const noexcept { return swirly::compare(id_, rhs.id_); }
  auto id() const noexcept { return id_; }
  auto contr() const noexcept { return contr_; }
  auto settlDay() const noexcept { return settlDay_; }
  auto state() const noexcept { return state_; }
  Lots lastLots() const noexcept { return lastLots_; }
  Ticks lastTicks() const noexcept { return lastTicks_; }
  Time lastTime() const noexcept { return lastTime_; }
  const MarketSide& bidSide() const noexcept { return bidSide_; }
  const MarketSide& offerSide() const noexcept { return offerSide_; }
  Id64 maxId() const noexcept { return maxId_; }

  void setState(MarketState state) noexcept { state_ = state; }
  MarketSide& bidSide() noexcept { return bidSide_; }
  MarketSide& offerSide() noexcept { return offerSide_; }
  void insertOrder(const OrderPtr& order) throw(std::bad_alloc)
  {
    side(order->side()).insertOrder(order);
  }
  void removeOrder(const Order& order) noexcept { side(order.side()).removeOrder(order); }
  void createOrder(const OrderPtr& order, Time now) throw(std::bad_alloc)
  {
    side(order->side()).createOrder(order, now);
  }
  void reviseOrder(Order& order, Lots lots, Time now) noexcept
  {
    side(order.side()).reviseOrder(order, lots, now);
  }
  void cancelOrder(Order& order, Time now) noexcept { side(order.side()).cancelOrder(order, now); }
  void takeOrder(Order& order, Lots lots, Time now) noexcept
  {
    side(order.side()).takeOrder(order, lots, now);
    lastLots_ = lots;
    lastTicks_ = order.ticks();
    lastTime_ = now;
  }
  Id64 allocId() noexcept { return ++maxId_; }
  boost::intrusive::set_member_hook<> idHook_;

 private:
  MarketSide& side(Side side) noexcept { return side == Side::Buy ? bidSide_ : offerSide_; }
  const MarketSide& side(Side side) const noexcept
  {
    return side == Side::Buy ? bidSide_ : offerSide_;
  }

  const Id64 id_;
  const Mnem contr_;
  const JDay settlDay_;
  MarketState state_;
  Lots lastLots_;
  Ticks lastTicks_;
  Time lastTime_;
  // Two sides constitute the market.
  MarketSide bidSide_;
  MarketSide offerSide_;
  Id64 maxId_;
};

inline std::ostream& operator<<(std::ostream& os, const Market& market)
{
  market.toJson(os);
  return os;
}

using MarketSet = IdSet<Market>;

} // swirly

#endif // SWIRLY_ELM_MARKET_HPP
