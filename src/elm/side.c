/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/elm/side.h>

#include <sc/elm/pool.h>

#include <sc/ash/err.h>
#include <sc/ash/log.h>

#include <stdlib.h>

static struct ScLevel*
lazy_level(struct ScSide* side, struct ScOrder* order)
{
    const ScIden key = order->i.action == SC_ACTION_BUY ? -order->i.ticks : order->i.ticks;
	struct ScRbNode* node = sc_tree_pfind(&side->levels, key);

	struct ScLevel* level;
    if (!node || node->key != key) {
        if (!(level = sc_pool_alloc_level(side->pool)))
            return NULL;
        sc_level_init(level);

        level->first_order = order;
        level->ticks = order->i.ticks;
        level->lots = order->i.resd;
        level->count = 1;
        sc_log_debug2("insert level: contr=%.16s,settl_day=%d,ticks=%ld",
                       order->contr.rec->mnem, order->settl_day, order->ticks);
        sc_tree_pinsert(&side->levels, key, &level->side_node_, node);
    } else {
        level = sc_side_level_entry(node);
        level->lots += order->i.resd;
        ++level->count;
    }

    order->level = level;
    return level;
}

static inline void
reduce(struct ScSide* side, struct ScOrder* order, ScLots delta)
{
    assert(order);
    assert(order->level);
    assert(delta >= 0 && delta <= order->i.resd);

    if (delta < order->i.resd) {
        // Reduce level and order by lots.
        order->level->lots -= delta;
        order->i.resd -= delta;
    } else {
        assert(delta == order->i.resd);
        sc_side_remove_order(side, order);
        order->i.resd = 0;
    }
}

SC_API void
sc_side_init(struct ScSide* side, ScPool pool)
{
    side->pool = pool;
    sc_tree_init(&side->levels);
    sc_list_init(&side->orders);

    // Last trade.
    side->last_ticks = 0;
    side->last_lots = 0;
    side->last_time = 0;
}

SC_API void
sc_side_term(struct ScSide* side)
{
    assert(side);
    while (!sc_list_empty(&side->orders)) {
        struct ScOrder* order = sc_side_order_entry(sc_list_remove_first(&side->orders));
        // Revision is unchanged by this call.
        sc_side_remove_order(side, order);
    }
}

SC_API ScBool
sc_side_insert_order(struct ScSide* side, struct ScOrder* order)
{
    assert(order);
    assert(!order->level);
    assert(order->i.ticks != 0);
    assert(order->i.resd > 0);
    assert(order->i.exec <= order->i.lots);
    assert(order->i.lots > 0);
    assert(order->i.min_lots >= 0);

	struct ScLevel* level = lazy_level(side, order);
    if (!level)
        return SC_FALSE;

    struct ScRbNode* next_level = sc_rbnode_next(&level->side_node_);
    struct ScDlNode* end = next_level
        ? &sc_side_level_entry(next_level)->first_order->side_node_
        : sc_list_end(&side->orders);
    // Insert order after the level's last order.
    // I.e. insert order before the next level's first order.
    sc_dlnode_insert_before(end, &order->side_node_);
    return SC_TRUE;
}

SC_API void
sc_side_remove_order(struct ScSide* side, struct ScOrder* order)
{
    struct ScLevel* level = order->level;
    level->lots -= order->i.resd;

    if (--level->count <= 0) {
        // Remove level.
        assert(level->lots == 0);
        sc_log_debug2("remove level: contr=%.16s,settl_day=%d,ticks=%ld",
                       order->contr.rec->mnem, order->settl_day, order->ticks);
        sc_tree_remove(&side->levels, &level->side_node_);
        sc_pool_free_level(side->pool, level);
    } else if (level->first_order == order) {
        // First order at this level is being removed.
        level->first_order = sc_side_order_entry(order->side_node_.next);
    }

    sc_dlnode_remove(&order->side_node_);
    sc_dlnode_init(&order->side_node_);

    // No longer associated with side.
    order->level = NULL;
}

SC_API void
sc_side_take_order(struct ScSide* side, struct ScOrder* order, ScLots lots, ScMillis now)
{
    reduce(side, order, lots);

    // Last trade.
    side->last_ticks = order->i.ticks;
    side->last_lots = lots;
    side->last_time = now;

    order->i.state = SC_STATE_TRADE;
    order->i.exec += lots;
    order->i.last_ticks = order->i.ticks;
    order->i.last_lots = lots;
    order->modified = now;
}

SC_API void
sc_side_revise_order(struct ScSide* side, struct ScOrder* order, ScLots lots, ScMillis now)
{
    assert(order);
    assert(order->level);
    assert(lots > 0);
    assert(lots >= order->i.exec && lots >= order->i.min_lots && lots <= order->i.lots);

    const ScLots delta = order->i.lots - lots;

    // This will increase order revision.
    reduce(side, order, delta);

    order->i.state = SC_STATE_REVISE;
    order->i.lots = lots;
    order->modified = now;
}
