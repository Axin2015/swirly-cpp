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
#ifndef ELM_MARKET_H
#define ELM_MARKET_H

#include <elm/side.h>

struct ElmMarket {
    DbrIden id;
    struct ElmSide bid_side;
    struct ElmSide ask_side;
    struct AshList subs;
};

static inline struct ElmSide*
elm_market_side(struct ElmMarket* market, int action)
{
    return action == DBR_BUY ? &market->bid_side : &market->ask_side;
}

DBR_EXTERN struct ElmMarket*
elm_market_lazy(struct DbrRec* mrec, struct ElmPool* pool);

// Assumes that mrec pointer is not null.

DBR_EXTERN void
elm_market_term(struct DbrRec* mrec);

DBR_EXTERN struct DbrBest*
elm_market_best(struct DbrRec* mrec, struct DbrBest* best);

static inline DbrBool
elm_market_insert(struct ElmMarket* market, struct DbrOrder* order)
{
    return elm_side_insert_order(elm_market_side(market, order->action), order);
}

static inline void
elm_market_remove(struct ElmMarket* market, struct DbrOrder* order)
{
    assert(order);
    elm_side_remove_order(elm_market_side(market, order->action), order);
}

static inline void
elm_market_take(struct ElmMarket* market, struct DbrOrder* order, DbrLots delta, DbrMillis now)
{
    elm_side_take_order(elm_market_side(market, order->action), order, delta, now);
}

static inline DbrBool
elm_market_revise(struct ElmMarket* market, struct DbrOrder* order, DbrLots lots, DbrMillis now)
{
    return elm_side_revise_order(elm_market_side(market, order->action), order, lots, now);
}

static inline void
elm_market_cancel(struct ElmMarket* market, struct DbrOrder* order, DbrMillis now)
{
    assert(order);
    elm_side_cancel_order(elm_market_side(market, order->action), order, now);
}

static inline void
elm_market_sub(struct ElmMarket* market, struct DbrSub* sub)
{
    ash_list_insert_back(&market->subs, &sub->market_node_);
}

static inline void
elm_market_unsub(struct DbrSub* sub)
{
    ash_dlnode_remove(&sub->market_node_);
}

static inline DbrIden
elm_market_id(struct ElmMarket* market)
{
    return market->id;
}

static inline struct ElmSide*
elm_market_bid_side(struct ElmMarket* market)
{
    return &market->bid_side;
}

static inline struct ElmSide*
elm_market_ask_side(struct ElmMarket* market)
{
    return &market->ask_side;
}

#endif // ELM_MARKET_H
