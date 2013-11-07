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

#include <dbr/book.h>
#include <dbr/journ.h>
#include <dbr/queue.h>

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

static void
free_matches(struct DbrSlNode* first, DbrPool pool)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrMatch* match = dbr_trans_match_entry(node);
        node = node->next;
        // Not committed so match object still owns the trades.
        dbr_pool_free_trade(pool, match->taker_trade);
        dbr_pool_free_trade(pool, match->maker_trade);
        dbr_pool_free_match(pool, match);
    }
}

/*
  Stop Loss orders are stored in this book until the trigger price specified in the order is
  reached or surpassed. When the trigger price is reached or surpassed, the order is released in the
  Regular lot book.

  The stop loss condition is met under the following circumstances:

  Sell order - A sell order in the Stop Loss book gets triggered when the last traded price in the
  normal book reaches or falls below the trigger price of the order.

  Buy order - A buy order in the Stop Loss book gets triggered when the last traded price in the
  normal book reaches or exceeds the trigger price of the order.
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
lazy_posn(struct DbrOrder* order, DbrPool pool)
{
    return fig_accnt_posn(order->accnt.rec, order->contr.rec, order->settl_date, pool);
}

static DbrBool
match_orders(struct DbrBook* book, struct DbrOrder* taker, const struct DbrSide* side, int direct,
             DbrJourn journ, DbrPool pool, struct DbrTrans* trans)
{
    struct DbrQueue mq;
    dbr_queue_init(&mq);

    size_t count = 0;
    DbrLots taken = 0;

    struct DbrRec* crec = book->crec;
    DbrDate settl_date = book->settl_date;

    struct DbrDlNode* node = dbr_side_first_order(side),
        * end = dbr_side_end_order(side);
    for (; taken < taker->resd && node != end; node = node->next) {

        struct DbrOrder* maker = dbr_side_order_entry(node);

        // Only consider orders while prices cross.
        if (spread(taker, maker, direct) > 0)
            break;

        const DbrIden match_id = dbr_journ_alloc_id(journ);
        struct DbrMatch* match = dbr_pool_alloc_match(pool);
        if (!match)
            goto fail1;
        dbr_match_init(match);

        struct DbrPosn* posn = fig_accnt_posn(maker->accnt.rec, crec, settl_date, pool);
        if (!posn) {
            // No need to free accnt or posn.
            dbr_pool_free_match(pool, match);
            goto fail1;
        }

        const DbrIden taker_id = dbr_journ_alloc_id(journ);
        struct DbrTrade* taker_trade = dbr_pool_alloc_trade(pool);
        if (!taker_trade) {
            // No need to free accnt or posn.
            dbr_pool_free_match(pool, match);
            goto fail1;
        }
        dbr_trade_init(taker_trade);

        const DbrIden maker_id = dbr_journ_alloc_id(journ);
        struct DbrTrade* maker_trade = dbr_pool_alloc_trade(pool);
        if (!maker_trade) {
            dbr_pool_free_trade(pool, taker_trade);
            dbr_pool_free_match(pool, match);
            goto fail1;
        }
        dbr_trade_init(maker_trade);

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
        taker_trade->order = taker->id;
        taker_trade->rev = taker->rev + 1;
        taker_trade->trader.rec = taker->trader.rec;
        taker_trade->accnt.rec = taker->accnt.rec;
        taker_trade->contr.rec = crec;
        taker_trade->settl_date = settl_date;
        strncpy(taker_trade->ref, taker->ref, DBR_REF_MAX);
        taker_trade->action = taker->action;
        taker_trade->ticks = match->ticks;
        taker_trade->resd = taker->resd - taken;
        taker_trade->exec = taker->exec + taken;
        taker_trade->lots = match->lots;
        taker_trade->match = match->id;
        taker_trade->cpty.rec = maker->accnt.rec;
        taker_trade->role = DBR_TAKER;
        taker_trade->created = now;
        taker_trade->modified = now;

        // Maker trade.
        maker_trade->id = maker_id;
        maker_trade->order = maker->id;
        maker_trade->rev = maker->rev + 1;
        maker_trade->trader.rec = maker->trader.rec;
        maker_trade->accnt.rec = maker->accnt.rec;
        maker_trade->contr.rec = crec;
        maker_trade->settl_date = settl_date;
        strncpy(maker_trade->ref, maker->ref, DBR_REF_MAX);
        maker_trade->action = maker->action;
        maker_trade->ticks = match->ticks;
        maker_trade->resd = maker->resd - match->lots;
        maker_trade->exec = maker->exec + match->lots;
        maker_trade->lots = match->lots;
        maker_trade->match = match->id;
        maker_trade->cpty.rec = taker->accnt.rec;
        maker_trade->role = DBR_MAKER;
        maker_trade->created = now;
        maker_trade->modified = now;

        match->taker_trade = taker_trade;
        match->maker_trade = maker_trade;

        dbr_queue_insert_back(&mq, &match->trans_node_);
    }

    struct DbrPosn* posn;
    // Avoid allocating position when there are no matches.
    if (count > 0) {
        if (!(posn = fig_accnt_posn(taker->accnt.rec, crec, settl_date, pool)))
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
    free_matches(dbr_queue_first(&mq), pool);
    return false;
}

DBR_EXTERN DbrBool
fig_match_orders(struct DbrBook* book, struct DbrOrder* taker, DbrJourn journ, DbrPool pool,
                 struct DbrTrans* trans)
{
    struct DbrSide* side;
    int direct;

    if (taker->action == DBR_BUY) {
        // Paid when the taker lifts the offer.
        side = &book->ask_side;
        direct = DBR_PAID;
    } else {
        assert(taker->action == DBR_SELL);
        // Given when the taker hits the bid.
        side = &book->bid_side;
        direct = DBR_GIVEN;
    }

    return match_orders(book, taker, side, direct, journ, pool, trans);
}
