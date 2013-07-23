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
#include "trader.h"

#include "market.h"
#include "pool.h"

#include <dbr/err.h>
#include <dbr/sess.h>

#include <stdlib.h>

static const struct DbrTraderSessVtbl SESS_VTBL = {
};

static struct DbrITraderSess sess_noop = {
    .vtbl = &SESS_VTBL
};

static void
free_orders(struct FigTrader* trader)
{
    assert(trader);
    struct DbrRbNode* node;
    while ((node = trader->orders.root)) {
        struct DbrOrder* order = dbr_trader_order_entry(node);
        fig_trader_release_order(trader, order);
        fig_pool_free_order(trader->pool, order);
    }
}

static void
free_subs(struct FigTrader* trader)
{
    assert(trader);
    struct DbrRbNode* node;
    while ((node = trader->subs.root)) {
        struct DbrSub* sub = dbr_trader_sub_entry(node);
        fig_market_unsub(sub);
        dbr_tree_remove(&trader->subs, node);
        fig_pool_free_sub(trader->pool, sub);
    }
}

DBR_EXTERN struct FigTrader*
fig_trader_lazy(struct DbrRec* trec, struct FigPool* pool, struct FigIndex* index)
{
    assert(trec);
    assert(trec->type == DBR_TRADER);
    struct FigTrader* trader = trec->trader.state;
    if (dbr_unlikely(!trader)) {
        trader = malloc(sizeof(struct FigTrader));
        if (dbr_unlikely(!trader)) {
            dbr_err_set(DBR_ENOMEM, "out of memory");
            return NULL;
        }
        trader->id = trec->id;
        trader->pool = pool;
        trader->index = index;
        dbr_tree_init(&trader->orders);
        dbr_tree_init(&trader->subs);
        trader->sess = &sess_noop;

        trec->trader.state = trader;
    }
    return trader;
}

DBR_EXTERN void
fig_trader_term(struct DbrRec* trec)
{
    assert(trec);
    assert(trec->type == DBR_TRADER);
    struct FigTrader* trader = trec->trader.state;
    if (trader) {
        trec->trader.state = NULL;
        free_subs(trader);
        free_orders(trader);
        free(trader);
    }
}

DBR_EXTERN DbrBool
fig_trader_sub(struct FigTrader* trader, struct FigMarket* market)
{
    struct FigPool* pool = trader->pool;
	struct DbrRbNode* node = dbr_tree_pfind(&trader->subs, market->id);
    if (node && node->key == market->id) {
        dbr_err_set(DBR_EINVAL, "subscription already exists");
        goto fail1;
    }
    struct DbrSub* sub = fig_pool_alloc_sub(pool, market->id);
    if (!sub)
        goto fail1;

    sub->market = market;
    sub->trader = trader;

    struct DbrRbNode* parent = node;
    dbr_tree_pinsert(&trader->subs, &sub->trader_node_, parent);
    fig_market_sub(market, sub);
    return true;

fail1:
    return false;
}

DBR_EXTERN void
fig_trader_unsub(struct FigTrader* trader, DbrIden mrid)
{
    struct DbrRbNode* node = dbr_tree_find(&trader->subs, mrid);
    if (node) {
        struct DbrSub* sub = dbr_trader_sub_entry(node);
        fig_market_unsub(sub);
        dbr_tree_remove(&trader->subs, node);
        fig_pool_free_sub(trader->pool, sub);
    }
}

DBR_EXTERN void
fig_trader_set_sess(struct FigTrader* trader, DbrTraderSess sess)
{
    trader->sess = sess ? sess : &sess_noop;
}

DBR_API DbrIden
dbr_trader_id(DbrTrader trader)
{
    return fig_trader_id(trader);
}

// TraderOrder

DBR_API struct DbrRbNode*
dbr_trader_find_order_id(DbrTrader trader, DbrIden id)
{
    return fig_trader_find_order_id(trader, id);
}

DBR_API struct DbrOrder*
dbr_trader_find_order_ref(DbrTrader trader, const char* ref)
{
    return fig_trader_find_order_ref(trader, ref);
}

DBR_API struct DbrRbNode*
dbr_trader_first_order(DbrTrader trader)
{
    return fig_trader_first_order(trader);
}

DBR_API struct DbrRbNode*
dbr_trader_last_order(DbrTrader trader)
{
    return fig_trader_last_order(trader);
}

DBR_API struct DbrRbNode*
dbr_trader_end_order(DbrTrader trader)
{
    return fig_trader_end_order(trader);
}

DBR_API DbrBool
dbr_trader_empty_order(DbrTrader trader)
{
    return fig_trader_empty_order(trader);
}

// TraderSub

DBR_API DbrBool
dbr_trader_sub(DbrTrader trader, DbrMarket market)
{
    return fig_trader_sub(trader, market);
}

DBR_API void
dbr_trader_unsub(DbrTrader trader, DbrIden mrid)
{
    return fig_trader_unsub(trader, mrid);
}

// TraderSess

DBR_API void
dbr_trader_set_sess(DbrTrader trader, DbrTraderSess sess)
{
    fig_trader_set_sess(trader, sess);
}

DBR_API DbrTraderSess
dbr_trader_sess(DbrTrader trader)
{
    return fig_trader_sess(trader);
}
