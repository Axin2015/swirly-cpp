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

#include "ctx.h"

#include <dbr/conv.h>
#include <dbr/err.h>

#include <stdlib.h>

static struct DbrLevel*
lazy_level(struct ElmSide* side, struct DbrOrder* order)
{
    const DbrIden key = order->action == DBR_BUY ? -order->ticks : order->ticks;
	struct DbrRbNode* node = ash_tree_pfind(&side->levels, key);

	struct DbrLevel* level;
    if (!node || node->key != key) {
        if (!(level = elm_ctx_alloc_level(side->ctx, key)))
            return NULL;

        level->first_order = order;
        level->count = 1;
        level->ticks = order->ticks;
        level->resd = order->resd;
        DBR_DEBUG2F("insert level: market=%.16s,ticks=%ld", order->market.rec->mnem,
                    order->ticks);
        ash_tree_pinsert(&side->levels, &level->side_node_, node);
    } else {
        level = dbr_side_level_entry(node);
        ++level->count;
        level->resd += order->resd;
    }

    order->level = level;
    return level;
}

static inline void
reduce(struct ElmSide* side, struct DbrOrder* order, DbrLots delta)
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
        elm_side_remove_order(side, order);
        order->resd = 0;
    }
}

DBR_EXTERN void
elm_side_init(struct ElmSide* side, struct ElmCtx* ctx)
{
    side->ctx = ctx;
    ash_tree_init(&side->levels);
    ash_list_init(&side->orders);

    // Last trade.
    side->last_ticks = 0;
    side->last_lots = 0;
    side->last_time = 0;
}

DBR_EXTERN void
elm_side_term(struct ElmSide* side)
{
    assert(side);
    while (!ash_list_empty(&side->orders)) {
        struct DbrOrder* order = dbr_side_order_entry(ash_list_remove_first(&side->orders));
        // Revision is unchanged by this call.
        elm_side_remove_order(side, order);
    }
}

DBR_EXTERN DbrBool
elm_side_insert_order(struct ElmSide* side, struct DbrOrder* order)
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

    struct DbrRbNode* next_level = ash_rbnode_next(&level->side_node_);
    struct DbrDlNode* end = next_level
        ? &dbr_side_level_entry(next_level)->first_order->side_node_
        : ash_list_end(&side->orders);
    // Insert order after the level's last order.
    // I.e. insert order before the next level's first order.
    ash_dlnode_insert_before(end, &order->side_node_);
    return true;
}

DBR_EXTERN void
elm_side_remove_order(struct ElmSide* side, struct DbrOrder* order)
{
    struct DbrLevel* level = order->level;
    level->resd -= order->resd;

    if (--level->count <= 0) {
        // Remove level.
        assert(level->resd == 0);
        DBR_DEBUG2F("remove level: market=%.16s,ticks=%ld", order->market.rec->mnem,
                    order->ticks);
        ash_tree_remove(&side->levels, &level->side_node_);
        elm_ctx_free_level(side->ctx, level);
    } else if (level->first_order == order) {
        // First order at this level is being removed.
        level->first_order = dbr_side_order_entry(order->side_node_.next);
    }

    ash_dlnode_remove(&order->side_node_);

    // No longer associated with side.
    order->level = NULL;
    ash_dlnode_init(&order->side_node_);
}

DBR_EXTERN void
elm_side_take_order(struct ElmSide* side, struct DbrOrder* order, DbrLots delta, DbrMillis now)
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
elm_side_revise_order(struct ElmSide* side, struct DbrOrder* order, DbrLots lots, DbrMillis now)
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
        dbr_err_set(&side->ctx->err, DBR_EINVAL, "invalid revision lots");
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
