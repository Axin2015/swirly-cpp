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

#include <dbr/ash/err.h>
#include <dbr/ash/log.h>
#include <dbr/elm/pool.h>

#include <assert.h>
#include <stdlib.h>

static void
free_users(struct FigAccnt* accnt)
{
    assert(accnt);
    struct DbrRbNode* node;
    while ((node = accnt->users.root)) {
        struct DbrMemb* memb = dbr_accnt_user_entry(node);
        dbr_tree_remove(&accnt->users, node);
        dbr_pool_free_memb(accnt->pool, memb);
    }
}

static void
free_groups(struct FigAccnt* accnt)
{
    assert(accnt);
    struct DbrRbNode* node;
    while ((node = accnt->groups.root)) {
        struct DbrMemb* memb = dbr_accnt_group_entry(node);
        dbr_tree_remove(&accnt->groups, node);
        dbr_pool_free_memb(accnt->pool, memb);
    }
}

static void
free_orders(struct FigAccnt* accnt)
{
    assert(accnt);
    struct DbrRbNode* node;
    while ((node = accnt->orders.root)) {
        struct DbrOrder* order = dbr_accnt_order_entry(node);
        fig_accnt_release_order(accnt, order);
        dbr_pool_free_order(accnt->pool, order);
    }
}

static void
free_execs(struct FigAccnt* accnt)
{
    assert(accnt);
    struct DbrRbNode* node;
    while ((node = accnt->trades.root)) {
        struct DbrExec* exec = dbr_accnt_trade_entry(node);
        dbr_tree_remove(&accnt->trades, node);
        dbr_exec_decref(exec, accnt->pool);
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
fig_accnt_lazy(struct DbrRec* arec, struct FigOrdIdx* ordidx, DbrPool pool)
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
        accnt->ordidx = ordidx;
        accnt->pool = pool;
        dbr_tree_init(&accnt->users);
        dbr_tree_init(&accnt->groups);
        dbr_tree_init(&accnt->orders);
        dbr_tree_init(&accnt->trades);
        dbr_tree_init(&accnt->posns);
        accnt->sess = NULL;
        dbr_rbnode_init(&accnt->sess_node_);

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
        fig_accnt_reset_user(accnt);
        fig_accnt_reset_group(accnt);
        free(accnt);
    }
}

DBR_EXTERN void
fig_accnt_reset_user(struct FigAccnt* accnt)
{
    free_execs(accnt);
    free_orders(accnt);
    free_groups(accnt);
    free_users(accnt);
}

DBR_EXTERN void
fig_accnt_reset_group(struct FigAccnt* accnt)
{
    free_posns(accnt);
}

DBR_EXTERN struct DbrPosn*
fig_accnt_update_posn(struct FigAccnt* accnt, struct DbrPosn* posn)
{
    const DbrKey key = dbr_posn_key(posn->accnt.rec->id, posn->contr.rec->id, posn->settl_day);
    struct DbrRbNode* node = dbr_tree_insert(&accnt->posns, key, &posn->accnt_node_);
    if (node != &posn->accnt_node_) {
        struct DbrPosn* exist = dbr_accnt_posn_entry(node);

        // Update existing position.

        assert(exist->accnt.rec == posn->accnt.rec);
        assert(exist->contr.rec == posn->contr.rec);
        assert(exist->settl_day == posn->settl_day);

        exist->buy_licks = posn->buy_licks;
        exist->buy_lots = posn->buy_lots;
        exist->sell_licks = posn->sell_licks;
        exist->sell_lots = posn->sell_lots;

        dbr_pool_free_posn(accnt->pool, posn);
        posn = exist;
    }
    return posn;
}

DBR_EXTERN struct DbrPosn*
fig_accnt_posn(struct DbrRec* arec, struct DbrRec* crec, DbrJd settl_day,
               struct FigOrdIdx* ordidx, DbrPool pool)
{
    assert(arec);
    assert(arec->type == DBR_ENTITY_ACCNT);

    assert(crec);
    assert(crec->type == DBR_ENTITY_CONTR);

    const DbrKey key = dbr_posn_key(arec->id, crec->id, settl_day);
    struct FigAccnt* accnt = fig_accnt_lazy(arec, ordidx, pool);
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
        posn->settl_day = settl_day;
        posn->buy_licks = 0;
        posn->buy_lots = 0;
        posn->sell_licks = 0;
        posn->sell_lots = 0;

        dbr_log_debug2("insert posn: accnt=%.16s, contr=%.16s, settl_day=%d",
                       arec->mnem, crec->mnem, settl_day);

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

// AccntUser

DBR_API struct DbrRbNode*
dbr_accnt_find_user_id(DbrAccnt accnt, DbrIden id)
{
    return fig_accnt_find_user_id(accnt, id);
}

DBR_API struct DbrRbNode*
dbr_accnt_first_user(DbrAccnt accnt)
{
    return fig_accnt_first_user(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_last_user(DbrAccnt accnt)
{
    return fig_accnt_last_user(accnt);
}

DBR_API DbrBool
dbr_accnt_empty_user(DbrAccnt accnt)
{
    return fig_accnt_empty_user(accnt);
}

// AccntGroup

DBR_API struct DbrRbNode*
dbr_accnt_find_group_id(DbrAccnt accnt, DbrIden id)
{
    return fig_accnt_find_group_id(accnt, id);
}

DBR_API struct DbrRbNode*
dbr_accnt_first_group(DbrAccnt accnt)
{
    return fig_accnt_first_group(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_last_group(DbrAccnt accnt)
{
    return fig_accnt_last_group(accnt);
}

DBR_API DbrBool
dbr_accnt_empty_group(DbrAccnt accnt)
{
    return fig_accnt_empty_group(accnt);
}

// AccntOrder

DBR_API struct DbrRbNode*
dbr_accnt_find_order_id(DbrAccnt accnt, DbrIden id)
{
    return fig_accnt_find_order_id(accnt, id);
}

DBR_API struct DbrOrder*
dbr_accnt_find_order_ref(DbrAccnt accnt, const char* ref)
{
    return fig_accnt_find_order_ref(accnt, ref);
}

DBR_API struct DbrRbNode*
dbr_accnt_first_order(DbrAccnt accnt)
{
    return fig_accnt_first_order(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_last_order(DbrAccnt accnt)
{
    return fig_accnt_last_order(accnt);
}

DBR_API DbrBool
dbr_accnt_empty_order(DbrAccnt accnt)
{
    return fig_accnt_empty_order(accnt);
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

DBR_API DbrBool
dbr_accnt_empty_posn(DbrAccnt accnt)
{
    return fig_accnt_empty_posn(accnt);
}

// Accnt

DBR_API DbrBool
dbr_accnt_logged_on(DbrAccnt accnt)
{
    return fig_accnt_logged_on(accnt);
}

DBR_API struct DbrSess*
dbr_accnt_sess(DbrAccnt accnt)
{
    return fig_accnt_sess(accnt);
}
