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

#include <dbr/book.h>
#include <dbr/err.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static void
free_orders(struct FigTrader* trader)
{
    assert(trader);
    struct DbrRbNode* node;
    while ((node = trader->orders.root)) {
        struct DbrOrder* order = dbr_trader_order_entry(node);
        fig_trader_release_order(trader, order);
        dbr_pool_free_order(trader->pool, order);
    }
}

static void
free_execs(struct FigTrader* trader)
{
    assert(trader);
    struct DbrRbNode* node;
    while ((node = trader->trades.root)) {
        struct DbrExec* exec = dbr_trader_trade_entry(node);
        dbr_tree_remove(&trader->trades, node);
        dbr_pool_free_exec(trader->pool, exec);
    }
}

static void
free_membs(struct FigTrader* trader)
{
    assert(trader);
    struct DbrRbNode* node;
    while ((node = trader->membs.root)) {
        struct DbrMemb* memb = dbr_trader_memb_entry(node);
        dbr_tree_remove(&trader->membs, node);
        dbr_pool_free_memb(trader->pool, memb);
    }
}

DBR_EXTERN struct FigTrader*
fig_trader_lazy(struct DbrRec* trec, struct FigIndex* index, DbrPool pool)
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
        trader->rec = trec;
        trader->index = index;
        trader->pool = pool;
        dbr_tree_init(&trader->orders);
        dbr_tree_init(&trader->trades);
        dbr_tree_init(&trader->membs);

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
        free_membs(trader);
        free_execs(trader);
        free_orders(trader);
        free(trader);
    }
}

DBR_EXTERN struct DbrOrder*
fig_trader_update_order(struct FigTrader* trader, struct DbrOrder* order)
{
    struct DbrRbNode* node = dbr_tree_insert(&trader->orders, order->id, &order->trader_node_);
    if (node) {
        struct DbrOrder* curr = dbr_trader_order_entry(node);

        // Update existing order.

        assert(curr->id == order->id);
        assert(curr->c.trader.rec == order->c.trader.rec);
        assert(curr->c.accnt.rec == order->c.accnt.rec);
        assert(curr->c.contr.rec == order->c.contr.rec);
        assert(curr->c.settl_date == order->c.settl_date);
        assert(strncmp(curr->c.ref, order->c.ref, DBR_REF_MAX) == 0);
        curr->c.status = order->c.status;
        assert(curr->c.action == order->c.action);
        assert(curr->c.ticks == order->c.ticks);
        curr->c.lots = order->c.lots;
        curr->c.resd = order->c.resd;
        curr->c.exec = order->c.exec;
        curr->c.last_lots = order->c.last_lots;
        curr->c.last_ticks = order->c.last_ticks;
        assert(curr->c.min_lots == order->c.min_lots);
        assert(curr->created == order->created);
        curr->modified = order->modified;

        dbr_pool_free_order(trader->pool, order);
        order = curr;
    } else if (order->c.ref[0] != '\0')
        fig_index_insert(trader->index, order);

    return order;
}

DBR_API struct DbrRec*
dbr_trader_rec(DbrTrader trader)
{
    return fig_trader_rec(trader);
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

DBR_API DbrBool
dbr_trader_empty_order(DbrTrader trader)
{
    return fig_trader_empty_order(trader);
}

// TraderTrade

DBR_API struct DbrRbNode*
dbr_trader_find_trade_id(DbrTrader trader, DbrIden id)
{
    return fig_trader_find_trade_id(trader, id);
}

DBR_API struct DbrRbNode*
dbr_trader_first_trade(DbrTrader trader)
{
    return fig_trader_first_trade(trader);
}

DBR_API struct DbrRbNode*
dbr_trader_last_trade(DbrTrader trader)
{
    return fig_trader_last_trade(trader);
}

DBR_API DbrBool
dbr_trader_empty_trade(DbrTrader trader)
{
    return fig_trader_empty_trade(trader);
}

// TraderMemb

DBR_API struct DbrRbNode*
dbr_trader_find_memb_id(DbrTrader trader, DbrIden id)
{
    return fig_trader_find_memb_id(trader, id);
}

DBR_API struct DbrRbNode*
dbr_trader_first_memb(DbrTrader trader)
{
    return fig_trader_first_memb(trader);
}

DBR_API struct DbrRbNode*
dbr_trader_last_memb(DbrTrader trader)
{
    return fig_trader_last_memb(trader);
}

DBR_API DbrBool
dbr_trader_empty_memb(DbrTrader trader)
{
    return fig_trader_empty_memb(trader);
}
