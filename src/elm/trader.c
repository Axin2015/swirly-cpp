/**
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

#include "ctx.h"
#include "market.h"

#include <dbr/err.h>
#include <dbr/sess.h>

#include <stdlib.h>

static const struct DbrTraderSessVtbl SESS_VTBL = {
};

static struct DbrITraderSess sess_noop = {
    .vtbl = &SESS_VTBL
};

static void
free_orders(struct ElmTrader* trader)
{
    assert(trader);
    struct DbrRbNode* node;
    while ((node = trader->orders.root)) {
        struct DbrOrder* order = dbr_trader_order_entry(node);
        elm_trader_release_order(trader, order);
        elm_ctx_free_order(trader->ctx, order);
    }
}

static void
free_subs(struct ElmTrader* trader)
{
    assert(trader);
    struct DbrRbNode* node;
    while ((node = trader->subs.root)) {
        struct DbrSub* sub = dbr_trader_sub_entry(node);
        elm_market_unsub(sub);
        ash_tree_remove(&trader->subs, node);
        elm_ctx_free_sub(trader->ctx, sub);
    }
}

DBR_EXTERN struct ElmTrader*
elm_trader_lazy(struct DbrRec* trec, struct ElmCtx* ctx, struct ElmIndex* index)
{
    assert(trec);
    assert(trec->type == DBR_TRADER);
    struct ElmTrader* trader = trec->trader.state;
    if (dbr_unlikely(!trader)) {
        trader = malloc(sizeof(struct ElmTrader));
        if (dbr_unlikely(!trader)) {
            dbr_err_set(&ctx->err, DBR_ENOMEM, "out of memory");
            return NULL;
        }
        trader->id = trec->id;
        trader->ctx = ctx;
        trader->index = index;
        ash_tree_init(&trader->orders);
        ash_tree_init(&trader->subs);
        trader->sess = &sess_noop;

        trec->trader.state = trader;
    }
    return trader;
}

DBR_EXTERN void
elm_trader_term(struct DbrRec* trec)
{
    assert(trec);
    assert(trec->type == DBR_TRADER);
    struct ElmTrader* trader = trec->trader.state;
    if (trader) {
        trec->trader.state = NULL;
        free_subs(trader);
        free_orders(trader);
        free(trader);
    }
}

DBR_EXTERN DbrBool
elm_trader_sub(struct ElmTrader* trader, struct ElmMarket* market)
{
    struct ElmCtx* ctx = trader->ctx;
	struct DbrRbNode* node = ash_tree_pfind(&trader->subs, market->id);
    if (node && node->key == market->id) {
        dbr_err_set(&ctx->err, DBR_EINVAL, "subscription already exists");
        goto fail1;
    }
    struct DbrSub* sub = elm_ctx_alloc_sub(ctx, market->id);
    if (!sub)
        goto fail1;

    sub->market = market;
    sub->trader = trader;

    struct DbrRbNode* parent = node;
    ash_tree_pinsert(&trader->subs, &sub->trader_node_, parent);
    elm_market_sub(market, sub);
    return true;

fail1:
    return false;
}

DBR_EXTERN void
elm_trader_unsub(struct ElmTrader* trader, DbrIden mrid)
{
    struct DbrRbNode* node = ash_tree_find(&trader->subs, mrid);
    if (node) {
        struct DbrSub* sub = dbr_trader_sub_entry(node);
        elm_market_unsub(sub);
        ash_tree_remove(&trader->subs, node);
        elm_ctx_free_sub(trader->ctx, sub);
    }
}

DBR_EXTERN void
elm_trader_set_sess(struct ElmTrader* trader, DbrTraderSess sess)
{
    trader->sess = sess ? sess : &sess_noop;
}
