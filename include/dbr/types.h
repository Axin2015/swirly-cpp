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
#ifndef DBR_TYPES_H
#define DBR_TYPES_H

#include <dbr/bool.h>
#include <dbr/node.h>

#include <stddef.h>

#include <sys/types.h> // ssize_t

/**
 * @addtogroup Types
 * @{
 */

typedef long DbrIden;

/**
 * @brief ISO8601 format: "%Y%m%d".
 */

typedef int DbrDate;
typedef long DbrIncs;
typedef long DbrIncs;
typedef unsigned long DbrFlags;

/**
 * @brief Milliseconds since Unix epoch.
 */

typedef long DbrMillis;

typedef DbrIncs DbrLots;
typedef DbrIncs DbrTicks;
typedef DbrIncs DbrLicks;

enum {
    DBR_DISPLAY_MAX = 64,
    DBR_EMAIL_MAX = 64,
    DBR_MNEM_MAX = 16,
    DBR_REF_MAX = 64,
    DBR_TENOR_MAX = 8
};

typedef char DbrDisplay[DBR_DISPLAY_MAX];

/**
 * @brief Email address.
 */

typedef char DbrEmail[DBR_EMAIL_MAX];

/**
 * @brief Memorable identifier.
 */

typedef char DbrMnem[DBR_MNEM_MAX];

/**
 * @brief Reference.
 */

typedef char DbrRef[DBR_REF_MAX];
typedef char DbrTenor[DBR_TENOR_MAX];

enum DbrAction {
    DBR_BUY = 1,
    DBR_SELL = -1
};

enum DbrMethod {
    // See dbr_rest.h for details.
    DBR_DELETE = 1,
    DBR_GET    = 2,
    DBR_HEAD   = 3,
    DBR_POST   = 4,
    DBR_PUT    = 5,
};

#define DBR_METHOD_MASK 0x000000FF

enum DbrEntity {
    // See dbr_rest.h for details.
    DBR_INSTR  = 1 << 8,
    DBR_MARKET = 2 << 8,
    DBR_TRADER = 3 << 8,
    DBR_ACCNT  = 4 << 8,
    DBR_DEPTH  = 5 << 8,
    DBR_ORDER  = 6 << 8,
    DBR_MEMB   = 7 << 8,
    DBR_TRADE  = 8 << 8,
    DBR_POSN   = 9 << 8
};

#define DBR_ENTITY_MASK 0x0000FF00

/** @} */

/**
 * @addtogroup Instr
 */

typedef struct ElmInstr* DbrInstr;

/**
 * @addtogroup Market
 */

typedef struct ElmMarket* DbrMarket;

struct DbrBest {
    DbrTicks bid_ticks;
    DbrLots bid_resd;
    DbrTicks ask_ticks;
    DbrLots ask_resd;
};

/**
 * @addtogroup Side
 */

typedef struct ElmSide* DbrSide;

/**
 * @addtogroup Accnt
 */

typedef struct ElmAccnt* DbrAccnt;

/**
 * @addtogroup Trader
 */

typedef struct ElmTrader* DbrTrader;

/**
 * @addtogroup Rec
 * @{
 */

/**
 * @brief Union used for record enrichment.
 */

union DbrURec {
    DbrIden id;
    struct DbrRec* rec;
};

