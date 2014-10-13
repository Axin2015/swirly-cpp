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
#ifndef FIG_ACCNT_H
#define FIG_ACCNT_H

#include <dbr/fig/accnt.h>

#include "index.h"

#include <dbr/elm/refcount.h>

#include <dbr/ash/tree.h>

struct FigAccnt {
    struct DbrRec* rec;
    struct FigIndex* index;
    DbrPool pool;
    struct DbrTree traders;
    struct DbrTree giveups;
    struct DbrTree orders;
    struct DbrTree trades;
    struct DbrTree posns;
    struct DbrSess* sess;
    // Private section.
    struct DbrRbNode sess_node_;
};

DBR_EXTERN struct FigAccnt*
fig_accnt_lazy(struct DbrRec* arec, struct FigIndex* index, DbrPool pool);

// Assumes that arec pointer is not null.

DBR_EXTERN void
fig_accnt_term(struct DbrRec* arec);

DBR_EXTERN void
fig_accnt_reset_trader(struct FigAccnt* accnt);

DBR_EXTERN void
fig_accnt_reset_giveup(struct FigAccnt* accnt);

static inline struct DbrRec*
fig_accnt_rec(const struct FigAccnt* accnt)
{
    return accnt->rec;
}

// Trader.

#define FIG_ACCNT_END_TRADER DBR_TREE_END

// Transfer ownership to state.

static inline void
fig_accnt_emplace_trader(struct FigAccnt* accnt, struct DbrPerm* perm)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct DbrRbNode* node = dbr_tree_insert(&accnt->traders, perm->trader.rec->id,
                                                 &perm->giveup_node_);
        assert(node == &perm->giveup_node_);
    }
#pragma GCC diagnostic pop
}

static inline struct DbrRbNode*
fig_accnt_find_trader_id(const struct FigAccnt* accnt, DbrIden id)
{
    return dbr_tree_find(&accnt->traders, id);
}

static inline struct DbrRbNode*
fig_accnt_first_trader(const struct FigAccnt* accnt)
{
    return dbr_tree_first(&accnt->traders);
}

static inline struct DbrRbNode*
fig_accnt_last_trader(const struct FigAccnt* accnt)
{
    return dbr_tree_last(&accnt->traders);
}

static inline DbrBool
fig_accnt_empty_trader(const struct FigAccnt* accnt)
{
    return dbr_tree_empty(&accnt->traders);
}

// Giveup.

#define FIG_ACCNT_END_GIVEUP DBR_TREE_END

// Transfer ownership to state.

static inline void
fig_accnt_emplace_giveup(struct FigAccnt* accnt, struct DbrPerm* perm)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct DbrRbNode* node = dbr_tree_insert(&accnt->giveups, perm->giveup.rec->id,
                                                 &perm->trader_node_);
        assert(node == &perm->trader_node_);
    }
#pragma GCC diagnostic pop
}

static inline struct DbrRbNode*
fig_accnt_find_giveup_id(const struct FigAccnt* accnt, DbrIden id)
{
    return dbr_tree_find(&accnt->giveups, id);
}

static inline struct DbrRbNode*
fig_accnt_first_giveup(const struct FigAccnt* accnt)
{
    return dbr_tree_first(&accnt->giveups);
}

static inline struct DbrRbNode*
fig_accnt_last_giveup(const struct FigAccnt* accnt)
{
    return dbr_tree_last(&accnt->giveups);
}

static inline DbrBool
fig_accnt_empty_giveup(const struct FigAccnt* accnt)
{
    return dbr_tree_empty(&accnt->giveups);
}

// Order.

#define FIG_ACCNT_END_ORDER DBR_TREE_END

// Transfer ownership to state.

static inline void
fig_accnt_emplace_order(struct FigAccnt* accnt, struct DbrOrder* order)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct DbrRbNode* node = dbr_tree_insert(&accnt->orders, order->id, &order->accnt_node_);
        assert(node == &order->accnt_node_);
    }
#pragma GCC diagnostic pop
    if (order->i.ref[0] != '\0')
        fig_index_insert(accnt->index, order);
}

// Release ownership from state.

static inline void
fig_accnt_release_order(struct FigAccnt* accnt, struct DbrOrder* order)
{
    dbr_tree_remove(&accnt->orders, &order->accnt_node_);
    dbr_rbnode_init(&order->accnt_node_);
    if (order->i.ref[0] != '\0')
        fig_index_remove(accnt->index, accnt->rec->id, order->i.ref);
}

// Release ownership from state.

static inline struct DbrOrder*
fig_accnt_release_order_id(struct FigAccnt* accnt, DbrIden id)
{
    struct DbrRbNode* node = dbr_tree_find(&accnt->orders, id);
    if (!node)
        return NULL;
    struct DbrOrder* order = dbr_accnt_order_entry(node);
    fig_accnt_release_order(accnt, order);
    return order;
}

// Release ownership from state.

