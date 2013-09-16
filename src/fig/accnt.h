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
#ifndef FIG_ACCNT_H
#define FIG_ACCNT_H

#include <dbr/accnt.h>
#include <dbr/pool.h>
#include <dbr/tree.h>
#include <dbr/types.h>

struct FigAccnt {
    DbrIden id;
    DbrPool pool;
    struct DbrTree membs;
    struct DbrTree trades;
    struct DbrTree posns;
    // Useful for drop-copy sessions.
    DbrAccntSess sess;
};

DBR_EXTERN struct FigAccnt*
fig_accnt_lazy(struct DbrRec* arec, DbrPool pool);

// Assumes that arec pointer is not null.

DBR_EXTERN void
fig_accnt_term(struct DbrRec* arec);

static inline DbrIden
fig_accnt_id(const struct FigAccnt* accnt)
{
    return accnt->id;
}

// Memb.

static inline void
fig_accnt_emplace_memb(struct FigAccnt* accnt, struct DbrMemb* memb)
{
    dbr_tree_insert(&accnt->membs, memb->trader.rec->id, &memb->accnt_node_);
}

// Trade.

// Transfer ownership to state.

static inline void
fig_accnt_emplace_trade(struct FigAccnt* accnt, struct DbrTrade* trade)
{
    dbr_tree_insert(&accnt->trades, trade->id, &trade->accnt_node_);
}

// Release ownership from state.

static inline void
fig_accnt_release_trade(struct FigAccnt* accnt, struct DbrTrade* trade)
{
    dbr_tree_remove(&accnt->trades, &trade->accnt_node_);
    dbr_rbnode_init(&trade->accnt_node_);
}

// Release ownership from state.

static inline struct DbrTrade*
fig_accnt_release_trade_id(struct FigAccnt* accnt, DbrIden id)
{
    struct DbrRbNode* node = dbr_tree_find(&accnt->trades, id);
    if (!node)
        return NULL;
    struct DbrTrade* trade = dbr_accnt_trade_entry(node);
    fig_accnt_release_trade(accnt, trade);
    return trade;
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

static inline struct DbrRbNode*
fig_accnt_end_trade(const struct FigAccnt* accnt)
{
    return dbr_tree_end(&accnt->trades);
}

static inline DbrBool
fig_accnt_empty_trade(const struct FigAccnt* accnt)
{
    return dbr_tree_empty(&accnt->trades);
}

// Posn.

static inline void
fig_accnt_emplace_posn(struct FigAccnt* accnt, struct DbrPosn* posn)
{
    // Synthetic key from contract and settlment date.
    const DbrIden key = posn->contr.rec->id * 100000000L + posn->settl_date;
    dbr_tree_insert(&accnt->posns, key, &posn->accnt_node_);
}

DBR_EXTERN struct DbrPosn*
fig_accnt_posn(struct DbrRec* arec, struct DbrRec* crec, DbrDate settl_date, DbrPool pool);

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

static inline struct DbrRbNode*
fig_accnt_end_posn(const struct FigAccnt* accnt)
{
    return dbr_tree_end(&accnt->posns);
}

static inline DbrBool
fig_accnt_empty_posn(const struct FigAccnt* accnt)
{
    return dbr_tree_empty(&accnt->posns);
}

// Sess.

DBR_EXTERN void
fig_accnt_set_sess(struct FigAccnt* accnt, DbrAccntSess sess);

static inline DbrAccntSess
fig_accnt_sess(const struct FigAccnt* accnt)
{
    return accnt->sess;
}

#endif // FIG_ACCNT_H
