/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_TYPES_H
#define SC_ASH_TYPES_H

#include <sc/ash/date.h>
#include <sc/ash/list.h>
#include <sc/ash/queue.h>
#include <sc/ash/slnode.h>
#include <sc/ash/tree.h>

#include <sys/types.h> // ssize_t

/**
 * @addtogroup Simple
 * @{
 */

typedef long ScIncs;

/**
 * Milliseconds since Unix epoch.
 */

typedef long ScMillis;

typedef ScIncs ScLots;

/**
 * Unit representing the minimum price increment.
 */

typedef ScIncs ScTicks;

/**
 * Sum of lots and ticks.
 */

typedef ScIncs ScLicks;

enum ScEntity {
    /**
     * Account.
     */
    SC_ENTITY_ACCNT = 1,
    /**
     * Contract.
     */
    SC_ENTITY_CONTR,
    /**
     * Permission.
     */
    SC_ENTITY_PERM,
    /**
     * Order.
     */
    SC_ENTITY_ORDER,
    /**
     * Execution.
     */
    SC_ENTITY_EXEC,
    /**
     * Position.
     */
    SC_ENTITY_POSN
};

enum ScState {
    SC_STATE_NEW = 1,
    SC_STATE_REVISE,
    SC_STATE_CANCEL,
    SC_STATE_TRADE
};

enum ScAction {
    SC_ACTION_BUY = 1,
    SC_ACTION_SELL = -1
};

enum {
    /**
     * Maximum display characters.
     */
    SC_DISPLAY_MAX = 64,
    /**
     * Maximum email characters.
     */
    SC_EMAIL_MAX = 64,
    /**
     * Maximum mnemonic characters.
     */
    SC_MNEM_MAX = 16,
    /**
     * Maximum reference characters.
     */
    SC_REF_MAX = 64
};

/**
 * Description suitable for display on user-interface.
 */

typedef char ScDisplay[SC_DISPLAY_MAX];

/**
 * Email address.
 */

typedef char ScEmail[SC_EMAIL_MAX];

/**
 * Memorable identifier.
 */

typedef char ScMnem[SC_MNEM_MAX];

/**
 * Reference.
 */

typedef char ScRef[SC_REF_MAX];

/** @} */

/**
 * @addtogroup Bank
 */

enum ScReg {
    /**
     * Order identifier register index.
     */
    SC_REG_ORDER = 0,
    /**
     * Execution identifier register index.
     */
    SC_REG_EXEC,
    /**
     * Match identifier register index.
     */
    SC_REG_MATCH,
    /**
     * Number of registers in @ref ScBank file.
     */
    SC_REG_LEN
};

/** @} */

/**
 * @addtogroup Pool
 */

typedef struct ElmPool* ScPool;

/**
 * @addtogroup Accnt
 */

typedef struct FigAccnt* ScAccnt;

/**
 * @addtogroup Rec
 * @{
 */

/**
 * Union used for record enrichment.
 */

union ScURec {
    ScIden id_only;
    struct ScRec* rec;
};

struct ScRec {
    /**
     * @publicsection
     */
    int type;
    ScIden id;
    ScMnem mnem;
    ScDisplay display;
    union {
        struct {
            ScEmail email;
            ScAccnt state;
        } accnt;
        struct {
            ScMnem asset_type;
            ScMnem asset;
            ScMnem ccy;
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
            ScLots min_lots;
            ScLots max_lots;
        } contr;
    };
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct ScSlNode shared_node_;
    // Cache nodes.
    struct ScSlNode id_node_;
    struct ScSlNode mnem_node_;
};

static inline void
sc_rec_init(struct ScRec* rec)
{
    sc_slnode_init(&rec->shared_node_);
    sc_slnode_init(&rec->id_node_);
    sc_slnode_init(&rec->mnem_node_);
}

static inline struct ScRec*
sc_shared_rec_entry(struct ScSlNode* node)
{
    return sc_implof(struct ScRec, shared_node_, node);
}

/** @} */

/**
 * @addtogroup Perm
 * @{
 */

struct ScPerm {
    /**
     * @publicsection
     */
    union ScURec trader;
    union ScURec giveup;
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct ScSlNode shared_node_;
    struct ScRbNode trader_node_;
    struct ScRbNode giveup_node_;
};

static inline void
sc_perm_init(struct ScPerm* perm)
{
    sc_slnode_init(&perm->shared_node_);
    sc_rbnode_init(&perm->trader_node_);
    sc_rbnode_init(&perm->giveup_node_);
}

static inline struct ScPerm*
sc_shared_perm_entry(struct ScSlNode* node)
{
    return sc_implof(struct ScPerm, shared_node_, node);
}

/** @} */

/**
 * @addtogroup Instruct
 * @{
 */

/**
 * Fields common to both ScOrder and ScExec.
 */

