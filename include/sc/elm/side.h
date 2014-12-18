
/*
 *  02110-1301 USA.
 */
#ifndef SC_ELM_SIDE_H
#define SC_ELM_SIDE_H

#include <sc/elm/types.h>

/**
 * @addtogroup Side
 * @{
 */

SC_API void
sc_side_init(struct ScSide* side, ScPool pool);

/**
 * Levels associated with side are also freed. Assumes that pointer is not null.
 */

SC_API void
sc_side_term(struct ScSide* side);

static inline struct ScOrder*
sc_side_order_entry(struct ScDlNode* node)
{
    return sc_implof(struct ScOrder, side_node_, node);
}

/**
 * Insert order into side. Assumes that the order does not already belong to a side. I.e. it assumes
 * that level member is null. Assumes that order-id and reference are unique.
 *
 * @return false if level allocation fails.
 */

SC_API ScBool
sc_side_insert_order(struct ScSide* side, struct ScOrder* order);

/**
 * Internal housekeeping aside, the state of the order is not affected by this function.
 */

SC_API void
sc_side_remove_order(struct ScSide* side, struct ScOrder* order);

/**
 * Reduce residual lots by lots. If the resulting residual is zero, then the order is removed from
 * the side.
 */

SC_API void
sc_side_take_order(struct ScSide* side, struct ScOrder* order, ScLots lots, ScMillis now);

static inline ScBool
sc_side_place_order(struct ScSide* side, struct ScOrder* order, ScMillis now)
{
    assert(order->i.lots > 0 && order->i.lots >= order->i.min_lots);
    order->i.state = SC_STATE_NEW;
    order->i.resd = order->i.lots;
    order->i.exec = 0;
    order->created = now;
    order->modified = now;
    return sc_side_insert_order(side, order);
}

SC_API void
sc_side_revise_order(struct ScSide* side, struct ScOrder* order, ScLots lots, ScMillis now);

static inline void
sc_side_cancel_order(struct ScSide* side, struct ScOrder* order, ScMillis now)
{
    sc_side_remove_order(side, order);
    order->i.state = SC_STATE_CANCEL;
    // Note that executed lots is not affected.
    order->i.resd = 0;
    order->modified = now;
}

static inline struct ScDlNode*
sc_side_first_order(const struct ScSide* side)
{
    return sc_list_first(&side->orders);
}

static inline struct ScDlNode*
sc_side_last_order(const struct ScSide* side)
{
    return sc_list_last(&side->orders);
}

static inline struct ScDlNode*
sc_side_end_order(const struct ScSide* side)
{
    return sc_list_end(&side->orders);
}

static inline ScBool
sc_side_empty_order(const struct ScSide* side)
{
    return sc_list_empty(&side->orders);
}

#define SC_SIDE_END_LEVEL NULL

static inline struct ScLevel*
sc_side_level_entry(struct ScRbNode* node)
{
    return sc_implof(struct ScLevel, side_node_, node);
}

static inline struct ScRbNode*
sc_side_find_level(const struct ScSide* side, ScIden id)
{
    return sc_tree_find(&side->levels, id);
}

static inline struct ScRbNode*
sc_side_first_level(const struct ScSide* side)
{
    return sc_tree_first(&side->levels);
}

static inline struct ScRbNode*
sc_side_last_level(const struct ScSide* side)
{
    return sc_tree_last(&side->levels);
}

static inline ScBool
sc_side_empty_level(const struct ScSide* side)
{
    return sc_tree_empty(&side->levels);
}

static inline ScTicks
sc_side_last_ticks(const struct ScSide* side)
{
    return side->last_ticks;
}

static inline ScLots
sc_side_last_lots(const struct ScSide* side)
{
    return side->last_lots;
}

static inline ScMillis
sc_side_last_time(const struct ScSide* side)
{
    return side->last_time;
}

/** @} */

#endif // SC_ELM_SIDE_H
