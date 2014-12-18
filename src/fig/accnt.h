/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef FIG_ACCNT_H
#define FIG_ACCNT_H

#include <sc/fig/accnt.h>

#include "ordidx.h"

#include <sc/elm/refcount.h>

#include <sc/ash/tree.h>

struct FigAccnt {
    struct ScRec* rec;
    struct FigOrdIdx* ordidx;
    ScPool pool;
    struct ScTree traders;
    struct ScTree giveups;
    struct ScTree orders;
    struct ScTree trades;
    struct ScTree posns;
    struct ScSess* sess;
    // Private section.
    struct ScRbNode sess_node_;
};

SC_EXTERN struct FigAccnt*
fig_accnt_lazy(struct ScRec* arec, struct FigOrdIdx* ordidx, ScPool pool);

// Assumes that arec pointer is not null.

SC_EXTERN void
fig_accnt_term(struct ScRec* arec);

SC_EXTERN void
fig_accnt_reset_trader(struct FigAccnt* accnt);

SC_EXTERN void
fig_accnt_reset_giveup(struct FigAccnt* accnt);

static inline struct ScRec*
fig_accnt_rec(const struct FigAccnt* accnt)
{
    return accnt->rec;
}

// Trader.

#define FIG_ACCNT_END_TRADER SC_TREE_END

// Transfer ownership to state.

static inline void
fig_accnt_emplace_trader(struct FigAccnt* accnt, struct ScPerm* perm)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct ScRbNode* node = sc_tree_insert(&accnt->traders, perm->trader.rec->id,
                                                 &perm->giveup_node_);
        assert(node == &perm->giveup_node_);
    }
#pragma GCC diagnostic pop
}

static inline struct ScRbNode*
fig_accnt_find_trader_id(const struct FigAccnt* accnt, ScIden id)
{
    return sc_tree_find(&accnt->traders, id);
}

static inline struct ScRbNode*
fig_accnt_first_trader(const struct FigAccnt* accnt)
{
    return sc_tree_first(&accnt->traders);
}

static inline struct ScRbNode*
fig_accnt_last_trader(const struct FigAccnt* accnt)
{
    return sc_tree_last(&accnt->traders);
}

static inline ScBool
fig_accnt_empty_trader(const struct FigAccnt* accnt)
{
    return sc_tree_empty(&accnt->traders);
}

// Giveup.

#define FIG_ACCNT_END_GIVEUP SC_TREE_END

// Transfer ownership to state.

static inline void
fig_accnt_emplace_giveup(struct FigAccnt* accnt, struct ScPerm* perm)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct ScRbNode* node = sc_tree_insert(&accnt->giveups, perm->giveup.rec->id,
                                                 &perm->trader_node_);
        assert(node == &perm->trader_node_);
    }
#pragma GCC diagnostic pop
}

static inline struct ScRbNode*
fig_accnt_find_giveup_id(const struct FigAccnt* accnt, ScIden id)
{
    return sc_tree_find(&accnt->giveups, id);
}

static inline struct ScRbNode*
fig_accnt_first_giveup(const struct FigAccnt* accnt)
{
    return sc_tree_first(&accnt->giveups);
}

static inline struct ScRbNode*
fig_accnt_last_giveup(const struct FigAccnt* accnt)
{
    return sc_tree_last(&accnt->giveups);
}

static inline ScBool
fig_accnt_empty_giveup(const struct FigAccnt* accnt)
{
    return sc_tree_empty(&accnt->giveups);
}

// Order.

#define FIG_ACCNT_END_ORDER SC_TREE_END

// Transfer ownership to state.

static inline void
fig_accnt_emplace_order(struct FigAccnt* accnt, struct ScOrder* order)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct ScRbNode* node = sc_tree_insert(&accnt->orders, order->id, &order->accnt_node_);
        assert(node == &order->accnt_node_);
    }
#pragma GCC diagnostic pop
    if (order->i.ref[0] != '\0')
        fig_ordidx_insert(accnt->ordidx, order);
}

// Release ownership from state.

static inline void
fig_accnt_release_order(struct FigAccnt* accnt, struct ScOrder* order)
{
    sc_tree_remove(&accnt->orders, &order->accnt_node_);
    sc_rbnode_init(&order->accnt_node_);
    if (order->i.ref[0] != '\0')
        fig_ordidx_remove(accnt->ordidx, accnt->rec->id, order->i.ref);
}

// Release ownership from state.

static inline struct ScOrder*
fig_accnt_release_order_id(struct FigAccnt* accnt, ScIden id)
{
    struct ScRbNode* node = sc_tree_find(&accnt->orders, id);
    if (!node)
        return NULL;
    struct ScOrder* order = sc_accnt_order_entry(node);
    fig_accnt_release_order(accnt, order);
    return order;
}

// Release ownership from state.