struct ScCommon {
    /**
     * The executing trader.
     */
    union ScURec trader;
    /**
     * The give-up counter-party.
     */
    union ScURec giveup;
    union ScURec contr;
    ScJd settl_day;
    /**
     * Ref is optional.
     */
    ScRef ref;
    /**
     * @sa enum ScState
     */
    int state;
    /**
     * @sa enum ScAction
     */
    int action;
    ScTicks ticks;
    /**
     * Must be greater than zero.
     */
    ScLots lots;
    /**
     * Must be greater than zero.
     */
    ScLots resd;
    /**
     * Must not be greater that lots.
     */
    ScLots exec;
    ScTicks last_ticks;
    ScLots last_lots;
    /**
     * Minimum to be filled by this order.
     */
    ScLots min_lots;
};

/** @} */

/**
 * @addtogroup Order
 * @{
 */

struct ScOrder {
    /**
     * @publicsection
     */
    /**
     * Set when order is associated with book.
     */
    struct ScLevel* level;
    ScIden id;
    struct ScCommon i;
    ScMillis created;
    ScMillis modified;
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct ScSlNode shared_node_;
    // Singly-linked for index.
    struct ScSlNode ref_node_;
    // Doubly-linked for side.
    struct ScDlNode side_node_;
    struct ScRbNode accnt_node_;
};

static inline void
sc_order_init(struct ScOrder* order)
{
    sc_slnode_init(&order->shared_node_);
    sc_slnode_init(&order->ref_node_);
    sc_dlnode_init(&order->side_node_);
    sc_rbnode_init(&order->accnt_node_);
}

static inline ScBool
sc_order_done(const struct ScOrder* order)
{
    return order->i.resd == 0;
}

static inline struct ScOrder*
sc_shared_order_entry(struct ScSlNode* node)
{
    return sc_implof(struct ScOrder, shared_node_, node);
}

/** @} */

/**
 * A level is an aggregation of orders by price.
 * @addtogroup Level
 * @{
 */

struct ScLevel {
    /**
     * @publicsection
     */
    struct ScOrder* first_order;
    ScTicks ticks;
    /**
     * Must be greater than zero.
     */
    ScLots lots;
    /**
     * Must be greater than zero.
     */
    size_t count;
    /**
     * @privatesection
     */
    struct ScRbNode side_node_;
};

static inline void
sc_level_init(struct ScLevel* level)
{
    sc_rbnode_init(&level->side_node_);
}

/** @} */

/**
 * @addtogroup Exec
 * @{
 */

enum ScRole {
    SC_ROLE_MAKER = 1,
    SC_ROLE_TAKER
};

struct ScExec {
    /**
     * @publicsection
     */
    ScIden id;
    ScIden order;
    struct ScCommon i;
    /**
     * @sa struct ScMatch
     */
    ScIden match;
    /**
     * @privatesection
     */
    /**
     * Reference count. Exec lifetime is managed by a shared reference count.
     */
    int refs_;
    /**
     * @publicsection
     */
    /**
     * @sa enum ScRole
     */
    int role;
    union ScURec cpty;
    ScMillis created;
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct ScSlNode shared_node_;
    struct ScRbNode accnt_node_;
};

static inline void
sc_exec_init(struct ScExec* exec)
{
    exec->refs_ = 1;
    sc_slnode_init(&exec->shared_node_);
    sc_rbnode_init(&exec->accnt_node_);
}

static inline ScBool
sc_exec_done(const struct ScExec* exec)
{
    return exec->i.resd == 0;
}

static inline struct ScExec*
sc_shared_exec_entry(struct ScSlNode* node)
{
    return sc_implof(struct ScExec, shared_node_, node);
}

/** @} */

/**
 * @addtogroup Match
 * @{
 */

/**
 * A Match represents two orders from opposing sides of the market that may trade.
 */

struct ScMatch {
    /**
     * @publicsection
     */
    ScIden id;
    ScTicks ticks;
    ScLots lots;
    struct ScExec* taker_exec;
    struct ScOrder* maker_order;
    struct ScExec* maker_exec;
    struct ScPosn* maker_posn;
    /**
     * @privatesection
     */
    /**
     * Pointer to next match or null.
     */
    struct ScSlNode trans_node_;
};

static inline void
sc_match_init(struct ScMatch* match)
{
    sc_slnode_init(&match->trans_node_);
}

/**
 * A single Transaction comprises many Matches.
 */
struct ScTrans {
    struct ScQueue matches;
    /**
     * All executions referenced in ScTrans::matches.
     */
    struct ScQueue execs;
    struct ScPosn* taker_posn;
};

static inline void
sc_trans_init(struct ScTrans* trans)
{
    sc_queue_init(&trans->matches);
    sc_queue_init(&trans->execs);
    trans->taker_posn = NULL;
}

static inline struct ScMatch*
sc_trans_match_entry(struct ScSlNode* node)
{
    return sc_implof(struct ScMatch, trans_node_, node);
}

/** @} */

/**
 * @addtogroup Posn
 * @{
 */

struct ScPosn {
    /**
     * @publicsection
     */
    union ScURec accnt;
    union ScURec contr;
    ScJd settl_day;
    ScLicks buy_licks;
    ScLots buy_lots;
    ScLicks sell_licks;
    ScLots sell_lots;
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct ScSlNode shared_node_;
    struct ScRbNode accnt_node_;
    struct ScRbNode update_node_;
};

