/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#ifndef SWIRLY_FIN_MARKETSIDE_HPP
#define SWIRLY_FIN_MARKETSIDE_HPP

#include <swirly/fin/Level.hpp>
#include <swirly/fin/Order.hpp>

namespace swirly {

class SWIRLY_API MarketSide {
 public:
  MarketSide() = default;

  ~MarketSide() noexcept;

  // Copy.
  MarketSide(const MarketSide&) = delete;
  MarketSide& operator=(const MarketSide&) = delete;

  // Move.
  MarketSide(MarketSide&&);
  MarketSide& operator=(MarketSide&&) = delete;

  const LevelSet& levels() const noexcept { return levels_; }
  const OrderList& orders() const noexcept { return orders_; }
  LevelSet& levels() noexcept { return levels_; }
  OrderList& orders() noexcept { return orders_; }

  /**
   * Insert order into side. Assumes that the order does not already belong to a side. I.e. it
   * assumes that level member is null. Assumes that order-id and reference are unique. This
   * function will only throw if a new level cannot be allocated.
   */
  void insertOrder(const OrderPtr& order) throw(std::bad_alloc);

  /**
   * Remove order from side. Internal housekeeping aside, the state of the order is not affected by
   * this function.
   */
  void removeOrder(const Order& order) noexcept
  {
    Level* const level{order.level()};
    if (level != nullptr) {
      removeOrder(*level, order);
    }
  }
  void createOrder(const OrderPtr& order, Time now) throw(std::bad_alloc)
  {
    order->create(now);
    insertOrder(order);
  }
  void reviseOrder(Order& order, Lots lots, Time now) noexcept
  {
    assert(lots > 0_lts);
    assert(lots >= order.exec());
    assert(lots >= order.minLots());
    assert(lots <= order.lots());

    Level* const level{order.level()};
    if (level != nullptr) {
      const Lots delta = order.lots() - lots;
      reduceLevel(*level, order, delta);
    }
    order.revise(lots, now);
  }
  void cancelOrder(Order& order, Time now) noexcept
  {
    Level* const level{order.level()};
    if (level != nullptr) {
      removeOrder(*level, order);
    }
    order.cancel(now);
  }
  /**
   * Reduce residual lots by lots. If the resulting residual is zero, then the order is removed from
   * the side.
   */
  void takeOrder(Order& order, Lots lots, Time now) noexcept
  {
    Level* const level{order.level()};
    if (level != nullptr) {
      reduceLevel(*level, order, lots);
    }
    order.trade(lots, order.ticks(), now);
  }

 private:
  /**
   * Insert level. This function will only throw if a new level cannot be allocated.
   */
  LevelSet::Iterator insertLevel(const OrderPtr& order) throw(std::bad_alloc);

  void removeOrder(Level& level, const Order& order) noexcept;

  void reduceLevel(Level& level, const Order& order, Lots delta) noexcept;

  LevelSet levels_;
  OrderList orders_;
};

} // swirly

#endif // SWIRLY_FIN_MARKETSIDE_HPP
