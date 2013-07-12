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
#ifndef ELM_TRADER_H
#define ELM_TRADER_H

#include <dbr/conv.h>

#include <elm/index.h>

#include <dbr/tree.h>

#include <assert.h>

struct ElmTrader {
    DbrIden id;
    struct ElmPool* pool;
    struct ElmIndex* index;
    struct DbrTree orders;
    struct DbrTree subs;
    DbrTraderSess sess;
};

DBR_EXTERN struct ElmTrader*
elm_trader_lazy(struct DbrRec* trec, struct ElmPool* pool, struct ElmIndex* index);

// Assumes that trec pointer is not null.

DBR_EXTERN void
elm_trader_term(struct DbrRec* trec);

static inline DbrIden
elm_trader_id(struct ElmTrader* trader)
{
    return trader->id;
}

// Order.

// Transfer ownership to state.

static inline void
elm_trader_emplace_order(struct ElmTrader* trader, struct DbrOrder* order)
{
    dbr_tree_insert(&trader->orders, &order->trader_node_);
    if (order->ref[0] != '\0')
        elm_index_insert(trader->index, order);
}

// Release ownership from state.

static inline void
elm_trader_release_order(struct ElmTrader* trader, struct DbrOrder* order)
{
    dbr_tree_remove(&trader->orders, &order->trader_node_);
    if (order->ref[0] != '\0')
        elm_index_remove(trader->index, trader->id, order->ref);
}

// Release ownership from state.

static inline struct DbrOrder*
elm_trader_release_order_id(struct ElmTrader* trader, DbrIden id)
{
    struct DbrRbNode* node = dbr_tree_find(&trader->orders, id);
    if (!node)
        return NULL;
    struct DbrOrder* order = dbr_trader_order_entry(node);
    elm_trader_release_order(trader, order);
    return order;
}

// Release ownership from state.

static inline struct DbrOrder*
elm_trader_release_order_ref(struct ElmTrader* trader, const char* ref)
{
    assert(ref);
    struct DbrOrder* order = elm_index_remove(trader->index, trader->id, ref);
    if (order)
        dbr_tree_remove(&trader->orders, &order->trader_node_);
    return order;
}

static inline struct DbrRbNode*
elm_trader_find_order_id(const struct ElmTrader* trader, DbrIden id)
{
    return dbr_tree_find(&trader->orders, id);
}

// Returns order directly because hash lookup is not a node-based container.

static inline struct DbrOrder*
elm_trader_find_order_ref(const struct ElmTrader* trader, const char* ref)
{
    assert(ref);
    return elm_index_find(trader->index, trader->id, ref);
}

static inline struct DbrRbNode*
elm_trader_first_order(const struct ElmTrader* trader)
{
    return dbr_tree_first(&trader->orders);
}

static inline struct DbrRbNode*
elm_trader_last_order(const struct ElmTrader* trader)
{
    return dbr_tree_last(&trader->orders);
}

static inline struct DbrRbNode*
elm_trader_end_order(const struct ElmTrader* trader)
{
    return dbr_tree_end(&trader->orders);
}

static inline DbrBool
elm_trader_empty_order(const struct ElmTrader* trader)
{
    return dbr_tree_empty(&trader->orders);
}

// Sub.

DBR_EXTERN DbrBool
elm_trader_sub(struct ElmTrader* trader, struct ElmMarket* market);

DBR_EXTERN void
elm_trader_unsub(struct ElmTrader* trader, DbrIden mrid);

DBR_EXTERN void
elm_trader_set_sess(struct ElmTrader* trader, DbrTraderSess sess);

static inline DbrTraderSess
elm_trader_sess(struct ElmTrader* trader)
{
    return trader->sess;
}

#endif // ELM_TRADER_H
