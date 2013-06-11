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
#include "match.h"

#include "accnt.h"
#include "ctx.h"
#include "market.h"

#include <ash/queue.h>

#include <dbr/conv.h>
#include <dbr/model.h>

#include <stdbool.h>
#include <string.h>

// Trades are considered positive or negative depending on whether the aggressor is buying or
// selling.

enum DbrDirect {
    // Aggressor buys.
    DBR_PAID = 1,
    // Aggressor sells.
    DBR_GIVEN = -1
};

/*
  Stop Loss orders are stored in this market until the trigger price specified in the order is
  reached or surpassed. When the trigger price is reached or surpassed, the order is released in the
  Regular lot market.

  The stop loss condition is met under the following circumstances:

  Sell order - A sell order in the Stop Loss market gets triggered when the last traded price in the
  normal market reaches or falls below the trigger price of the order.

  Buy order - A buy order in the Stop Loss market gets triggered when the last traded price in the
  normal market reaches or exceeds the trigger price of the order.
*/

static inline DbrTicks
spread(struct DbrOrder* taker, struct DbrOrder* maker, int direct)
{
    return direct == DBR_PAID
        // Paid when the taker lifts the offer.
        ? maker->ticks - taker->ticks
        // Given when the taker hits the bid.
        : taker->ticks - maker->ticks;
}

static inline struct DbrPosn*
lazy_posn(struct DbrOrder* order, struct ElmCtx* ctx)
{
    struct DbrRec* mrec = order->market.rec;
    return elm_accnt_posn(order->accnt.rec, mrec->market.instr.rec, mrec->market.settl_date, ctx);
}

static DbrBool
match_orders(struct ElmCtx* ctx, DbrModel model, struct ElmMarket* market, struct DbrOrder* taker,
             const struct ElmSide* side, int direct, struct DbrTrans* trans)
{
    struct AshQueue mq;
    ash_queue_init(&mq);

    size_t count = 0;
    DbrLots taken = 0;

    struct DbrRec* mrec = taker->market.rec;
    struct DbrRec* irec = mrec->market.instr.rec;
    DbrDate settl_date = mrec->market.settl_date;

    struct DbrDlNode* node = elm_side_first_order(side),
        * end = elm_side_end_order(side);
    for (; taken < taker->resd && node != end; node = node->next) {

        struct DbrOrder* maker = dbr_side_order_entry(node);

        // Only consider orders while prices cross.
        if (spread(taker, maker, direct) > 0)
            break;

        const DbrIden match_id = dbr_model_alloc_id(model);
        struct DbrMatch* match = elm_ctx_alloc_match(ctx);
        if (!match)
            goto fail1;

        struct DbrPosn* posn = elm_accnt_posn(maker->accnt.rec, irec, settl_date, ctx);
        if (!posn) {
            // No need to free accnt or posn.
            elm_ctx_free_match(ctx, match);
            goto fail1;
        }

        const DbrIden taker_id = dbr_model_alloc_id(model);
        struct DbrTrade* taker_trade = elm_ctx_alloc_trade(ctx, taker_id);
        if (!taker_trade) {
            // No need to free accnt or posn.
            elm_ctx_free_match(ctx, match);
            goto fail1;
        }

        const DbrIden maker_id = dbr_model_alloc_id(model);
        struct DbrTrade* maker_trade = elm_ctx_alloc_trade(ctx, maker_id);
        if (!maker_trade) {
            elm_ctx_free_trade(ctx, taker_trade);
            elm_ctx_free_match(ctx, match);
            goto fail1;
        }

        match->id = match_id;
        match->maker_order = maker;
        match->maker_posn = posn;
        match->ticks = maker->ticks;
        match->lots = dbr_min(taker->resd - taken, maker->resd);

        ++count;
        taken += match->lots;

        const DbrMillis now = taker->created;

        // Taker trade.
        taker_trade->id = taker_id;
        taker_trade->match = match->id;
        taker_trade->order = taker->id;
        taker_trade->order_rev = taker->rev + 1;
        taker_trade->trader.rec = taker->trader.rec;
        taker_trade->accnt.rec = taker->accnt.rec;
        strncpy(taker_trade->ref, taker->ref, DBR_REF_MAX);
        taker_trade->market.rec = mrec;
        taker_trade->cpty.rec = maker->accnt.rec;
        taker_trade->role = DBR_TAKER;
        taker_trade->action = taker->action;
        taker_trade->ticks = match->ticks;
        taker_trade->resd = taker->resd - taken;
        taker_trade->exec = taker->exec + taken;
        taker_trade->lots = match->lots;
        taker_trade->settl_date = settl_date;
        taker_trade->created = now;
        taker_trade->modified = now;

        // Maker trade.
        maker_trade->id = maker_id;
        maker_trade->match = match->id;
        maker_trade->order = maker->id;
        maker_trade->order_rev = maker->rev + 1;
        maker_trade->trader.rec = maker->trader.rec;
        maker_trade->accnt.rec = maker->accnt.rec;
        strncpy(maker_trade->ref, maker->ref, DBR_REF_MAX);
        maker_trade->market.rec = mrec;
        maker_trade->cpty.rec = taker->accnt.rec;
        maker_trade->role = DBR_MAKER;
        maker_trade->action = maker->action;
        maker_trade->ticks = match->ticks;
        maker_trade->resd = maker->resd - match->lots;
        maker_trade->exec = maker->exec + match->lots;
        maker_trade->lots = match->lots;
        maker_trade->settl_date = settl_date;
        maker_trade->created = now;
        maker_trade->modified = now;

        match->taker_trade = taker_trade;
        match->maker_trade = maker_trade;

        ash_queue_push(&mq, &match->trans_node_);
    }

    struct DbrPosn* posn;
    // Avoid allocating position when there are no matches.
    if (count > 0) {
        if (!(posn = elm_accnt_posn(taker->accnt.rec, irec, settl_date, ctx)))
            goto fail1;
    } else
        posn = NULL;

    // Commit to trans.
    trans->new_order = taker;
    trans->new_posn = posn;
    trans->first_match = mq.first;
    trans->count = count;
    trans->taken = taken;

    return true;
 fail1:
    elm_ctx_free_matches(ctx, mq.first);
    return false;
}

DBR_EXTERN DbrBool
elm_match_orders(struct ElmCtx* ctx, DbrModel model, struct ElmMarket* market,
                 struct DbrOrder* taker, struct DbrTrans* trans)
{
    struct ElmSide* side;
    int direct;

    if (taker->action == DBR_BUY) {
        // Paid when the taker lifts the offer.
        side = &market->ask_side;
        direct = DBR_PAID;
    } else {
        assert(taker->action == DBR_SELL);
        // Given when the taker hits the bid.
        side = &market->bid_side;
        direct = DBR_GIVEN;
    }

    return match_orders(ctx, model, market, taker, side, direct, trans);
}
