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
#ifndef DBR_TYPES_H
#define DBR_TYPES_H

#include <dbr/dlnode.h>
#include <dbr/queue.h>
#include <dbr/rbnode.h>
#include <dbr/slnode.h>

#include <sys/types.h> // ssize_t

/**
 * @addtogroup Types
 * @{
 */

/**
 * ISO8601 format: "%Y%m%d".
 */

typedef int DbrDate;
typedef long DbrIncs;

/**
 * Milliseconds since Unix epoch.
 */

typedef long DbrMillis;

typedef DbrIncs DbrLots;

/**
 * Unit representing the minimum price increment.
 */

typedef DbrIncs DbrTicks;

/**
 * Sum of lots and ticks.
 */

typedef DbrIncs DbrLicks;

enum DbrEntity {
    DBR_TRADER = 0x01,
    DBR_ACCNT  = 0x02,
    DBR_CONTR  = 0x04,
    DBR_ORDER  = 0x08,
    DBR_EXEC   = 0x10,
    DBR_MEMB   = 0x20,
    DBR_POSN   = 0x40
};

enum {
    DBR_VIEW   = 0x80
};

enum {
    DBR_DISPLAY_MAX = 64,
    DBR_EMAIL_MAX = 64,
    DBR_MNEM_MAX = 16,
    DBR_REF_MAX = 64
};

/**
 * Description suitable for display on user-interface.
 */

typedef char DbrDisplay[DBR_DISPLAY_MAX];

/**
 * Email address.
 */

typedef char DbrEmail[DBR_EMAIL_MAX];

/**
 * Memorable identifier.
 */

typedef char DbrMnem[DBR_MNEM_MAX];

/**
 * Reference.
 */

typedef char DbrRef[DBR_REF_MAX];

enum DbrAction {
    DBR_BUY = 1,
    DBR_SELL = -1
};

/** @} */

/**
 * @addtogroup Trader
 */

typedef struct FigTrader* DbrTrader;

/**
 * @addtogroup Accnt
 */

typedef struct FigAccnt* DbrAccnt;

/**
 * @addtogroup Data
 * @{
 */

/**
 * Union used for record enrichment.
 */

union DbrURec {
    DbrIden id_only;
    struct DbrRec* rec;
};

struct DbrRec {
    /**
     * @publicsection
     */
    int type;
    DbrIden id;
    DbrMnem mnem;
    DbrDisplay display;
    union {
        struct {
            DbrEmail email;
            DbrTrader state;
        } trader;
        struct {
            DbrEmail email;
            DbrAccnt state;
        } accnt;
        struct {
            DbrMnem asset_type;
            DbrMnem asset;
            DbrMnem ccy;
            int tick_numer;
            int tick_denom;
            // Transient.
            double price_inc;
            int lot_numer;
            int lot_denom;
            // Transient.
            double qty_inc;
            // Transient.
            int price_dp;
            int pip_dp;
            // Transient.
            int qty_dp;
            DbrLots min_lots;
            DbrLots max_lots;
        } contr;
    };
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct DbrSlNode shared_node_;
    // Cache nodes.
    struct DbrSlNode id_node_;
    struct DbrSlNode mnem_node_;
};

static inline void
dbr_rec_init(struct DbrRec* rec)
{
    dbr_slnode_init(&rec->shared_node_);
    dbr_slnode_init(&rec->id_node_);
    dbr_slnode_init(&rec->mnem_node_);
}

static inline struct DbrRec*
dbr_shared_rec_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrRec, shared_node_, node);
}

/** @} */

/**
 * @addtogroup Model
 * @{
 */

typedef struct DbrIModel {
    const struct DbrModelVtbl* vtbl;
}* DbrModel;

/** @} */

/**
 * @addtogroup Journ
 * @{
 */

typedef struct DbrIJourn {
    const struct DbrJournVtbl* vtbl;
}* DbrJourn;

/** @} */

/**
 * @addtogroup SideLevel
 * @{
 */

struct DbrLevel {
    /**
     * @publicsection
     */
    struct DbrOrder* first_order;
    DbrTicks ticks;
    // Must be greater than zero.
    DbrLots lots;
    // Must be greater than zero.
    size_t count;
    /**
     * @privatesection
     */
    struct DbrRbNode side_node_;
};

static inline void
dbr_level_init(struct DbrLevel* level)
{
    dbr_rbnode_init(&level->side_node_);
}

/** @} */

/**
 * @addtogroup TraderMemb
 * @{
 */

struct DbrMemb {
    /**
     * @publicsection
     */
    union DbrURec trader;
    union DbrURec accnt;
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct DbrSlNode shared_node_;
    struct DbrRbNode trader_node_;
    struct DbrRbNode accnt_node_;
};

static inline void
dbr_memb_init(struct DbrMemb* memb)
{
    dbr_slnode_init(&memb->shared_node_);
    dbr_rbnode_init(&memb->trader_node_);
}

static inline struct DbrMemb*
dbr_shared_memb_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrMemb, shared_node_, node);
}

/** @} */

/**
 * @addtogroup AccntPosn
 * @{
 */

