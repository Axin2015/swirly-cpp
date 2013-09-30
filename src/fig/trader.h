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
#ifndef FIG_TRADER_H
#define FIG_TRADER_H

#include <fig/index.h>

#include <dbr/pool.h>
#include <dbr/trader.h>
#include <dbr/tree.h>

#include <assert.h>

struct FigTrader {
    DbrIden id;
    struct FigIndex* index;
    DbrPool pool;
    struct DbrTree orders;
    struct DbrTree subs;
};

DBR_EXTERN struct FigTrader*
fig_trader_lazy(struct DbrRec* trec, struct FigIndex* index, DbrPool pool);

// Assumes that trec pointer is not null.

DBR_EXTERN void
fig_trader_term(struct DbrRec* trec);

static inline DbrIden
fig_trader_id(struct FigTrader* trader)
{
    return trader->id;
}

// Order.

// Transfer ownership to state.

static inline void
fig_trader_emplace_order(struct FigTrader* trader, struct DbrOrder* order)
{
    dbr_tree_insert(&trader->orders, order->id, &order->trader_node_);
    if (order->ref[0] != '\0')
        fig_index_insert(trader->index, order);
}

// Release ownership from state.

static inline void
fig_trader_release_order(struct FigTrader* trader, struct DbrOrder* order)
{
    dbr_tree_remove(&trader->orders, &order->trader_node_);
    dbr_rbnode_init(&order->trader_node_);
    if (order->ref[0] != '\0')
        fig_index_remove(trader->index, trader->id, order->ref);
}

// Release ownership from state.

static inline struct DbrOrder*
fig_trader_release_order_id(struct FigTrader* trader, DbrIden id)
{
    struct DbrRbNode* node = dbr_tree_find(&trader->orders, id);
    if (!node)
        return NULL;
    struct DbrOrder* order = dbr_trader_order_entry(node);
    fig_trader_release_order(trader, order);
    return order;
}

// Release ownership from state.

static inline struct DbrOrder*
fig_trader_release_order_ref(struct FigTrader* trader, const char* ref)
{
    assert(ref);
    struct DbrOrder* order = fig_index_remove(trader->index, trader->id, ref);
    if (order) {
        dbr_tree_remove(&trader->orders, &order->trader_node_);
        dbr_rbnode_init(&order->trader_node_);
    }
    return order;
}

static inline struct DbrRbNode*
fig_trader_find_order_id(const struct FigTrader* trader, DbrIden id)
{
    return dbr_tree_find(&trader->orders, id);
}

// Returns order directly because hash lookup is not a node-based container.

static inline struct DbrOrder*
fig_trader_find_order_ref(const struct FigTrader* trader, const char* ref)
{
    assert(ref);
    return fig_index_find(trader->index, trader->id, ref);
}

static inline struct DbrRbNode*
fig_trader_first_order(const struct FigTrader* trader)
{
    return dbr_tree_first(&trader->orders);
}

static inline struct DbrRbNode*
fig_trader_last_order(const struct FigTrader* trader)
{
    return dbr_tree_last(&trader->orders);
}

static inline struct DbrRbNode*
fig_trader_end_order(const struct FigTrader* trader)
{
    return dbr_tree_end(&trader->orders);
}

static inline DbrBool
fig_trader_empty_order(const struct FigTrader* trader)
{
    return dbr_tree_empty(&trader->orders);
}

// Sub.

DBR_EXTERN DbrBool
fig_trader_sub(struct FigTrader* trader, struct DbrBook* book);

DBR_EXTERN void
fig_trader_unsub(struct FigTrader* trader, struct DbrBook* book);

#endif // FIG_TRADER_H