static inline void
sc_posn_init(struct ScPosn* posn)
{
    sc_slnode_init(&posn->shared_node_);
    sc_rbnode_init(&posn->accnt_node_);
    sc_rbnode_init(&posn->update_node_);
}

/**
 * Synthetic posn key.
 */

static inline ScKey
sc_posn_key(ScIden aid, ScIden cid, ScJd settl_day)
{
    enum {
        // 16 million ids.
        ID_MASK = (1 << 24) - 1,
        // 16 bits is sufficient for truncated Julian day.
        JD_MASK = (1 << 16) - 1
    };
    // Truncated Julian Day (TJD).
    const ScJd tjd = sc_jd_to_tjd(settl_day);
    return ((((ScKey)aid) & ID_MASK) << 40) | ((((ScKey)cid) & ID_MASK) << 16)
        | (((ScKey)tjd) & JD_MASK);
}

static inline struct ScPosn*
sc_shared_posn_entry(struct ScSlNode* node)
{
    return sc_implof(struct ScPosn, shared_node_, node);
}

/** @} */

/**
 * @addtogroup View
 * @{
 */

enum {
    /**
     * Maximum price levels in view.
     */
    SC_LEVEL_MAX = 3
};

struct ScView {
    union ScURec contr;
    ScJd settl_day;
    ScTicks bid_ticks[SC_LEVEL_MAX];
    ScLots bid_lots[SC_LEVEL_MAX];
    size_t bid_count[SC_LEVEL_MAX];
    ScTicks offer_ticks[SC_LEVEL_MAX];
    ScLots offer_lots[SC_LEVEL_MAX];
    size_t offer_count[SC_LEVEL_MAX];
    ScMillis created;
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct ScSlNode shared_node_;
    struct ScRbNode clnt_node_;
    struct ScRbNode update_node_;
};

static inline void
sc_view_init(struct ScView* view)
{
    sc_slnode_init(&view->shared_node_);
    sc_rbnode_init(&view->clnt_node_);
    sc_rbnode_init(&view->update_node_);
}

/**
 * Synthetic view key.
 */

static inline ScKey
sc_view_key(ScIden cid, ScJd settl_day)
{
    enum {
        // 16 million ids.
        ID_MASK = (1 << 24) - 1,
        // 16 bits is sufficient for truncated Julian day.
        JD_MASK = (1 << 16) - 1
    };
    // Truncated Julian Day (TJD).
    const ScJd tjd = sc_jd_to_tjd(settl_day);
    return ((((ScKey)cid) & ID_MASK) << 16) | (((ScKey)tjd) & JD_MASK);
}

static inline struct ScView*
sc_shared_view_entry(struct ScSlNode* node)
{
    return sc_implof(struct ScView, shared_node_, node);
}

/** @} */

/**
 * An Order Book comprises two sides: one for Bids and one for Offers.
 * @addtogroup Side
 * @{
 */

struct ScSide {
    ScPool pool;
    struct ScTree levels;
    /**
     * The orders for each side are ordered by price and time.
     */
    struct ScList orders;
    /**
     * Last trade information.
     */
    ScTicks last_ticks;
    ScLots last_lots;
    ScMillis last_time;
};

/** @} */

/**
 * @addtogroup Book
 * @{
 */

struct ScBook {
    struct ScRec* crec;
    ScJd settl_day;
    struct ScSide bid_side;
    struct ScSide offer_side;
    /**
     * @privatesection
     */
    struct ScRbNode serv_node_;
    struct ScRbNode update_node_;
};

/**
 * Synthetic book key.
 */

static inline ScKey
sc_book_key(ScIden cid, ScJd settl_day)
{
    enum {
        // 16 million ids.
        ID_MASK = (1 << 24) - 1,
        // 16 bits is sufficient for truncated Julian day.
        JD_MASK = (1 << 16) - 1
    };
    // Truncated Julian Day (TJD).
    const ScJd tjd = sc_jd_to_tjd(settl_day);
    return ((((ScKey)cid) & ID_MASK) << 16) | (((ScKey)tjd) & JD_MASK);
}

/** @} */

/**
 * @addtogroup Sess
 * @{
 */

struct ScSub {
    /**
     * @privatesection
     */
    int refs_;
    struct ScRbNode sess_node_;
};

static inline void
sc_sub_init(struct ScSub* sub)
{
    sub->refs_ = 1;
    sc_rbnode_init(&sub->sess_node_);
}

struct ScSess {
    ScUuid uuid;
    ScPool pool;
    int sid;
    /**
     * The heartbeat interval requested by remote side.
     */
    int hbint;
    struct ScTree subs;
    struct ScTree accnts;
    /**
     * @privatesection
     */
    long marker_;
    // FigSessIdx bucket list.
    struct ScSlNode uuid_node_;
};

static inline struct ScSub*
sc_sess_sub_entry(struct ScRbNode* node)
{
    return sc_implof(struct ScSub, sess_node_, node);
}

/** @} */

#endif // SC_ASH_TYPES_H