struct DbrPosn {
    /**
     * @publicsection
     */
    union DbrURec accnt;
    union DbrURec contr;
    DbrDate settl_date;
    DbrLicks buy_licks;
    DbrLots buy_lots;
    DbrLicks sell_licks;
    DbrLots sell_lots;
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct DbrSlNode shared_node_;
    struct DbrRbNode accnt_node_;
    struct DbrRbNode update_node_;
};

static inline void
dbr_posn_init(struct DbrPosn* posn)
{
    dbr_slnode_init(&posn->shared_node_);
    dbr_rbnode_init(&posn->accnt_node_);
    dbr_rbnode_init(&posn->update_node_);
}

static inline struct DbrPosn*
dbr_shared_posn_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrPosn, shared_node_, node);
}

/** @} */

/**
 * @addtogroup Exec
 * @{
 */

enum DbrState {
    DBR_NEW = 1,
    DBR_REVISE,
    DBR_CANCEL,
    DBR_TRADE
};

struct DbrCommon {
    union DbrURec trader;
    union DbrURec accnt;
    union DbrURec contr;
    DbrDate settl_date;
    // Ref is optional.
    DbrRef ref;
    /**
     * @sa enum DbrState
     */
    int state;
    /**
     * @sa enum DbrAction
     */
    int action;
    DbrTicks ticks;
    // Must be greater than zero.
    DbrLots lots;
    // Must be greater than zero.
    DbrLots resd;
    // Must not be greater that lots.
    DbrLots exec;
    DbrTicks last_ticks;
    DbrLots last_lots;
    // Minimum to be filled by this order.
    DbrLots min_lots;
};

struct DbrOrder {
    /**
     * @publicsection
     */
    // Set when order is associated with book.
    struct DbrLevel* level;
    DbrIden id;
    struct DbrCommon c;
    DbrMillis created;
    DbrMillis modified;
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct DbrSlNode shared_node_;
    // Singly-linked for index.
    struct DbrSlNode ref_node_;
    // Doubly-linked for side.
    struct DbrDlNode side_node_;
    struct DbrRbNode trader_node_;
};

static inline void
dbr_order_init(struct DbrOrder* order)
{
    dbr_slnode_init(&order->shared_node_);
    dbr_slnode_init(&order->ref_node_);
    dbr_dlnode_init(&order->side_node_);
    dbr_rbnode_init(&order->trader_node_);
}

static inline DbrBool
dbr_order_done(const struct DbrOrder* order)
{
    return order->c.resd == 0;
}

static inline struct DbrOrder*
dbr_shared_order_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrOrder, shared_node_, node);
}

enum DbrRole {
    DBR_MAKER = 1,
    DBR_TAKER
};

struct DbrExec {
    /**
     * @publicsection
     */
    DbrIden id;
    DbrIden order;
    struct DbrCommon c;
    DbrIden match;
    int locks_;
    /**
     * @sa enum DbrRole
     */
    int role;
    union DbrURec cpty;
    DbrMillis created;
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct DbrSlNode shared_node_;
    struct DbrRbNode trader_node_;
};

static inline void
dbr_exec_init(struct DbrExec* exec)
{
    exec->locks_ = 0;
    dbr_slnode_init(&exec->shared_node_);
    dbr_rbnode_init(&exec->trader_node_);
}

static inline DbrBool
dbr_exec_done(const struct DbrExec* exec)
{
    return exec->c.resd == 0;
}

static inline struct DbrExec*
dbr_shared_exec_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrExec, shared_node_, node);
}

struct DbrMatch {
    /**
     * @publicsection
     */
    DbrIden id;
    DbrTicks ticks;
    DbrLots lots;
    struct DbrExec* taker_exec;
    struct DbrOrder* maker_order;
    struct DbrExec* maker_exec;
    struct DbrPosn* maker_posn;
    /**
     * @privatesection
     */
    /**
     * Pointer to next match or null.
     */
    struct DbrSlNode trans_node_;
};

static inline void
dbr_match_init(struct DbrMatch* match)
{
    dbr_slnode_init(&match->trans_node_);
}

struct DbrTrans {
    struct DbrQueue matches;
    struct DbrQueue execs;
    struct DbrPosn* taker_posn;
};

static inline void
dbr_trans_init(struct DbrTrans* trans)
{
    dbr_queue_init(&trans->matches);
    dbr_queue_init(&trans->execs);
    trans->taker_posn = NULL;
}

static inline struct DbrMatch*
dbr_trans_match_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrMatch, trans_node_, node);
}

/** @} */

/**
 * @addtogroup Data
 * @{
 */

struct DbrView {
    union DbrURec contr;
    DbrDate settl_date;
    DbrTicks bid_ticks;
    DbrLots bid_lots;
    size_t bid_count;
    DbrTicks ask_ticks;
    DbrLots ask_lots;
    size_t ask_count;
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct DbrSlNode shared_node_;
    struct DbrRbNode clnt_node_;
    struct DbrRbNode update_node_;
};

static inline void
dbr_view_init(struct DbrView* view)
{
    dbr_slnode_init(&view->shared_node_);
    dbr_rbnode_init(&view->clnt_node_);
    dbr_rbnode_init(&view->update_node_);
}

static inline struct DbrView*
dbr_shared_view_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrView, shared_node_, node);
}

/** @} */

#endif // DBR_TYPES_H
