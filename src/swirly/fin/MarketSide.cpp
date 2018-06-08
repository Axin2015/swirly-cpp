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
#include "MarketSide.hpp"

namespace swirly {
inline namespace fin {
using namespace std;

MarketSide::~MarketSide() = default;

MarketSide::MarketSide(MarketSide&&) = default;

void MarketSide::insert_order(const OrderPtr& order)
{
    assert(order->level() == nullptr);
    assert(order->ticks() != 0_tks);
    assert(order->resd_lots() > 0_lts);
    assert(order->exec_lots() <= order->lots());
    assert(order->lots() > 0_lts);
    assert(order->min_lots() >= 0_lts);

    auto it = insert_level(order);
    // Next level.
    ++it;
    if (it != levels_.end()) {
        // Insert order after the level's last order.
        // I.e. insert order before the next level's first order.
        orders_.insert_before(order, it->first_order());
    } else {
        orders_.insert_back(order);
    }
}

LevelSet::Iterator MarketSide::insert_level(const OrderPtr& order)
{
    LevelSet::Iterator it;
    bool found;
    tie(it, found) = levels_.find_hint(order->side(), order->ticks());
    if (!found) {
        it = levels_.emplace_hint(it, *order);
    } else {
        it->add_order(*order);
    }
    order->set_level(&*it);
    return it;
}

void MarketSide::remove_order(Level& level, const Order& order) noexcept
{
    level.sub_order(order);

    if (level.count() == 0) {
        // Remove level.
        assert(level.lots() == 0_lts);
        levels_.remove(level);
    } else if (&level.first_order() == &order) {
        // First order at this level is being removed.
        auto it = OrderList::to_iterator(order);
        level.set_first_order(*++it);
    }

    orders_.remove(order);

    // No longer associated with side.
    order.set_level(nullptr);
}

void MarketSide::reduce_level(Level& level, const Order& order, Lots delta) noexcept
{
    assert(delta >= 0_lts);
    assert(delta <= order.resd_lots());

    if (delta < order.resd_lots()) {
        // Reduce level's resd by delta.
        level.reduce(delta);
    } else {
        assert(delta == order.resd_lots());
        remove_order(level, order);
    }
}

} // namespace fin
} // namespace swirly
