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
#ifndef FIG_MARKET_H
#define FIG_MARKET_H

#include <fig/side.h>

struct FigMarket {
    DbrIden id;
    struct FigSide bid_side;
    struct FigSide ask_side;
    struct DbrList subs;
};

static inline struct FigSide*
fig_market_side(struct FigMarket* market, int action)
{
    return action == DBR_BUY ? &market->bid_side : &market->ask_side;
}

DBR_EXTERN struct FigMarket*
fig_market_lazy(struct DbrRec* mrec, DbrPool pool);

// Assumes that mrec pointer is not null.

DBR_EXTERN void
fig_market_term(struct DbrRec* mrec);

DBR_EXTERN struct DbrBest*
fig_market_best(struct DbrRec* mrec, struct DbrBest* best);

static inline DbrBool
fig_market_insert(struct FigMarket* market, struct DbrOrder* order)
{
    return fig_side_insert_order(fig_market_side(market, order->action), order);
}

static inline void
fig_market_remove(struct FigMarket* market, struct DbrOrder* order)
{
    assert(order);
    fig_side_remove_order(fig_market_side(market, order->action), order);
}

static inline void
fig_market_take(struct FigMarket* market, struct DbrOrder* order, DbrLots delta, DbrMillis now)
{
    fig_side_take_order(fig_market_side(market, order->action), order, delta, now);
}

static inline DbrBool
fig_market_revise(struct FigMarket* market, struct DbrOrder* order, DbrLots lots, DbrMillis now)
{
    return fig_side_revise_order(fig_market_side(market, order->action), order, lots, now);
}

static inline void
fig_market_cancel(struct FigMarket* market, struct DbrOrder* order, DbrMillis now)
{
    assert(order);
    fig_side_cancel_order(fig_market_side(market, order->action), order, now);
}

static inline void
fig_market_sub(struct FigMarket* market, struct DbrSub* sub)
{
    dbr_list_insert_back(&market->subs, &sub->market_node_);
}

static inline void
fig_market_unsub(struct DbrSub* sub)
{
    dbr_dlnode_remove(&sub->market_node_);
}

static inline DbrIden
fig_market_id(struct FigMarket* market)
{
    return market->id;
}

static inline struct FigSide*
fig_market_bid_side(struct FigMarket* market)
{
    return &market->bid_side;
}

static inline struct FigSide*
fig_market_ask_side(struct FigMarket* market)
{
    return &market->ask_side;
}

#endif // FIG_MARKET_H
