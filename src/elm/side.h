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
#ifndef ELM_SIDE_H
#define ELM_SIDE_H

#include <dbr/types.h>

#include <ash/list.h>
#include <ash/tree.h>

#include <assert.h>
#include <stdbool.h>

struct ElmSide {
    struct ElmCtx* ctx;
    struct AshTree levels;
    struct AshList orders;
    // Last trade information.
    DbrTicks last_ticks;
    DbrLots last_lots;
    DbrMillis last_time;
};

DBR_EXTERN void
elm_side_init(struct ElmSide* side, struct ElmCtx* ctx);

// Levels associated with side are also freed.
// Assumes that pointer is not null.

DBR_EXTERN void
elm_side_term(struct ElmSide* side);

// Insert order without affecting revision information.
// Assumes that order does not already belong to a side. I.e. it assumes that level is null.
// Assumes that order-id and party-ref (if ref is not empty) are unique. The elm_side_find_ref() can
// be used to detect duplicate party-refs. Returns false if level allocation fails.

DBR_EXTERN DbrBool
elm_side_insert_order(struct ElmSide* side, struct DbrOrder* order);

// Internal housekeeping aside, the state of the order is not affected by this function.

DBR_EXTERN void
elm_side_remove_order(struct ElmSide* side, struct DbrOrder* order);

// Reduce residual lots by delta. If the resulting residual is zero, then the order is removed from
// the side.

DBR_EXTERN void
elm_side_take_order(struct ElmSide* side, struct DbrOrder* order, DbrLots delta, DbrMillis now);

DBR_EXTERN DbrBool
elm_side_revise_order(struct ElmSide* side, struct DbrOrder* order, DbrLots lots, DbrMillis now);

static inline void
elm_side_cancel_order(struct ElmSide* side, struct DbrOrder* order, DbrMillis now)
{
    elm_side_remove_order(side, order);
    ++order->rev;
    order->status = DBR_CANCELLED;
    // Note that executed lots is not affected.
    order->resd = 0;
    order->modified = now;
}

// Side Order.

static inline struct DbrDlNode*
elm_side_first_order(const struct ElmSide* side)
{
    return ash_list_first(&side->orders);
}

static inline struct DbrDlNode*
elm_side_last_order(const struct ElmSide* side)
{
    return ash_list_last(&side->orders);
}

static inline struct DbrDlNode*
elm_side_end_order(const struct ElmSide* side)
{
    return ash_list_end(&side->orders);
}

static inline DbrBool
elm_side_empty_order(const struct ElmSide* side)
{
    return ash_list_empty(&side->orders);
}

// Side Level.

static inline struct DbrRbNode*
elm_side_find_level(const struct ElmSide* side, DbrIden id)
{
    return ash_tree_find(&side->levels, id);
}

static inline struct DbrRbNode*
elm_side_first_level(const struct ElmSide* side)
{
    return ash_tree_first(&side->levels);
}

static inline struct DbrRbNode*
elm_side_last_level(const struct ElmSide* side)
{
    return ash_tree_last(&side->levels);
}

static inline struct DbrRbNode*
elm_side_end_level(const struct ElmSide* side)
{
    return ash_tree_end(&side->levels);
}

static inline DbrBool
elm_side_empty_level(const struct ElmSide* side)
{
    return ash_tree_empty(&side->levels);
}

static inline DbrTicks
elm_side_last_ticks(const struct ElmSide* side)
{
    return side->last_ticks;
}

static inline DbrLots
elm_side_last_lots(const struct ElmSide* side)
{
    return side->last_lots;
}

static inline DbrMillis
elm_side_last_time(const struct ElmSide* side)
{
    return side->last_time;
}

#endif // ELM_SIDE_H
