/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
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

#include <dbr/elm/refcount.h>
#include <dbr/elm/side.h>

#include <dbr/ash/bank.h>
#include <dbr/ash/queue.h>

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
free_match_list(struct DbrSlNode* first, DbrPool pool)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrMatch* match = dbr_trans_match_entry(node);
        node = node->next;
        // Not committed so match object still owns the trades.
        dbr_exec_decref(match->taker_exec, pool);
        dbr_exec_decref(match->maker_exec, pool);
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
        ? maker->i.ticks - taker->i.ticks
        // Given when the taker hits the bid.
        : taker->i.ticks - maker->i.ticks;
}

static DbrBool
match_orders(struct DbrBook* book, struct DbrOrder* taker, const struct DbrSide* side, int direct,
             struct DbrBank* bank, struct FigOrdIdx* ordidx, DbrPool pool, struct DbrTrans* trans)
{
    DbrLots taken = 0;
    DbrTicks last_ticks = 0;
    DbrLots last_lots = 0;

    struct DbrRec* crec = book->crec;
    DbrJd settl_day = book->settl_day;

    struct DbrDlNode* node = dbr_side_first_order(side),
        * end = dbr_side_end_order(side);
    for (; taken < taker->i.resd && node != end; node = node->next) {

        struct DbrOrder* maker = dbr_side_order_entry(node);

        // Only consider orders while prices cross.
        if (spread(taker, maker, direct) > 0)
            break;

        struct DbrMatch* match = dbr_pool_alloc_match(pool);
        if (!match)
            goto fail1;
        dbr_match_init(match);

        struct DbrPosn* posn = fig_accnt_posn(maker->i.giveup.rec, crec, settl_day,
                                              ordidx, pool);
        if (!posn) {
            // No need to free accnt or posn.
            dbr_pool_free_match(pool, match);
            goto fail1;
        }

        struct DbrExec* taker_exec = dbr_pool_alloc_exec(pool);
        if (!taker_exec) {
            // No need to free accnt or posn.
            dbr_pool_free_match(pool, match);
            goto fail1;
        }
        dbr_exec_init(taker_exec);

        struct DbrExec* maker_exec = dbr_pool_alloc_exec(pool);
        if (!maker_exec) {
            dbr_exec_decref(taker_exec, pool);
            dbr_pool_free_match(pool, match);
            goto fail1;
        }
        dbr_exec_init(maker_exec);

        const DbrIden taker_id = dbr_bank_add_fetch(bank, DBR_REG_EXEC, 1L);
        const DbrIden maker_id = dbr_bank_add_fetch(bank, DBR_REG_EXEC, 1L);
        const DbrIden match_id = dbr_bank_add_fetch(bank, DBR_REG_MATCH, 1L);

        match->id = match_id;
        match->maker_order = maker;
        match->maker_posn = posn;
        match->ticks = maker->i.ticks;
        match->lots = dbr_min(taker->i.resd - taken, maker->i.resd);

        taken += match->lots;
        last_ticks = match->ticks;
        last_lots = match->lots;

        const DbrMillis now = taker->created;

        // Taker trade.
        taker_exec->id = taker_id;
        taker_exec->order = taker->id;
        taker_exec->i.trader.rec = taker->i.trader.rec;
        taker_exec->i.giveup.rec = taker->i.giveup.rec;
        taker_exec->i.contr.rec = crec;
        taker_exec->i.settl_day = settl_day;
        strncpy(taker_exec->i.ref, taker->i.ref, DBR_REF_MAX);
        taker_exec->i.state = DBR_STATE_TRADE;
        taker_exec->i.action = taker->i.action;
        taker_exec->i.ticks = taker->i.ticks;
        taker_exec->i.lots = taker->i.lots;
        taker_exec->i.resd = taker->i.resd - taken;
        taker_exec->i.exec = taker->i.exec + taken;
        taker_exec->i.last_ticks = match->ticks;
        taker_exec->i.last_lots = match->lots;
        taker_exec->i.min_lots = taker->i.min_lots;
        taker_exec->match = match->id;
        taker_exec->role = DBR_ROLE_TAKER;
        taker_exec->cpty.rec = maker->i.giveup.rec;
        taker_exec->created = now;

        // Maker trade.
        maker_exec->id = maker_id;
        maker_exec->order = maker->id;
        maker_exec->i.trader.rec = maker->i.trader.rec;
        maker_exec->i.giveup.rec = maker->i.giveup.rec;
        maker_exec->i.contr.rec = crec;
        maker_exec->i.settl_day = settl_day;
        strncpy(maker_exec->i.ref, maker->i.ref, DBR_REF_MAX);
        maker_exec->i.state = DBR_STATE_TRADE;
        maker_exec->i.action = maker->i.action;
        maker_exec->i.ticks = maker->i.ticks;
        maker_exec->i.lots = maker->i.lots;
        maker_exec->i.resd = maker->i.resd - match->lots;
        maker_exec->i.exec = maker->i.exec + match->lots;
        maker_exec->i.last_ticks = match->ticks;
        maker_exec->i.last_lots = match->lots;
        maker_exec->i.min_lots = maker->i.min_lots;
        maker_exec->match = match->id;
        maker_exec->role = DBR_ROLE_MAKER;
        maker_exec->cpty.rec = taker->i.giveup.rec;
        maker_exec->created = now;

        match->taker_exec = taker_exec;
        match->maker_exec = maker_exec;

        dbr_queue_insert_back(&trans->matches, &match->trans_node_);

        // Maker updated first because this is consistent with last-look semantics.
        // N.B. the reference count is not incremented here.
        dbr_queue_insert_back(&trans->execs, &maker_exec->shared_node_);
        dbr_queue_insert_back(&trans->execs, &taker_exec->shared_node_);
    }

    if (!dbr_queue_empty(&trans->matches)) {

        // Avoid allocating position when there are no matches.
        if (!(trans->taker_posn = fig_accnt_posn(taker->i.giveup.rec, crec, settl_day,
                                                 ordidx, pool)))
            goto fail1;

        // Commit taker order.
        taker->i.state = DBR_STATE_TRADE;
        taker->i.resd -= taken;
        taker->i.exec += taken;
        taker->i.last_ticks = last_ticks;
        taker->i.last_lots = last_lots;

    } else
        trans->taker_posn = NULL;

    return DBR_TRUE;
 fail1:
    free_match_list(dbr_queue_first(&trans->matches), pool);
    return DBR_FALSE;
}

DBR_EXTERN DbrBool
fig_match_orders(struct DbrBook* book, struct DbrOrder* taker, struct DbrBank* bank,
                 struct FigOrdIdx* ordidx, DbrPool pool, struct DbrTrans* trans)
{
    struct DbrSide* side;
    int direct;

    if (taker->i.action == DBR_ACTION_BUY) {
        // Paid when the taker lifts the offer.
        side = &book->offer_side;
        direct = DBR_PAID;
    } else {
        assert(taker->i.action == DBR_ACTION_SELL);
        // Given when the taker hits the bid.
        side = &book->bid_side;
        direct = DBR_GIVEN;
    }

    return match_orders(book, taker, side, direct, bank, ordidx, pool, trans);
}
