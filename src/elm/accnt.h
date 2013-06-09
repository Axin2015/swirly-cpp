/**
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
#ifndef ELM_ACCNT_H
#define ELM_ACCNT_H

#include <dbr/conv.h>

#include <ash/tree.h>

struct ElmAccnt {
    DbrIden id;
    struct ElmCtx* ctx;
    struct AshTree membs;
    struct AshTree trades;
    struct AshTree posns;
    // Useful for drop-copy sessions.
    DbrAccntSess sess;
};

DBR_EXTERN struct ElmAccnt*
elm_accnt_lazy(struct DbrRec* arec, struct ElmCtx* ctx);

// Assumes that arec pointer is not null.

DBR_EXTERN void
elm_accnt_term(struct DbrRec* arec);

static inline DbrIden
elm_accnt_id(const struct ElmAccnt* accnt)
{
    return accnt->id;
}

// Memb.

static inline void
elm_accnt_emplace_memb(struct ElmAccnt* accnt, struct DbrMemb* memb)
{
    ash_tree_insert(&accnt->membs, &memb->accnt_node_);
}

// Trade.

// Transfer ownership to state.

static inline void
elm_accnt_emplace_trade(struct ElmAccnt* accnt, struct DbrTrade* trade)
{
    ash_tree_insert(&accnt->trades, &trade->accnt_node_);
}

// Release ownership from state.

static inline void
elm_accnt_release_trade(struct ElmAccnt* accnt, struct DbrTrade* trade)
{
    ash_tree_remove(&accnt->trades, &trade->accnt_node_);
}

// Release ownership from state.

static inline struct DbrTrade*
elm_accnt_release_trade_id(struct ElmAccnt* accnt, DbrIden id)
{
    struct DbrRbNode* node = ash_tree_find(&accnt->trades, id);
    if (!node)
        return NULL;
    struct DbrTrade* trade = dbr_accnt_trade_entry(node);
    elm_accnt_release_trade(accnt, trade);
    return trade;
}

static inline struct DbrRbNode*
elm_accnt_find_trade_id(const struct ElmAccnt* accnt, DbrIden id)
{
    return ash_tree_find(&accnt->trades, id);
}

static inline struct DbrRbNode*
elm_accnt_first_trade(const struct ElmAccnt* accnt)
{
    return ash_tree_first(&accnt->trades);
}

static inline struct DbrRbNode*
elm_accnt_last_trade(const struct ElmAccnt* accnt)
{
    return ash_tree_last(&accnt->trades);
}

static inline struct DbrRbNode*
elm_accnt_end_trade(const struct ElmAccnt* accnt)
{
    return ash_tree_end(&accnt->trades);
}

static inline DbrBool
elm_accnt_empty_trade(const struct ElmAccnt* accnt)
{
    return ash_tree_empty(&accnt->trades);
}

// Posn.

static inline void
elm_accnt_emplace_posn(struct ElmAccnt* accnt, struct DbrPosn* posn)
{
    ash_tree_insert(&accnt->posns, &posn->accnt_node_);
}

DBR_EXTERN struct DbrPosn*
elm_accnt_posn(struct DbrRec* arec, struct DbrRec* irec, DbrDate settl_date, struct ElmCtx* ctx);

static inline struct DbrRbNode*
elm_accnt_find_posn_id(const struct ElmAccnt* accnt, DbrIden id)
{
    return ash_tree_find(&accnt->posns, id);
}

static inline struct DbrRbNode*
elm_accnt_first_posn(const struct ElmAccnt* accnt)
{
    return ash_tree_first(&accnt->posns);
}

static inline struct DbrRbNode*
elm_accnt_last_posn(const struct ElmAccnt* accnt)
{
    return ash_tree_last(&accnt->posns);
}

static inline struct DbrRbNode*
elm_accnt_end_posn(const struct ElmAccnt* accnt)
{
    return ash_tree_end(&accnt->posns);
}

static inline DbrBool
elm_accnt_empty_posn(const struct ElmAccnt* accnt)
{
    return ash_tree_empty(&accnt->posns);
}

// Sess.

DBR_EXTERN void
elm_accnt_set_sess(struct ElmAccnt* accnt, DbrAccntSess sess);

static inline DbrAccntSess
elm_accnt_sess(const struct ElmAccnt* accnt)
{
    return accnt->sess;
}

#endif // ELM_ACCNT_H
