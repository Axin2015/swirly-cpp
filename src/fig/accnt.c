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
#include "accnt.h"

#include <dbr/err.h>
#include <dbr/log.h>
#include <dbr/sess.h>

#include <assert.h>
#include <stdlib.h>

static void
trade(DbrAccntSess sess, struct DbrOrder* order, struct DbrTrade* trade, struct DbrPosn* posn)
{
}

static const struct DbrAccntSessVtbl SESS_VTBL = {
    .trade = trade
};

static struct DbrIAccntSess sess_noop = {
    .vtbl = &SESS_VTBL
};

static void
free_membs(struct FigAccnt* accnt)
{
    assert(accnt);
    struct DbrRbNode* node;
    while ((node = accnt->membs.root)) {
        struct DbrMemb* memb = dbr_accnt_memb_entry(node);
        dbr_tree_remove(&accnt->membs, node);
        dbr_pool_free_memb(accnt->pool, memb);
    }
}

static void
free_trades(struct FigAccnt* accnt)
{
    assert(accnt);
    struct DbrRbNode* node;
    while ((node = accnt->trades.root)) {
        struct DbrTrade* trade = dbr_accnt_trade_entry(node);
        dbr_tree_remove(&accnt->trades, node);
        dbr_pool_free_trade(accnt->pool, trade);
    }
}

static void
free_posns(struct FigAccnt* accnt)
{
    assert(accnt);
    struct DbrRbNode* node;
    while ((node = accnt->posns.root)) {
        struct DbrPosn* posn = dbr_accnt_posn_entry(node);
        dbr_tree_remove(&accnt->posns, node);
        dbr_pool_free_posn(accnt->pool, posn);
    }
}

DBR_EXTERN struct FigAccnt*
fig_accnt_lazy(struct DbrRec* arec, DbrPool pool)
{
    assert(arec);
    assert(arec->type == DBR_ACCNT);
    struct FigAccnt* accnt = arec->accnt.state;
    if (dbr_unlikely(!accnt)) {
        accnt = malloc(sizeof(struct FigAccnt));
        if (dbr_unlikely(!accnt)) {
            dbr_err_set(DBR_ENOMEM, "out of memory");
            return NULL;
        }
        accnt->id = arec->id;
        accnt->pool = pool;
        dbr_tree_init(&accnt->membs);
        dbr_tree_init(&accnt->trades);
        dbr_tree_init(&accnt->posns);
        accnt->sess = &sess_noop;

        arec->accnt.state = accnt;
    }
    return accnt;
}

DBR_EXTERN void
fig_accnt_term(struct DbrRec* arec)
{
    assert(arec);
    assert(arec->type == DBR_ACCNT);
    struct FigAccnt* accnt = arec->accnt.state;
    if (accnt) {
        arec->accnt.state = NULL;
        free_posns(accnt);
        free_trades(accnt);
        free_membs(accnt);
        free(accnt);
    }
}

DBR_EXTERN struct DbrPosn*
fig_accnt_posn(struct DbrRec* arec, struct DbrRec* crec, DbrDate settl_date, DbrPool pool)
{
    assert(arec);
    assert(arec->type == DBR_ACCNT);

    assert(crec);
    assert(crec->type == DBR_CONTR);

    // Synthetic key from contract and settlment date.
    const DbrIden key = crec->id * 100000000L + settl_date;

    struct FigAccnt* accnt = fig_accnt_lazy(arec, pool);
    if (dbr_unlikely(!accnt))
        return NULL;

    struct DbrPosn* posn;
	struct DbrRbNode* node = dbr_tree_pfind(&accnt->posns, key);
    if (!node || node->key != key) {
        if (!(posn = dbr_pool_alloc_posn(accnt->pool)))
            return NULL;

        posn->accnt.rec = arec;
        posn->contr.rec = crec;
        posn->settl_date = settl_date;
        posn->buy_licks = 0;
        posn->buy_lots = 0;
        posn->sell_licks = 0;
        posn->sell_lots = 0;

        dbr_log_debug1("insert posn: accnt=%.16s, contr=%.16s, settl_date=%d",
                       arec->mnem, crec->mnem, settl_date);

        struct DbrRbNode* parent = node;
        dbr_tree_pinsert(&accnt->posns, key, &posn->accnt_node_, parent);
    } else
        posn = dbr_accnt_posn_entry(node);
    return posn;
}

DBR_EXTERN void
fig_accnt_set_sess(struct FigAccnt* accnt, DbrAccntSess sess)
{
    accnt->sess = sess ? sess : &sess_noop;
}

DBR_API DbrIden
dbr_accnt_id(DbrAccnt accnt)
{
    return fig_accnt_id(accnt);
}

// AccntTrade

DBR_API struct DbrRbNode*
dbr_accnt_find_trade_id(DbrAccnt accnt, DbrIden id)
{
    return fig_accnt_find_trade_id(accnt, id);
}

DBR_API struct DbrRbNode*
dbr_accnt_first_trade(DbrAccnt accnt)
{
    return fig_accnt_first_trade(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_last_trade(DbrAccnt accnt)
{
    return fig_accnt_last_trade(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_end_trade(DbrAccnt accnt)
{
    return fig_accnt_end_trade(accnt);
}

DBR_API DbrBool
dbr_accnt_empty_trade(DbrAccnt accnt)
{
    return fig_accnt_empty_trade(accnt);
}

// AccntPosn.

DBR_API struct DbrRbNode*
dbr_accnt_find_posn_id(DbrAccnt accnt, DbrIden id)
{
    return fig_accnt_find_posn_id(accnt, id);
}

DBR_API struct DbrRbNode*
dbr_accnt_first_posn(DbrAccnt accnt)
{
    return fig_accnt_first_posn(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_last_posn(DbrAccnt accnt)
{
    return fig_accnt_last_posn(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_end_posn(DbrAccnt accnt)
{
    return fig_accnt_end_posn(accnt);
}

DBR_API DbrBool
dbr_accnt_empty_posn(DbrAccnt accnt)
{
    return fig_accnt_empty_posn(accnt);
}

DBR_API void
dbr_accnt_set_sess(DbrAccnt accnt, DbrAccntSess sess)
{
    fig_accnt_set_sess(accnt, sess);
}

DBR_API DbrAccntSess
dbr_accnt_sess(DbrAccnt accnt)
{
    return fig_accnt_sess(accnt);
}
