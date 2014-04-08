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
#ifndef FIG_TRADER_H
#define FIG_TRADER_H

#include <dbr/trader.h>

#include "ordidx.h"

#include <dbr/refcount.h>
#include <dbr/tree.h>

#include <assert.h>

struct FigTrader {
    struct DbrRec* rec;
    struct FigOrdIdx* ordidx;
    DbrPool pool;
    struct DbrTree orders;
    struct DbrTree trades;
    struct DbrTree perms;
    struct DbrSess* sess;
    // Private section.
    struct DbrRbNode sess_node_;
};

DBR_EXTERN struct FigTrader*
fig_trader_lazy(struct DbrRec* trec, struct FigOrdIdx* ordidx, DbrPool pool);

// Assumes that trec pointer is not null.

DBR_EXTERN void
fig_trader_term(struct DbrRec* trec);

DBR_EXTERN void
fig_trader_clear(struct FigTrader* trader);

static inline struct DbrRec*
fig_trader_rec(struct FigTrader* trader)
{
    return trader->rec;
}

// Order.

#define FIG_TRADER_END_ORDER DBR_TREE_END

// Transfer ownership to state.

static inline void
fig_trader_emplace_order(struct FigTrader* trader, struct DbrOrder* order)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct DbrRbNode* node = dbr_tree_insert(&trader->orders, order->id, &order->trader_node_);
        assert(node == &order->trader_node_);
    }
#pragma GCC diagnostic pop
    if (order->c.ref[0] != '\0')
        fig_ordidx_insert(trader->ordidx, order);
}

// Release ownership from state.

static inline void
fig_trader_release_order(struct FigTrader* trader, struct DbrOrder* order)
{
    dbr_tree_remove(&trader->orders, &order->trader_node_);
    dbr_rbnode_init(&order->trader_node_);
    if (order->c.ref[0] != '\0')
        fig_ordidx_remove(trader->ordidx, trader->rec->id, order->c.ref);
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
    struct DbrOrder* order = fig_ordidx_remove(trader->ordidx, trader->rec->id, ref);
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
    return fig_ordidx_find(trader->ordidx, trader->rec->id, ref);
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

static inline DbrBool
fig_trader_empty_order(const struct FigTrader* trader)
{
    return dbr_tree_empty(&trader->orders);
}

// Trade.

#define FIG_TRADER_END_TRADE DBR_TREE_END

static inline void
fig_trader_insert_trade(struct FigTrader* trader, struct DbrExec* exec)
{
    dbr_exec_incref(exec);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct DbrRbNode* node = dbr_tree_insert(&trader->trades, exec->id, &exec->trader_node_);
        assert(node == &exec->trader_node_);
    }
#pragma GCC diagnostic pop
}

static inline void
fig_trader_remove_trade(struct FigTrader* trader, struct DbrExec* exec)
{
    dbr_tree_remove(&trader->trades, &exec->trader_node_);
    dbr_rbnode_init(&exec->trader_node_);
    dbr_exec_decref(exec, trader->pool);
}

// Release ownership from state.

static inline DbrBool
fig_trader_remove_trade_id(struct FigTrader* trader, DbrIden id)
{
    struct DbrRbNode* node = dbr_tree_find(&trader->trades, id);
    if (!node)
        return DBR_FALSE;

    struct DbrExec* exec = dbr_trader_trade_entry(node);
    fig_trader_remove_trade(trader, exec);
    return DBR_TRUE;
}

static inline struct DbrRbNode*
fig_trader_find_trade_id(const struct FigTrader* trader, DbrIden id)
{
    return dbr_tree_find(&trader->trades, id);
}

static inline struct DbrRbNode*
fig_trader_first_trade(const struct FigTrader* trader)
{
    return dbr_tree_first(&trader->trades);
}

static inline struct DbrRbNode*
fig_trader_last_trade(const struct FigTrader* trader)
{
    return dbr_tree_last(&trader->trades);
}

static inline DbrBool
fig_trader_empty_trade(const struct FigTrader* trader)
{
    return dbr_tree_empty(&trader->trades);
}

// Perm.

#define FIG_TRADER_END_PERM DBR_TREE_END

// Transfer ownership to state.

static inline void
fig_trader_emplace_perm(struct FigTrader* trader, struct DbrMemb* memb)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct DbrRbNode* node = dbr_tree_insert(&trader->perms, memb->accnt.rec->id,
                                                 &memb->trader_node_);
        assert(node == &memb->trader_node_);
    }
#pragma GCC diagnostic pop
}

static inline struct DbrRbNode*
fig_trader_find_perm_id(const struct FigTrader* trader, DbrIden id)
{
    return dbr_tree_find(&trader->perms, id);
}

static inline struct DbrRbNode*
fig_trader_first_perm(const struct FigTrader* trader)
{
    return dbr_tree_first(&trader->perms);
}

static inline struct DbrRbNode*
fig_trader_last_perm(const struct FigTrader* trader)
{
    return dbr_tree_last(&trader->perms);
}

static inline DbrBool
fig_trader_empty_perm(const struct FigTrader* trader)
{
    return dbr_tree_empty(&trader->perms);
}

static inline DbrBool
fig_trader_logged_on(const struct FigTrader* trader)
{
    return trader->sess != NULL;
}

static inline struct DbrSess*
fig_trader_sess(const struct FigTrader* trader)
{
    return trader->sess;
}

#endif // FIG_TRADER_H
