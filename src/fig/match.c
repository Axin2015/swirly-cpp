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
        dbr_pool_free_exec(pool, match->taker_exec);
        dbr_pool_free_exec(pool, match->maker_exec);
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
        ? maker->c.ticks - taker->c.ticks
        // Given when the taker hits the bid.
        : taker->c.ticks - maker->c.ticks;
}

static inline struct DbrPosn*
lazy_posn(struct DbrOrder* order, DbrPool pool)
{
    return fig_accnt_posn(order->c.accnt.rec, order->c.contr.rec, order->c.settl_date, pool);
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
    for (; taken < taker->c.resd && node != end; node = node->next) {

        struct DbrOrder* maker = dbr_side_order_entry(node);

        // Only consider orders while prices cross.
        if (spread(taker, maker, direct) > 0)
            break;

        const DbrIden match_id = dbr_journ_alloc_id(journ);
        struct DbrMatch* match = dbr_pool_alloc_match(pool);
        if (!match)
            goto fail1;
        dbr_match_init(match);

        struct DbrPosn* posn = fig_accnt_posn(maker->c.accnt.rec, crec, settl_date, pool);
        if (!posn) {
            // No need to free accnt or posn.
            dbr_pool_free_match(pool, match);
            goto fail1;
        }

        const DbrIden taker_id = dbr_journ_alloc_id(journ);
        struct DbrExec* taker_exec = dbr_pool_alloc_exec(pool);
        if (!taker_exec) {
            // No need to free accnt or posn.
            dbr_pool_free_match(pool, match);
            goto fail1;
        }
        dbr_exec_init(taker_exec);

        const DbrIden maker_id = dbr_journ_alloc_id(journ);
        struct DbrExec* maker_exec = dbr_pool_alloc_exec(pool);
        if (!maker_exec) {
            dbr_pool_free_exec(pool, taker_exec);
            dbr_pool_free_match(pool, match);
            goto fail1;
        }
        dbr_exec_init(maker_exec);

        match->id = match_id;
        match->maker_order = maker;
        match->maker_posn = posn;
        match->ticks = maker->c.ticks;
        match->lots = dbr_min(taker->c.resd - taken, maker->c.resd);

        ++count;
        taken += match->lots;

        const DbrMillis now = taker->created;

        // Taker trade.
        taker_exec->id = taker_id;
        taker_exec->c.id = taker->c.id;
        taker_exec->c.rev = taker->c.rev + 1;
        taker_exec->c.status = DBR_TRADED;
        taker_exec->c.trader.rec = taker->c.trader.rec;
        taker_exec->c.accnt.rec = taker->c.accnt.rec;
        taker_exec->c.contr.rec = crec;
        taker_exec->c.settl_date = settl_date;
        strncpy(taker_exec->c.ref, taker->c.ref, DBR_REF_MAX);
        taker_exec->c.action = taker->c.action;
        taker_exec->c.ticks = taker->c.ticks;
        taker_exec->c.lots = taker->c.lots;
        taker_exec->c.resd = taker->c.resd - taken;
        taker_exec->c.exec = taker->c.exec + taken;
        taker_exec->c.last_ticks = match->ticks;
        taker_exec->c.last_lots = match->lots;
        taker_exec->match = match->id;
        taker_exec->role = DBR_TAKER;
        taker_exec->cpty.rec = maker->c.accnt.rec;
        taker_exec->created = now;
        taker_exec->modified = now;

        // Maker trade.
        maker_exec->id = maker_id;
        maker_exec->c.id = maker->c.id;
        maker_exec->c.rev = maker->c.rev + 1;
        maker_exec->c.status = DBR_TRADED;
        maker_exec->c.trader.rec = maker->c.trader.rec;
        maker_exec->c.accnt.rec = maker->c.accnt.rec;
        maker_exec->c.contr.rec = crec;
        maker_exec->c.settl_date = settl_date;
        strncpy(maker_exec->c.ref, maker->c.ref, DBR_REF_MAX);
        maker_exec->c.action = maker->c.action;
        maker_exec->c.ticks = maker->c.ticks;
        maker_exec->c.lots = maker->c.lots;
        maker_exec->c.resd = maker->c.resd - match->lots;
        maker_exec->c.exec = maker->c.exec + match->lots;
        maker_exec->c.last_ticks = match->ticks;
        maker_exec->c.last_lots = match->lots;
        maker_exec->match = match->id;
        maker_exec->role = DBR_MAKER;
        maker_exec->cpty.rec = taker->c.accnt.rec;
        maker_exec->created = now;
        maker_exec->modified = now;

        match->taker_exec = taker_exec;
        match->maker_exec = maker_exec;

        dbr_queue_insert_back(&mq, &match->trans_node_);
    }

    struct DbrPosn* posn;
    // Avoid allocating position when there are no matches.
    if (count > 0) {
        if (!(posn = fig_accnt_posn(taker->c.accnt.rec, crec, settl_date, pool)))
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

    if (taker->c.action == DBR_BUY) {
        // Paid when the taker lifts the offer.
        side = &book->ask_side;
        direct = DBR_PAID;
    } else {
        assert(taker->c.action == DBR_SELL);
        // Given when the taker hits the bid.
        side = &book->bid_side;
        direct = DBR_GIVEN;
    }

    return match_orders(book, taker, side, direct, journ, pool, trans);
}