static inline struct DbrOrder*
fig_accnt_release_order_ref(struct FigAccnt* accnt, const char* ref)
{
    assert(ref);
    struct DbrOrder* order = fig_index_remove(accnt->index, accnt->rec->id, ref);
    if (order) {
        dbr_tree_remove(&accnt->orders, &order->accnt_node_);
        dbr_rbnode_init(&order->accnt_node_);
    }
    return order;
}

static inline struct DbrRbNode*
fig_accnt_find_order_id(const struct FigAccnt* accnt, DbrIden id)
{
    return dbr_tree_find(&accnt->orders, id);
}

// Returns order directly because hash lookup is not a node-based container.

static inline struct DbrOrder*
fig_accnt_find_order_ref(const struct FigAccnt* accnt, const char* ref)
{
    assert(ref);
    return fig_index_find(accnt->index, accnt->rec->id, ref);
}

static inline struct DbrRbNode*
fig_accnt_first_order(const struct FigAccnt* accnt)
{
    return dbr_tree_first(&accnt->orders);
}

static inline struct DbrRbNode*
fig_accnt_last_order(const struct FigAccnt* accnt)
{
    return dbr_tree_last(&accnt->orders);
}

static inline DbrBool
fig_accnt_empty_order(const struct FigAccnt* accnt)
{
    return dbr_tree_empty(&accnt->orders);
}

// Trade.

#define FIG_ACCNT_END_TRADE DBR_TREE_END

static inline void
fig_accnt_insert_trade(struct FigAccnt* accnt, struct DbrExec* exec)
{
    dbr_exec_incref(exec);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct DbrRbNode* node = dbr_tree_insert(&accnt->trades, exec->id, &exec->accnt_node_);
        assert(node == &exec->accnt_node_);
    }
#pragma GCC diagnostic pop
}

static inline void
fig_accnt_remove_trade(struct FigAccnt* accnt, struct DbrExec* exec)
{
    dbr_tree_remove(&accnt->trades, &exec->accnt_node_);
    dbr_rbnode_init(&exec->accnt_node_);
    dbr_exec_decref(exec, accnt->pool);
}

// Release ownership from state.

static inline DbrBool
fig_accnt_remove_trade_id(struct FigAccnt* accnt, DbrIden id)
{
    struct DbrRbNode* node = dbr_tree_find(&accnt->trades, id);
    if (!node)
        return DBR_FALSE;

    struct DbrExec* exec = dbr_accnt_trade_entry(node);
    fig_accnt_remove_trade(accnt, exec);
    return DBR_TRUE;
}

static inline struct DbrRbNode*
fig_accnt_find_trade_id(const struct FigAccnt* accnt, DbrIden id)
{
    return dbr_tree_find(&accnt->trades, id);
}

static inline struct DbrRbNode*
fig_accnt_first_trade(const struct FigAccnt* accnt)
{
    return dbr_tree_first(&accnt->trades);
}

static inline struct DbrRbNode*
fig_accnt_last_trade(const struct FigAccnt* accnt)
{
    return dbr_tree_last(&accnt->trades);
}

static inline DbrBool
fig_accnt_empty_trade(const struct FigAccnt* accnt)
{
    return dbr_tree_empty(&accnt->trades);
}

// Posn.

#define FIG_ACCNT_END_POSN DBR_TREE_END

static inline void
fig_accnt_emplace_posn(struct FigAccnt* accnt, struct DbrPosn* posn)
{
    const DbrKey key = dbr_posn_key(posn->accnt.rec->id, posn->contr.rec->id, posn->settl_day);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct DbrRbNode* node = dbr_tree_insert(&accnt->posns, key, &posn->accnt_node_);
        assert(node == &posn->accnt_node_);
    }
#pragma GCC diagnostic pop
}

DBR_EXTERN struct DbrPosn*
fig_accnt_update_posn(struct FigAccnt* accnt, struct DbrPosn* posn);

DBR_EXTERN struct DbrPosn*
fig_accnt_posn(struct DbrRec* arec, struct DbrRec* crec, DbrJd settl_day,
               struct FigIndex* index, DbrPool pool);

static inline struct DbrRbNode*
fig_accnt_find_posn_id(const struct FigAccnt* accnt, DbrIden id)
{
    return dbr_tree_find(&accnt->posns, id);
}

static inline struct DbrRbNode*
fig_accnt_first_posn(const struct FigAccnt* accnt)
{
    return dbr_tree_first(&accnt->posns);
}

static inline struct DbrRbNode*
fig_accnt_last_posn(const struct FigAccnt* accnt)
{
    return dbr_tree_last(&accnt->posns);
}

static inline DbrBool
fig_accnt_empty_posn(const struct FigAccnt* accnt)
{
    return dbr_tree_empty(&accnt->posns);
}

// Accnt.

static inline DbrBool
fig_accnt_logged_on(const struct FigAccnt* accnt)
{
    return accnt->sess != NULL;
}

static inline struct DbrSess*
fig_accnt_sess(const struct FigAccnt* accnt)
{
    return accnt->sess;
}

#endif // FIG_ACCNT_H
