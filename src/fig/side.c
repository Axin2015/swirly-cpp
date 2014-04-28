/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
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
#include <dbr/side.h>

#include <dbr/err.h>
#include <dbr/log.h>
#include <dbr/pool.h>

#include <stdlib.h>

static struct DbrLevel*
lazy_level(struct DbrSide* side, struct DbrOrder* order)
{
    const DbrIden key = order->c.action == DBR_ACTION_BUY ? -order->c.ticks : order->c.ticks;
	struct DbrRbNode* node = dbr_tree_pfind(&side->levels, key);

	struct DbrLevel* level;
    if (!node || node->key != key) {
        if (!(level = dbr_pool_alloc_level(side->pool)))
            return NULL;
        dbr_level_init(level);

        level->first_order = order;
        level->ticks = order->c.ticks;
        level->lots = order->c.resd;
        level->count = 1;
        dbr_log_debug2("insert level: contr=%.16s,settl_day=%d,ticks=%ld",
                       order->contr.rec->mnem, order->settl_day, order->ticks);
        dbr_tree_pinsert(&side->levels, key, &level->side_node_, node);
    } else {
        level = dbr_side_level_entry(node);
        level->lots += order->c.resd;
        ++level->count;
    }

    order->level = level;
    return level;
}

static inline void
reduce(struct DbrSide* side, struct DbrOrder* order, DbrLots delta)
{
    assert(order);
    assert(order->level);
    assert(delta >= 0 && delta <= order->c.resd);

    if (delta < order->c.resd) {
        // Reduce level and order by lots.
        order->level->lots -= delta;
        order->c.resd -= delta;
    } else {
        assert(delta == order->c.resd);
        dbr_side_remove_order(side, order);
        order->c.resd = 0;
    }
}

DBR_API void
dbr_side_init(struct DbrSide* side, DbrPool pool)
{
    side->pool = pool;
    dbr_tree_init(&side->levels);
    dbr_list_init(&side->orders);

    // Last trade.
    side->last_ticks = 0;
    side->last_lots = 0;
    side->last_time = 0;
}

DBR_API void
dbr_side_term(struct DbrSide* side)
{
    assert(side);
    while (!dbr_list_empty(&side->orders)) {
        struct DbrOrder* order = dbr_side_order_entry(dbr_list_remove_first(&side->orders));
        // Revision is unchanged by this call.
        dbr_side_remove_order(side, order);
    }
}

DBR_API DbrBool
dbr_side_insert_order(struct DbrSide* side, struct DbrOrder* order)
{
    assert(order);
    assert(!order->level);
    assert(order->c.ticks != 0);
    assert(order->c.resd > 0);
    assert(order->c.exec <= order->c.lots);
    assert(order->c.lots > 0);
    assert(order->c.min_lots >= 0);

	struct DbrLevel* level = lazy_level(side, order);
    if (!level)
        return DBR_FALSE;

    struct DbrRbNode* next_level = dbr_rbnode_next(&level->side_node_);
    struct DbrDlNode* end = next_level
        ? &dbr_side_level_entry(next_level)->first_order->side_node_
        : dbr_list_end(&side->orders);
    // Insert order after the level's last order.
    // I.e. insert order before the next level's first order.
    dbr_dlnode_insert_before(end, &order->side_node_);
    return DBR_TRUE;
}

DBR_API void
dbr_side_remove_order(struct DbrSide* side, struct DbrOrder* order)
{
    struct DbrLevel* level = order->level;
    level->lots -= order->c.resd;

    if (--level->count <= 0) {
        // Remove level.
        assert(level->lots == 0);
        dbr_log_debug2("remove level: contr=%.16s,settl_day=%d,ticks=%ld",
                       order->contr.rec->mnem, order->settl_day, order->ticks);
        dbr_tree_remove(&side->levels, &level->side_node_);
        dbr_pool_free_level(side->pool, level);
    } else if (level->first_order == order) {
        // First order at this level is being removed.
        level->first_order = dbr_side_order_entry(order->side_node_.next);
    }

    dbr_dlnode_remove(&order->side_node_);
    dbr_dlnode_init(&order->side_node_);

    // No longer associated with side.
    order->level = NULL;
}

DBR_API void
dbr_side_take_order(struct DbrSide* side, struct DbrOrder* order, DbrLots lots, DbrMillis now)
{
    reduce(side, order, lots);

    // Last trade.
    side->last_ticks = order->c.ticks;
    side->last_lots = lots;
    side->last_time = now;

    order->c.state = DBR_STATE_TRADE;
    order->c.exec += lots;
    order->c.last_ticks = order->c.ticks;
    order->c.last_lots = lots;
    order->modified = now;
}

DBR_API void
dbr_side_revise_order(struct DbrSide* side, struct DbrOrder* order, DbrLots lots, DbrMillis now)
{
    assert(order);
    assert(order->level);
    assert(lots > 0);
    assert(lots >= order->c.exec && lots >= order->c.min_lots && lots <= order->c.lots);

    const DbrLots delta = order->c.lots - lots;

    // This will increase order revision.
    reduce(side, order, delta);

    order->c.state = DBR_STATE_REVISE;
    order->c.lots = lots;
    order->modified = now;
}
