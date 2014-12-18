/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "accnt.h"

#include <sc/elm/pool.h>

#include <sc/ash/err.h>
#include <sc/ash/log.h>

#include <assert.h>
#include <stdlib.h>

static void
free_traders(struct FigAccnt* accnt)
{
    assert(accnt);
    struct ScRbNode* node;
    while ((node = accnt->traders.root)) {
        struct ScPerm* perm = sc_accnt_trader_entry(node);
        sc_tree_remove(&accnt->traders, node);
        sc_pool_free_perm(accnt->pool, perm);
    }
}

static void
free_giveups(struct FigAccnt* accnt)
{
    assert(accnt);
    struct ScRbNode* node;
    while ((node = accnt->giveups.root)) {
        struct ScPerm* perm = sc_accnt_giveup_entry(node);
        sc_tree_remove(&accnt->giveups, node);
        sc_pool_free_perm(accnt->pool, perm);
    }
}

static void
free_orders(struct FigAccnt* accnt)
{
    assert(accnt);
    struct ScRbNode* node;
    while ((node = accnt->orders.root)) {
        struct ScOrder* order = sc_accnt_order_entry(node);
        fig_accnt_release_order(accnt, order);
        sc_pool_free_order(accnt->pool, order);
    }
}

static void
free_execs(struct FigAccnt* accnt)
{
    assert(accnt);
    struct ScRbNode* node;
    while ((node = accnt->trades.root)) {
        struct ScExec* exec = sc_accnt_trade_entry(node);
        sc_tree_remove(&accnt->trades, node);
        sc_exec_decref(exec, accnt->pool);
    }
}

static void
free_posns(struct FigAccnt* accnt)
{
    assert(accnt);
    struct ScRbNode* node;
    while ((node = accnt->posns.root)) {
        struct ScPosn* posn = sc_accnt_posn_entry(node);
        sc_tree_remove(&accnt->posns, node);
        sc_pool_free_posn(accnt->pool, posn);
    }
}

SC_EXTERN struct FigAccnt*
fig_accnt_lazy(struct ScRec* arec, struct FigOrdIdx* ordidx, ScPool pool)
{
    assert(arec);
    assert(arec->type == SC_ENTITY_ACCNT);
    struct FigAccnt* accnt = arec->accnt.state;
    if (sc_unlikely(!accnt)) {
        accnt = malloc(sizeof(struct FigAccnt));
        if (sc_unlikely(!accnt)) {
            sc_err_set(SC_ENOMEM, "out of memory");
            return NULL;
        }
        accnt->rec = arec;
        accnt->ordidx = ordidx;
        accnt->pool = pool;
        sc_tree_init(&accnt->traders);
        sc_tree_init(&accnt->giveups);
        sc_tree_init(&accnt->orders);
        sc_tree_init(&accnt->trades);
        sc_tree_init(&accnt->posns);
        accnt->sess = NULL;
        sc_rbnode_init(&accnt->sess_node_);

        arec->accnt.state = accnt;
    }
    return accnt;
}

SC_EXTERN void
fig_accnt_term(struct ScRec* arec)
{
    assert(arec);
    assert(arec->type == SC_ENTITY_ACCNT);
    struct FigAccnt* accnt = arec->accnt.state;
    if (accnt) {
        arec->accnt.state = NULL;
        fig_accnt_reset_trader(accnt);
        fig_accnt_reset_giveup(accnt);
        free(accnt);
    }
}

SC_EXTERN void
fig_accnt_reset_trader(struct FigAccnt* accnt)
{
    free_execs(accnt);
    free_orders(accnt);
    free_giveups(accnt);
    free_traders(accnt);
}

SC_EXTERN void
fig_accnt_reset_giveup(struct FigAccnt* accnt)
{
    free_posns(accnt);
}

SC_EXTERN struct ScPosn*
fig_accnt_update_posn(struct FigAccnt* accnt, struct ScPosn* posn)
{
    const ScKey key = sc_posn_key(posn->accnt.rec->id, posn->contr.rec->id, posn->settl_day);
    struct ScRbNode* node = sc_tree_insert(&accnt->posns, key, &posn->accnt_node_);
    if (node != &posn->accnt_node_) {
        struct ScPosn* exist = sc_accnt_posn_entry(node);

        // Update existing position.

        assert(exist->accnt.rec == posn->accnt.rec);
        assert(exist->contr.rec == posn->contr.rec);
        assert(exist->settl_day == posn->settl_day);

        exist->buy_licks = posn->buy_licks;
        exist->buy_lots = posn->buy_lots;
        exist->sell_licks = posn->sell_licks;
        exist->sell_lots = posn->sell_lots;

        sc_pool_free_posn(accnt->pool, posn);
        posn = exist;
    }
    return posn;
}

SC_EXTERN struct ScPosn*
fig_accnt_posn(struct ScRec* arec, struct ScRec* crec, ScJd settl_day,
               struct FigOrdIdx* ordidx, ScPool pool)
{
    assert(arec);
    assert(arec->type == SC_ENTITY_ACCNT);

