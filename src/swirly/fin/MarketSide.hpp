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
#ifndef SWIRLY_FIN_MARKETSIDE_HPP
#define SWIRLY_FIN_MARKETSIDE_HPP

#include <swirly/fin/Level.hpp>
#include <swirly/fin/Order.hpp>

namespace swirly {
inline namespace fin {

class SWIRLY_API MarketSide {
  public:
    MarketSide() = default;

    ~MarketSide();

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
     *
     * Throws std::bad_alloc.
     */
    void insert_order(const OrderPtr& order);

    /**
     * Remove order from side. Internal housekeeping aside, the state of the order is not affected
     * by this function.
     */
    void remove_order(const Order& order) noexcept
    {
        Level* const level{order.level()};
        if (level != nullptr) {
            remove_order(*level, order);
        }
    }
    /**
     * Throws std::bad_alloc.
     */
    void create_order(Time now, const OrderPtr& order)
    {
        order->create(now);
        insert_order(order);
    }
    void revise_order(Time now, Order& order, Lots lots) noexcept
    {
        assert(lots > 0_lts);
        assert(lots >= order.exec_lots());
        assert(lots >= order.min_lots());
        assert(lots <= order.lots());

        Level* const level{order.level()};
        if (level != nullptr) {
            const Lots delta = order.lots() - lots;
            reduce_level(*level, order, delta);
        }
        order.revise(now, lots);
    }
    void cancel_order(Time now, Order& order) noexcept
    {
        Level* const level{order.level()};
        if (level != nullptr) {
            remove_order(*level, order);
        }
        order.cancel(now);
    }
    /**
     * Reduce residual lots by lots. If the resulting residual is zero, then the order is removed
     * from the side.
     */
    void take_order(Time now, Order& order, Lots lots) noexcept
    {
        Level* const level{order.level()};
        if (level != nullptr) {
            reduce_level(*level, order, lots);
        }
        order.trade(now, lots, order.ticks());
    }

  private:
    /**
     * Insert level. This function will only throw if a new level cannot be allocated.
     *
     * Throws std::bad_alloc.
     */
    LevelSet::Iterator insert_level(const OrderPtr& order);

    void remove_order(Level& level, const Order& order) noexcept;

    void reduce_level(Level& level, const Order& order, Lots delta) noexcept;

    LevelSet levels_;
    OrderList orders_;
};

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_MARKETSIDE_HPP
