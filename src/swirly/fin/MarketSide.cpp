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

void MarketSide::insertOrder(const OrderPtr& order)
{
    assert(order->level() == nullptr);
    assert(order->ticks() != 0_tks);
    assert(order->resdLots() > 0_lts);
    assert(order->execLots() <= order->lots());
    assert(order->lots() > 0_lts);
    assert(order->minLots() >= 0_lts);

    auto it = insertLevel(order);
    // Next level.
    ++it;
    if (it != levels_.end()) {
        // Insert order after the level's last order.
        // I.e. insert order before the next level's first order.
        orders_.insertBefore(order, it->firstOrder());
    } else {
        orders_.insertBack(order);
    }
}

LevelSet::Iterator MarketSide::insertLevel(const OrderPtr& order)
{
    LevelSet::Iterator it;
    bool found;
    tie(it, found) = levels_.findHint(order->side(), order->ticks());
    if (!found) {
        it = levels_.emplaceHint(it, *order);
    } else {
        it->addOrder(*order);
    }
    order->setLevel(&*it);
    return it;
}

void MarketSide::removeOrder(Level& level, const Order& order) noexcept
{
    level.subOrder(order);

    if (level.count() == 0) {
        // Remove level.
        assert(level.lots() == 0_lts);
        levels_.remove(level);
    } else if (&level.firstOrder() == &order) {
        // First order at this level is being removed.
        auto it = OrderList::toIterator(order);
        level.setFirstOrder(*++it);
    }

    orders_.remove(order);

    // No longer associated with side.
    order.setLevel(nullptr);
}

void MarketSide::reduceLevel(Level& level, const Order& order, Lots delta) noexcept
{
    assert(delta >= 0_lts);
    assert(delta <= order.resdLots());

    if (delta < order.resdLots()) {
        // Reduce level's resd by delta.
        level.reduce(delta);
    } else {
        assert(delta == order.resdLots());
        removeOrder(level, order);
    }
}

} // namespace fin
} // namespace swirly