    assert(crec);
    assert(crec->type == SC_ENTITY_CONTR);

    struct ScPosn* posn;

    struct FigAccnt* accnt = fig_accnt_lazy(arec, ordidx, pool);
    if (sc_unlikely(!accnt))
        return NULL;

    const ScKey key = sc_posn_key(arec->id, crec->id, settl_day);
	struct ScRbNode* node = sc_tree_pfind(&accnt->posns, key);
    if (!node || node->key != key) {
        if (!(posn = sc_pool_alloc_posn(accnt->pool)))
            return NULL;
        sc_posn_init(posn);

        posn->accnt.rec = arec;
        posn->contr.rec = crec;
        posn->settl_day = settl_day;
        posn->buy_licks = 0;
        posn->buy_lots = 0;
        posn->sell_licks = 0;
        posn->sell_lots = 0;

        sc_log_debug2("insert posn: accnt=%.16s, contr=%.16s, settl_day=%d",
                       arec->mnem, crec->mnem, settl_day);

        struct ScRbNode* parent = node;
        sc_tree_pinsert(&accnt->posns, key, &posn->accnt_node_, parent);
    } else
        posn = sc_accnt_posn_entry(node);
    return posn;
}

SC_API struct ScRec*
sc_accnt_rec(ScAccnt accnt)
{
    return fig_accnt_rec(accnt);
}

// AccntTrader

SC_API struct ScRbNode*
sc_accnt_find_trader_id(ScAccnt accnt, ScIden id)
{
    return fig_accnt_find_trader_id(accnt, id);
}

SC_API struct ScRbNode*
sc_accnt_first_trader(ScAccnt accnt)
{
    return fig_accnt_first_trader(accnt);
}

SC_API struct ScRbNode*
sc_accnt_last_trader(ScAccnt accnt)
{
    return fig_accnt_last_trader(accnt);
}

SC_API ScBool
sc_accnt_empty_trader(ScAccnt accnt)
{
    return fig_accnt_empty_trader(accnt);
}

// AccntGiveup

SC_API struct ScRbNode*
sc_accnt_find_giveup_id(ScAccnt accnt, ScIden id)
{
    return fig_accnt_find_giveup_id(accnt, id);
}

SC_API struct ScRbNode*
sc_accnt_first_giveup(ScAccnt accnt)
{
    return fig_accnt_first_giveup(accnt);
}

SC_API struct ScRbNode*
sc_accnt_last_giveup(ScAccnt accnt)
{
    return fig_accnt_last_giveup(accnt);
}

SC_API ScBool
sc_accnt_empty_giveup(ScAccnt accnt)
{
    return fig_accnt_empty_giveup(accnt);
}

// AccntOrder

SC_API struct ScRbNode*
sc_accnt_find_order_id(ScAccnt accnt, ScIden id)
{
    return fig_accnt_find_order_id(accnt, id);
}

SC_API struct ScOrder*
sc_accnt_find_order_ref(ScAccnt accnt, const char* ref)
{
    return fig_accnt_find_order_ref(accnt, ref);
}

SC_API struct ScRbNode*
sc_accnt_first_order(ScAccnt accnt)
{
    return fig_accnt_first_order(accnt);
}

SC_API struct ScRbNode*
sc_accnt_last_order(ScAccnt accnt)
{
    return fig_accnt_last_order(accnt);
}

SC_API ScBool
sc_accnt_empty_order(ScAccnt accnt)
{
    return fig_accnt_empty_order(accnt);
}

// AccntTrade

SC_API struct ScRbNode*
sc_accnt_find_trade_id(ScAccnt accnt, ScIden id)
{
    return fig_accnt_find_trade_id(accnt, id);
}

SC_API struct ScRbNode*
sc_accnt_first_trade(ScAccnt accnt)
{
    return fig_accnt_first_trade(accnt);
}

SC_API struct ScRbNode*
sc_accnt_last_trade(ScAccnt accnt)
{
    return fig_accnt_last_trade(accnt);
}

SC_API ScBool
sc_accnt_empty_trade(ScAccnt accnt)
{
    return fig_accnt_empty_trade(accnt);
}

// AccntPosn.

SC_API struct ScRbNode*
sc_accnt_find_posn_id(ScAccnt accnt, ScIden id)
{
    return fig_accnt_find_posn_id(accnt, id);
}

SC_API struct ScRbNode*
sc_accnt_first_posn(ScAccnt accnt)
{
    return fig_accnt_first_posn(accnt);
}

SC_API struct ScRbNode*
sc_accnt_last_posn(ScAccnt accnt)
{
    return fig_accnt_last_posn(accnt);
}

SC_API ScBool
sc_accnt_empty_posn(ScAccnt accnt)
{
    return fig_accnt_empty_posn(accnt);
}

// Accnt

SC_API ScBool
sc_accnt_logged_on(ScAccnt accnt)
{
    return fig_accnt_logged_on(accnt);
}

SC_API struct ScSess*
sc_accnt_sess(ScAccnt accnt)
{
    return fig_accnt_sess(accnt);
}
