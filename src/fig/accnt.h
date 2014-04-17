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

#include <dbr/accnt.h>

#include "ordidx.h"

#include <dbr/conv.h> // dbr_book_key()
#include <dbr/refcount.h>
#include <dbr/tree.h>

struct FigAccnt {
    struct DbrRec* rec;
    struct FigOrdIdx* ordidx;
    DbrPool pool;
    struct DbrTree users;
    struct DbrTree groups;
    struct DbrTree orders;
    struct DbrTree trades;
    struct DbrTree posns;
    struct DbrSess* sess;
    // Private section.
    struct DbrRbNode sess_node_;
};

DBR_EXTERN struct FigAccnt*
fig_accnt_lazy(struct DbrRec* arec, struct FigOrdIdx* ordidx, DbrPool pool);

// Assumes that arec pointer is not null.

DBR_EXTERN void
fig_accnt_term(struct DbrRec* arec);

DBR_EXTERN void
fig_accnt_clear(struct FigAccnt* accnt);

static inline struct DbrRec*
fig_accnt_rec(const struct FigAccnt* accnt)
{
    return accnt->rec;
}

// User.

#define FIG_ACCNT_END_USER DBR_TREE_END

// Transfer ownership to state.

static inline void
fig_accnt_emplace_user(struct FigAccnt* accnt, struct DbrMemb* memb)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct DbrRbNode* node = dbr_tree_insert(&accnt->users, memb->user.rec->id,
                                                 &memb->group_node_);
        assert(node == &memb->group_node_);
    }
#pragma GCC diagnostic pop
}

static inline struct DbrRbNode*
fig_accnt_find_user_id(const struct FigAccnt* accnt, DbrIden id)
{
    return dbr_tree_find(&accnt->users, id);
}

static inline struct DbrRbNode*
fig_accnt_first_user(const struct FigAccnt* accnt)
{
    return dbr_tree_first(&accnt->users);
}

static inline struct DbrRbNode*
fig_accnt_last_user(const struct FigAccnt* accnt)
{
    return dbr_tree_last(&accnt->users);
}

static inline DbrBool
fig_accnt_empty_user(const struct FigAccnt* accnt)
{
    return dbr_tree_empty(&accnt->users);
}

// Group.

#define FIG_ACCNT_END_GROUP DBR_TREE_END

// Transfer ownership to state.

static inline void
fig_accnt_emplace_group(struct FigAccnt* accnt, struct DbrMemb* memb)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    {
        struct DbrRbNode* node = dbr_tree_insert(&accnt->groups, memb->group.rec->id,
                                                 &memb->user_node_);
        assert(node == &memb->user_node_);
    }
#pragma GCC diagnostic pop
}

static inline struct DbrRbNode*
fig_accnt_find_group_id(const struct FigAccnt* accnt, DbrIden id)
{
    return dbr_tree_find(&accnt->groups, id);
}

static inline struct DbrRbNode*
fig_accnt_first_group(const struct FigAccnt* accnt)
{
    return dbr_tree_first(&accnt->groups);
}

static inline struct DbrRbNode*
fig_accnt_last_group(const struct FigAccnt* accnt)
{
    return dbr_tree_last(&accnt->groups);
}

static inline DbrBool
fig_accnt_empty_group(const struct FigAccnt* accnt)
{
    return dbr_tree_empty(&accnt->groups);
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
    if (order->c.ref[0] != '\0')
        fig_ordidx_insert(accnt->ordidx, order);
}

// Release ownership from state.

static inline void
fig_accnt_release_order(struct FigAccnt* accnt, struct DbrOrder* order)
{
    dbr_tree_remove(&accnt->orders, &order->accnt_node_);
    dbr_rbnode_init(&order->accnt_node_);
    if (order->c.ref[0] != '\0')
        fig_ordidx_remove(accnt->ordidx, accnt->rec->id, order->c.ref);
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
    struct DbrOrder* order = fig_ordidx_remove(accnt->ordidx, accnt->rec->id, ref);
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
    return fig_ordidx_find(accnt->ordidx, accnt->rec->id, ref);
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
    // Synthetic key from contract and settlment date.
    const DbrIden key = dbr_book_key(posn->contr.rec->id, posn->settl_date);
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
fig_accnt_posn(struct DbrRec* arec, struct DbrRec* crec, DbrDate settl_date,
               struct FigOrdIdx* ordidx, DbrPool pool);

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
