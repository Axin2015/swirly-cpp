/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#include "side.h"

#include <dbr/conv.h>
#include <dbr/err.h>
#include <dbr/log.h>

#include <stdlib.h>

static struct DbrLevel*
lazy_level(struct FigSide* side, struct DbrOrder* order)
{
    const DbrIden key = order->action == DBR_BUY ? -order->ticks : order->ticks;
	struct DbrRbNode* node = dbr_tree_pfind(&side->levels, key);

	struct DbrLevel* level;
    if (!node || node->key != key) {
        if (!(level = dbr_pool_alloc_level(side->pool, key)))
            return NULL;

        level->first_order = order;
        level->count = 1;
        level->ticks = order->ticks;
        level->resd = order->resd;
        dbr_log_debug2("insert level: market=%.16s,ticks=%ld", order->market.rec->mnem,
                       order->ticks);
        dbr_tree_pinsert(&side->levels, &level->side_node_, node);
    } else {
        level = dbr_side_level_entry(node);
        ++level->count;
        level->resd += order->resd;
    }

    order->level = level;
    return level;
}

static inline void
reduce(struct FigSide* side, struct DbrOrder* order, DbrLots delta)
{
    assert(order);
    assert(order->level);
    assert(delta >= 0 && delta <= order->resd);

    if (delta < order->resd) {
        // Reduce level and order by delta.
        order->level->resd -= delta;
        order->resd -= delta;
    } else {
        assert(delta == order->resd);
        fig_side_remove_order(side, order);
        order->resd = 0;
    }
}

DBR_EXTERN void
fig_side_init(struct FigSide* side, DbrPool pool)
{
    side->pool = pool;
    dbr_tree_init(&side->levels);
    dbr_list_init(&side->orders);

    // Last trade.
    side->last_ticks = 0;
    side->last_lots = 0;
    side->last_time = 0;
}

DBR_EXTERN void
fig_side_term(struct FigSide* side)
{
    assert(side);
    while (!dbr_list_empty(&side->orders)) {
        struct DbrOrder* order = dbr_side_order_entry(dbr_list_remove_first(&side->orders));
        // Revision is unchanged by this call.
        fig_side_remove_order(side, order);
    }
}

DBR_EXTERN DbrBool
fig_side_insert_order(struct FigSide* side, struct DbrOrder* order)
{
    assert(order);
    assert(!order->level);
    assert(order->ticks != 0);
    assert(order->resd > 0);
    assert(order->exec <= order->lots);
    assert(order->lots > 0);
    assert(order->min >= 0);

	struct DbrLevel* level = lazy_level(side, order);
    if (!level)
        return false;

    struct DbrRbNode* next_level = dbr_rbnode_next(&level->side_node_);
    struct DbrDlNode* end = next_level
        ? &dbr_side_level_entry(next_level)->first_order->side_node_
        : dbr_list_end(&side->orders);
    // Insert order after the level's last order.
    // I.e. insert order before the next level's first order.
    dbr_dlnode_insert_before(end, &order->side_node_);
    return true;
}

DBR_EXTERN void
fig_side_remove_order(struct FigSide* side, struct DbrOrder* order)
{
    struct DbrLevel* level = order->level;
    level->resd -= order->resd;

    if (--level->count <= 0) {
        // Remove level.
        assert(level->resd == 0);
        dbr_log_debug2("remove level: market=%.16s,ticks=%ld", order->market.rec->mnem,
                       order->ticks);
        dbr_tree_remove(&side->levels, &level->side_node_);
        dbr_pool_free_level(side->pool, level);
    } else if (level->first_order == order) {
        // First order at this level is being removed.
        level->first_order = dbr_side_order_entry(order->side_node_.next);
    }

    dbr_dlnode_remove(&order->side_node_);

    // No longer associated with side.
    order->level = NULL;
    dbr_dlnode_init(&order->side_node_);
}

DBR_EXTERN void
fig_side_take_order(struct FigSide* side, struct DbrOrder* order, DbrLots delta, DbrMillis now)
{
    reduce(side, order, delta);

    // Last trade.
    side->last_ticks = order->ticks;
    side->last_lots = delta;
    side->last_time = now;

    ++order->rev;
    order->status = order->resd == 0 ? DBR_FILLED : DBR_PARTIAL;
    order->exec += delta;
    order->modified = now;
}

DBR_EXTERN DbrBool
fig_side_revise_order(struct FigSide* side, struct DbrOrder* order, DbrLots lots, DbrMillis now)
{
    assert(order);
    assert(order->level);
    assert(lots >= 0);

    // Assumes that order already belongs to this side.
    // Reduced lots must not be:
    // 1. less than executed lots;
    // 2. less than min lots;
    // 3. greater than original lots.

    if (lots < order->exec || lots < order->min || lots > order->lots) {
        dbr_err_set(DBR_EINVAL, "invalid revision lots");
        return false;
    }

    const DbrLots delta = order->lots - lots;

    // This will increase order revision.
    reduce(side, order, delta);

    ++order->rev;
    order->status = DBR_REVISED;
    order->lots = lots;
    order->modified = now;

    return true;
}

DBR_API struct DbrDlNode*
dbr_side_first_order(DbrSide side)
{
    return fig_side_first_order(side);
}

DBR_API struct DbrDlNode*
dbr_side_last_order(DbrSide side)
{
    return fig_side_last_order(side);
}

DBR_API struct DbrDlNode*
dbr_side_end_order(DbrSide side)
{
    return fig_side_end_order(side);
}

DBR_API DbrBool
dbr_side_empty_order(DbrSide side)
{
    return fig_side_empty_order(side);
}

// SideLevel

DBR_API struct DbrRbNode*
dbr_side_find_level(DbrSide side, DbrTicks ticks)
{
    return fig_side_find_level(side, ticks);
}

DBR_API struct DbrRbNode*
dbr_side_first_level(DbrSide side)
{
    return fig_side_first_level(side);
}

DBR_API struct DbrRbNode*
dbr_side_last_level(DbrSide side)
{
    return fig_side_last_level(side);
}

DBR_API struct DbrRbNode*
dbr_side_end_level(DbrSide side)
{
    return fig_side_end_level(side);
}

DBR_API DbrBool
dbr_side_empty_level(DbrSide side)
{
    return fig_side_empty_level(side);
}

// SideLast

DBR_API DbrTicks
dbr_side_last_ticks(DbrSide side)
{
    return fig_side_last_ticks(side);
}

DBR_API DbrLots
dbr_side_last_lots(DbrSide side)
{
    return fig_side_last_lots(side);
}

DBR_API DbrMillis
dbr_side_last_time(DbrSide side)
{
    return fig_side_last_time(side);
}