struct DbrRec {
    /**
     * @publicsection
     */
    int type;
    DbrIden id;
    DbrMnem mnem;
    union {
        struct {
            DbrDisplay display;
            DbrMnem asset_type;
            DbrMnem instr_type;
            DbrMnem asset;
            DbrMnem ccy;
            int tick_numer;
            int tick_denom;
            double price_inc;
            int lot_numer;
            int lot_denom;
            double qty_inc;
            int price_dp;
            int pip_dp;
            int qty_dp;
            DbrLots min_lots;
            DbrLots max_lots;
            DbrInstr state;
        } instr;
        struct {
            union DbrURec instr;
            DbrTenor tenor;
            DbrDate settl_date;
            DbrMarket state;
        } market;
        struct {
            DbrDisplay display;
            DbrEmail email;
            DbrAccnt state;
        } accnt;
        struct {
            DbrDisplay display;
            DbrEmail email;
            DbrTrader state;
        } trader;
    };
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct DbrSlNode model_node_;
    // Cache nodes.
    struct DbrSlNode id_node_;
    struct DbrSlNode mnem_node_;
};

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
 * @addtogroup SideLevel
 * @{
 */

struct DbrLevel {
    /**
     * @publicsection
     */
    struct DbrOrder* first_order;
    // Must be greater than zero.
    size_t count;
    // Signed where negative means buy.
    DbrTicks ticks;
    // Must be greater than zero.
    DbrLots resd;
    /**
     * @privatesection
     */
    struct DbrRbNode side_node_;
};

/** @} */

/**
 * @addtogroup Exec
 * @{
 */

enum DbrStatus {
    DBR_NEW = 1,
    DBR_REVISED,
    DBR_CANCELLED,
    DBR_REJECTED,
    DBR_PARTIAL,
    DBR_FILLED
};

struct DbrOrder {
    /**
     * @publicsection
     */
    // Immutable. Zero is reserved for sentinel nodes only.
    DbrIden id;
    // Set when order is associated with market.
    struct DbrLevel* level;
    // Order revision counter.
    int rev;
    /**
     * @sa enum DbrStatus
     */
    int status;
    // Immutable except for resd.
    union DbrURec trader;
    union DbrURec accnt;
    // Ref is optional.
    DbrRef ref;
    union DbrURec market;
    /**
     * @sa enum DbrAction
     */
    int action;
    DbrTicks ticks;
    // Must be greater than zero.
    DbrLots resd;
    // Must not be greater that lots.
    DbrLots exec;
    // Must be greater than zero.
    DbrLots lots;
    // Minimum to be filled by this order.
    DbrLots min;
    // Flags.
    DbrFlags flags;
    DbrMillis created;
    DbrMillis modified;
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct DbrSlNode model_node_;
    // Singly-linked for index.
    struct DbrSlNode ref_node_;
    // Doubly-linked for side.
    struct DbrDlNode side_node_;
    struct DbrRbNode trader_node_;
};

enum DbrRole {
    DBR_MAKER = 1,
    DBR_TAKER
};

struct DbrTrade {
    /**
     * @publicsection
     */
    DbrIden id;
    DbrIden match;
    DbrIden order;
    int order_rev;
    union DbrURec trader;
    union DbrURec accnt;
    // Ref is optional.
    DbrRef ref;
    union DbrURec market;
    union DbrURec cpty;
    /**
     * @sa enum DbrRole
     */
    int role;
    /**
     * @sa enum DbrAction
     */
    int action;
    DbrTicks ticks;
    DbrLots resd;
    DbrLots exec;
    DbrLots lots;
    DbrDate settl_date;
    DbrMillis created;
    DbrMillis modified;
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct DbrSlNode model_node_;
    struct DbrRbNode accnt_node_;
};

/** @} */

/**
 * @addtogroup Trans
 * @{
 */

struct DbrMatch {
    /**
     * @publicsection
     */
    DbrIden id;
    struct DbrOrder* maker_order;
    struct DbrPosn* maker_posn;
    // Must be greater than zero.
    DbrTicks ticks;
    // Must be greater than zero.
    DbrLots lots;
    struct DbrTrade* taker_trade;
    struct DbrTrade* maker_trade;
    /**
     * @privatesection
     */
    /**
     * Pointer to next match or null.
     */
    struct DbrSlNode trans_node_;
};

struct DbrTrans {
    struct DbrOrder* new_order;
    // Null if no matches.
    struct DbrPosn* new_posn;
    // Null if no matches.
    struct DbrSlNode* first_match;
    /**
     * Number of matches.
     */
    size_t count;
    /**
     * Total quantity taken.
     */
    DbrLots taken;
};

/** @} */

/**
 * @addtogroup AccntMemb
 * @{
 */

struct DbrMemb {
    /**
     * @publicsection
     */
    union DbrURec accnt;
    union DbrURec trader;
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct DbrSlNode model_node_;
    struct DbrRbNode accnt_node_;
};

/** @} */

/**
 * @addtogroup AccntPosn
 * @{
 */

struct DbrPosn {
    /**
     * @publicsection
     */
    DbrIden id;
    union DbrURec accnt;
    union DbrURec instr;
    DbrDate settl_date;
    DbrLicks buy_licks;
    DbrLots buy_lots;
    DbrLicks sell_licks;
    DbrLots sell_lots;
    /**
     * @privatesection
     */
    // Singly-linked for data model.
    struct DbrSlNode model_node_;
    struct DbrRbNode accnt_node_;
};

/** @} */

/**
 * @addtogroup AccntSess
 * @{
 */

typedef struct DbrIAccntSess {
    const struct DbrAccntSessVtbl* vtbl;
}* DbrAccntSess;

/** @} */

/**
 * @addtogroup TraderSub
 * @{
 */

struct DbrSub {
    /**
     * @publicsection
     */
    DbrMarket market;
    DbrTrader trader;
    /**
     * @privatesection
     */
    struct DbrDlNode market_node_;
    struct DbrRbNode trader_node_;
};

/** @} */

/**
 * @addtogroup TraderSess
 * @{
 */

typedef struct DbrITraderSess {
    const struct DbrTraderSessVtbl* vtbl;
}* DbrTraderSess;

/** @} */

#endif // DBR_TYPES_H
