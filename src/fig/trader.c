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
free_trades(struct FigTrader* trader)
{
    assert(trader);
    struct DbrRbNode* node;
    while ((node = trader->trades.root)) {
        struct DbrTrade* trade = dbr_trader_trade_entry(node);
        dbr_tree_remove(&trader->trades, node);
        dbr_pool_free_trade(trader->pool, trade);
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
        free_orders(trader);
        free_trades(trader);
        free(trader);
    }
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
