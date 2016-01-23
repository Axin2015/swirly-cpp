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
#include <swirly/elm/BookSide.hpp>

using namespace std;

namespace swirly {

LevelSet::Iterator BookSide::insertLevel(const OrderPtr& order) throw (std::bad_alloc)
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

void BookSide::reduceLevel(const Order& order, Lots rDelta, Lots qDelta) noexcept
{
    using namespace enumops;

    assert(order.level() != nullptr);
    assert(rDelta >= 0_lts);
    assert(rDelta <= order.resd());

    if (rDelta < order.resd()) {
        // Reduce level's resd by delta.
        Level* const level{order.level()};
        assert(level != nullptr);
        level->reduce(rDelta, qDelta);
    } else {
        assert(rDelta == order.resd());
        removeOrder(order);
    }
}

BookSide::~BookSide() noexcept = default;

BookSide::BookSide(BookSide&&) = default;

void BookSide::insertOrder(const OrderPtr& order) throw (std::bad_alloc)
{
    assert(order->level() != nullptr);
    assert(order->ticks() != 0_tks);
    assert(order->resd() > 0_lts);
    assert(order->exec() <= order->lots());
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

void BookSide::removeOrder(const Order& order) noexcept
{
    Level* const level{order.level()};
    if (level == nullptr)
        return;

    level->subOrder(order);

    if (level->count() == 0) {
        // Remove level.
        assert(level->resd() == 0_lts);
        levels_.remove(*level);
    } else if (&level->firstOrder() == &order) {
        // First order at this level is being removed.
        auto it = OrderList::toIterator(order);
        level->setFirstOrder(*++it);
    }

    orders_.remove(order);

    // No longer associated with side.
    order.setLevel(nullptr);
}

} // swirly
