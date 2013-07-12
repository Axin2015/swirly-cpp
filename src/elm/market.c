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
#include "market.h"

#include "pool.h"

#include <dbr/conv.h>
#include <dbr/err.h>

#include <stdlib.h>

DBR_EXTERN struct ElmMarket*
elm_market_lazy(struct DbrRec* mrec, struct ElmPool* pool)
{
    assert(mrec);
    assert(mrec->type == DBR_MARKET);
    struct ElmMarket* market = mrec->market.state;
    if (dbr_unlikely(!market)) {
        market = malloc(sizeof(struct ElmMarket));
        if (dbr_unlikely(!market)) {
            dbr_err_set(DBR_ENOMEM, "out of memory");
            return NULL;
        }
        market->id = mrec->id;
        elm_side_init(&market->bid_side, pool);
        elm_side_init(&market->ask_side, pool);
        dbr_list_init(&market->subs);
        mrec->market.state = market;
    }
    return market;
}

DBR_EXTERN void
elm_market_term(struct DbrRec* mrec)
{
    assert(mrec);
    assert(mrec->type == DBR_MARKET);
    struct ElmMarket* market = mrec->market.state;
    if (market) {
        mrec->market.state = NULL;
        elm_side_term(&market->bid_side);
        elm_side_term(&market->ask_side);
        free(market);
    }
}

DBR_EXTERN struct DbrBest*
elm_market_best(struct DbrRec* mrec, struct DbrBest* best)
{
    DbrMarket market = mrec->market.state;
    if (market) {
        struct ElmSide* side = &market->bid_side;
        struct DbrRbNode* it = elm_side_first_level(side);
        struct DbrRbNode* end = elm_side_end_level(side);
        if (it != end) {
            struct DbrLevel* level = dbr_side_level_entry(it);
            best->bid_ticks = level->ticks;
            best->bid_resd = level->resd;
        } else {
            best->bid_ticks = 0;
            best->bid_resd = 0;
        }
        side = &market->ask_side;
        it = elm_side_first_level(side);
        end = elm_side_end_level(side);
        if (it != end) {
            struct DbrLevel* level = dbr_side_level_entry(it);
            best->ask_ticks = level->ticks;
            best->ask_resd = level->resd;
        } else {
            best->ask_ticks = 0;
            best->ask_resd = 0;
        }
    } else {
        best->bid_ticks = 0;
        best->bid_resd = 0;
        best->ask_ticks = 0;
        best->ask_resd = 0;
    }
    return best;
}

DBR_API DbrIden
dbr_market_id(DbrMarket market)
{
    return elm_market_id(market);
}

DBR_API DbrSide
dbr_market_bid_side(DbrMarket market)
{
    return elm_market_bid_side(market);
}

DBR_API DbrSide
dbr_market_ask_side(DbrMarket market)
{
    return elm_market_ask_side(market);
}

DBR_API struct DbrBest*
dbr_market_best(struct DbrRec* mrec, struct DbrBest* best)
{
    return elm_market_best(mrec, best);
}
