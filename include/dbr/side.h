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
#ifndef DBR_SIDE_H
#define DBR_SIDE_H

#include <dbr/list.h>
#include <dbr/pool.h>
#include <dbr/tree.h>
#include <dbr/types.h>

/**
 * @addtogroup Side
 * @{
 */

struct DbrSide {
    DbrPool pool;
    struct DbrTree levels;
    struct DbrList orders;
    // Last trade information.
    DbrTicks last_ticks;
    DbrLots last_lots;
    DbrMillis last_time;
};

DBR_API void
dbr_side_init(struct DbrSide* side, DbrPool pool);

// Levels associated with side are also freed.
// Assumes that pointer is not null.

DBR_API void
dbr_side_term(struct DbrSide* side);

/** @} */

/**
 * @addtogroup SideOrder
 * @{
 */

static inline struct DbrOrder*
dbr_side_order_entry(struct DbrDlNode* node)
{
    return dbr_implof(struct DbrOrder, side_node_, node);
}

/** @} */

// Insert order without affecting revision information.
// Assumes that order does not already belong to a side. I.e. it assumes that level is null.
// Assumes that order-id and party-ref (if ref is not empty) are unique. The dbr_side_find_ref() can
// be used to detect duplicate party-refs. Returns false if level allocation fails.

DBR_API DbrBool
dbr_side_insert_order(struct DbrSide* side, struct DbrOrder* order);

// Internal housekeeping aside, the state of the order is not affected by this function.

DBR_API void
dbr_side_remove_order(struct DbrSide* side, struct DbrOrder* order);

// Reduce residual lots by lots. If the resulting residual is zero, then the order is removed from
// the side.

DBR_API void
dbr_side_take_order(struct DbrSide* side, struct DbrOrder* order, DbrLots lots, DbrMillis now);

static inline DbrBool
dbr_side_place_order(struct DbrSide* side, struct DbrOrder* order, DbrMillis now)
{
    assert(order->c.lots > 0 && order->c.lots >= order->c.min_lots);
    order->c.state = DBR_STATE_NEW;
    order->c.resd = order->c.lots;
    order->c.exec = 0;
    order->created = now;
    order->modified = now;
    return dbr_side_insert_order(side, order);
}

DBR_API void
dbr_side_revise_order(struct DbrSide* side, struct DbrOrder* order, DbrLots lots, DbrMillis now);

static inline void
dbr_side_cancel_order(struct DbrSide* side, struct DbrOrder* order, DbrMillis now)
{
    dbr_side_remove_order(side, order);
    order->c.state = DBR_STATE_CANCEL;
    // Note that executed lots is not affected.
    order->c.resd = 0;
    order->modified = now;
}

static inline struct DbrDlNode*
dbr_side_first_order(const struct DbrSide* side)
{
    return dbr_list_first(&side->orders);
}

static inline struct DbrDlNode*
dbr_side_last_order(const struct DbrSide* side)
{
    return dbr_list_last(&side->orders);
}

static inline struct DbrDlNode*
dbr_side_end_order(const struct DbrSide* side)
{
    return dbr_list_end(&side->orders);
}

static inline DbrBool
dbr_side_empty_order(const struct DbrSide* side)
{
    return dbr_list_empty(&side->orders);
}

/** @} */

/**
 * @addtogroup SideLevel
 * @{
 */

#define DBR_SIDE_END_LEVEL NULL

static inline struct DbrLevel*
dbr_side_level_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrLevel, side_node_, node);
}

static inline struct DbrRbNode*
dbr_side_find_level(const struct DbrSide* side, DbrIden id)
{
    return dbr_tree_find(&side->levels, id);
}

static inline struct DbrRbNode*
dbr_side_first_level(const struct DbrSide* side)
{
    return dbr_tree_first(&side->levels);
}

static inline struct DbrRbNode*
dbr_side_last_level(const struct DbrSide* side)
{
    return dbr_tree_last(&side->levels);
}

static inline DbrBool
dbr_side_empty_level(const struct DbrSide* side)
{
    return dbr_tree_empty(&side->levels);
}

/** @} */

/**
 * @addtogroup SideLast
 * @{
 */

static inline DbrTicks
dbr_side_last_ticks(const struct DbrSide* side)
{
    return side->last_ticks;
}

static inline DbrLots
dbr_side_last_lots(const struct DbrSide* side)
{
    return side->last_lots;
}

static inline DbrMillis
dbr_side_last_time(const struct DbrSide* side)
{
    return side->last_time;
}

/** @} */

#endif // DBR_SIDE_H
