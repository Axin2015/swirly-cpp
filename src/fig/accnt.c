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
#include "accnt.h"

#include <dbr/conv.h> // dbr_book_key()
#include <dbr/err.h>
#include <dbr/log.h>
#include <dbr/pool.h>

#include <assert.h>
#include <stdlib.h>

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
    assert(arec->type == DBR_ENTITY_ACCNT);
    struct FigAccnt* accnt = arec->accnt.state;
    if (dbr_unlikely(!accnt)) {
        accnt = malloc(sizeof(struct FigAccnt));
        if (dbr_unlikely(!accnt)) {
            dbr_err_set(DBR_ENOMEM, "out of memory");
            return NULL;
        }
        accnt->rec = arec;
        accnt->pool = pool;
        dbr_tree_init(&accnt->membs);
        dbr_tree_init(&accnt->posns);

        arec->accnt.state = accnt;
    }
    return accnt;
}

DBR_EXTERN void
fig_accnt_term(struct DbrRec* arec)
{
    assert(arec);
    assert(arec->type == DBR_ENTITY_ACCNT);
    struct FigAccnt* accnt = arec->accnt.state;
    if (accnt) {
        arec->accnt.state = NULL;
        // Do not free members because these are owned by the trader.
        free_posns(accnt);
        free(accnt);
    }
}

DBR_EXTERN void
fig_accnt_clear(struct FigAccnt* accnt)
{
    free_posns(accnt);
    dbr_tree_init(&accnt->membs);
}

DBR_EXTERN struct DbrPosn*
fig_accnt_update_posn(struct FigAccnt* accnt, struct DbrPosn* posn)
{
    // Synthetic key from contract and settlment date.
    const DbrIden key = dbr_book_key(posn->contr.rec->id, posn->settl_date);
    struct DbrRbNode* node = dbr_tree_insert(&accnt->posns, key, &posn->accnt_node_);
    if (node != &posn->accnt_node_) {
        struct DbrPosn* curr = dbr_accnt_posn_entry(node);

        // Update existing position.

        assert(curr->accnt.rec == posn->accnt.rec);
        assert(curr->contr.rec == posn->contr.rec);
        assert(curr->settl_date == posn->settl_date);

        curr->buy_licks = posn->buy_licks;
        curr->buy_lots = posn->buy_lots;
        curr->sell_licks = posn->sell_licks;
        curr->sell_lots = posn->sell_lots;

        dbr_pool_free_posn(accnt->pool, posn);
        posn = curr;
    }
    return posn;
}

DBR_EXTERN struct DbrPosn*
fig_accnt_posn(struct DbrRec* arec, struct DbrRec* crec, DbrDate settl_date, DbrPool pool)
{
    assert(arec);
    assert(arec->type == DBR_ENTITY_ACCNT);

    assert(crec);
    assert(crec->type == DBR_ENTITY_CONTR);

    const DbrIden key = dbr_book_key(crec->id, settl_date);
    struct FigAccnt* accnt = fig_accnt_lazy(arec, pool);
    if (dbr_unlikely(!accnt))
        return NULL;

    struct DbrPosn* posn;
	struct DbrRbNode* node = dbr_tree_pfind(&accnt->posns, key);
    if (!node || node->key != key) {
        if (!(posn = dbr_pool_alloc_posn(accnt->pool)))
            return NULL;
        dbr_posn_init(posn);

        posn->accnt.rec = arec;
        posn->contr.rec = crec;
        posn->settl_date = settl_date;
        posn->buy_licks = 0;
        posn->buy_lots = 0;
        posn->sell_licks = 0;
        posn->sell_lots = 0;

        dbr_log_debug2("insert posn: accnt=%.16s, contr=%.16s, settl_date=%d",
                       arec->mnem, crec->mnem, settl_date);

        struct DbrRbNode* parent = node;
        dbr_tree_pinsert(&accnt->posns, key, &posn->accnt_node_, parent);
    } else
        posn = dbr_accnt_posn_entry(node);
    return posn;
}

DBR_API struct DbrRec*
dbr_accnt_rec(DbrAccnt accnt)
{
    return fig_accnt_rec(accnt);
}

// AccntMemb

DBR_API struct DbrRbNode*
dbr_accnt_find_memb_id(DbrAccnt accnt, DbrIden id)
{
    return fig_accnt_find_memb_id(accnt, id);
}

DBR_API struct DbrRbNode*
dbr_accnt_first_memb(DbrAccnt accnt)
{
    return fig_accnt_first_memb(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_last_memb(DbrAccnt accnt)
{
    return fig_accnt_last_memb(accnt);
}

DBR_API DbrBool
dbr_accnt_empty_memb(DbrAccnt accnt)
{
    return fig_accnt_empty_memb(accnt);
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

DBR_API DbrBool
dbr_accnt_empty_posn(DbrAccnt accnt)
{
    return fig_accnt_empty_posn(accnt);
}