static inline struct ScOrder*
fig_accnt_release_order_ref(struct FigAccnt* accnt, const char* ref)
{
    assert(ref);
    struct ScOrder* order = fig_ordidx_remove(accnt->ordidx, accnt->rec->id, ref);
    if (order) {
        sc_tree_remove(&accnt->orders, &order->accnt_node_);
        sc_rbnode_init(&order->accnt_node_);
    }
    return order;
}

static inline struct ScRbNode*
fig_accnt_find_order_id(const struct FigAccnt* accnt, ScIden id)
{
    return sc_tree_find(&accnt->orders, id);
}

// Returns order directly because hash lookup is not a node-based container.

static inline struct ScOrder*
fig_accnt_find_order_ref(const struct FigAccnt* accnt, const char* ref)
{
    assert(ref);
    return fig_ordidx_find(accnt->ordidx, accnt->rec->id, ref);
}

static inline struct ScRbNode*
fig_accnt_first_order(const struct FigAccnt* accnt)
{
    return sc_tree_first(&accnt->orders);
}

static inline struct ScRbNode*
fig_accnt_last_order(const struct FigAccnt* accnt)
{
    return sc_tree_last(&accnt->orders);
}

static inline ScBool
fig_accnt_empty_order(const struct FigAccnt* accnt)
{
    return sc_tree_empty(&accnt->orders);
}

// Trade.

#define FIG_ACCNT_END_TRADE SC_TREE_END

static inline void
fig_accnt_insert_trade(struct FigAccnt* accnt, struct ScExec* exec)
{
    sc_exec_incref(exec);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct ScRbNode* node = sc_tree_insert(&accnt->trades, exec->id, &exec->accnt_node_);
        assert(node == &exec->accnt_node_);
    }
#pragma GCC diagnostic pop
}

static inline void
fig_accnt_remove_trade(struct FigAccnt* accnt, struct ScExec* exec)
{
    sc_tree_remove(&accnt->trades, &exec->accnt_node_);
    sc_rbnode_init(&exec->accnt_node_);
    sc_exec_decref(exec, accnt->pool);
}

// Release ownership from state.

static inline ScBool
fig_accnt_remove_trade_id(struct FigAccnt* accnt, ScIden id)
{
    struct ScRbNode* node = sc_tree_find(&accnt->trades, id);
    if (!node)
        return SC_FALSE;

    struct ScExec* exec = sc_accnt_trade_entry(node);
    fig_accnt_remove_trade(accnt, exec);
    return SC_TRUE;
}

static inline struct ScRbNode*
fig_accnt_find_trade_id(const struct FigAccnt* accnt, ScIden id)
{
    return sc_tree_find(&accnt->trades, id);
}

static inline struct ScRbNode*
fig_accnt_first_trade(const struct FigAccnt* accnt)
{
    return sc_tree_first(&accnt->trades);
}

static inline struct ScRbNode*
fig_accnt_last_trade(const struct FigAccnt* accnt)
{
    return sc_tree_last(&accnt->trades);
}

static inline ScBool
fig_accnt_empty_trade(const struct FigAccnt* accnt)
{
    return sc_tree_empty(&accnt->trades);
}

// Posn.

#define FIG_ACCNT_END_POSN SC_TREE_END

static inline void
fig_accnt_emplace_posn(struct FigAccnt* accnt, struct ScPosn* posn)
{
    const ScKey key = sc_posn_key(posn->accnt.rec->id, posn->contr.rec->id, posn->settl_day);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct ScRbNode* node = sc_tree_insert(&accnt->posns, key, &posn->accnt_node_);
        assert(node == &posn->accnt_node_);
    }
#pragma GCC diagnostic pop
}

SC_EXTERN struct ScPosn*
fig_accnt_update_posn(struct FigAccnt* accnt, struct ScPosn* posn);

SC_EXTERN struct ScPosn*
fig_accnt_posn(struct ScRec* arec, struct ScRec* crec, ScJd settl_day,
               struct FigOrdIdx* ordidx, ScPool pool);

static inline struct ScRbNode*
fig_accnt_find_posn_id(const struct FigAccnt* accnt, ScIden id)
{
    return sc_tree_find(&accnt->posns, id);
}

static inline struct ScRbNode*
fig_accnt_first_posn(const struct FigAccnt* accnt)
{
    return sc_tree_first(&accnt->posns);
}

static inline struct ScRbNode*
fig_accnt_last_posn(const struct FigAccnt* accnt)
{
    return sc_tree_last(&accnt->posns);
}

static inline ScBool
fig_accnt_empty_posn(const struct FigAccnt* accnt)
{
    return sc_tree_empty(&accnt->posns);
}

// Accnt.

static inline ScBool
fig_accnt_logged_on(const struct FigAccnt* accnt)
{
    return accnt->sess != NULL;
}

static inline struct ScSess*
fig_accnt_sess(const struct FigAccnt* accnt)
{
    return accnt->sess;
}

#endif // FIG_ACCNT_H
