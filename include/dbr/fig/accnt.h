/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_FIG_ACCNT_H
#define DBR_FIG_ACCNT_H

/**
 * @file
 * Accounts represent both traders and giveups.
 */

#include <dbr/elm/types.h>

/**
 * @addtogroup Accnt
 * @{
 */

/**
 * @return the associated record.
 */

DBR_API struct DbrRec*
dbr_accnt_rec(DbrAccnt accnt);

#define DBR_ACCNT_END_TRADER NULL

static inline struct DbrPerm*
dbr_accnt_trader_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrPerm, giveup_node_, node);
}

DBR_API struct DbrRbNode*
dbr_accnt_find_trader_id(DbrAccnt accnt, DbrIden id);

DBR_API struct DbrRbNode*
dbr_accnt_first_trader(DbrAccnt accnt);

DBR_API struct DbrRbNode*
dbr_accnt_last_trader(DbrAccnt accnt);

DBR_API DbrBool
dbr_accnt_empty_trader(DbrAccnt accnt);

#define DBR_ACCNT_END_GIVEUP NULL

static inline struct DbrPerm*
dbr_accnt_giveup_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrPerm, trader_node_, node);
}

DBR_API struct DbrRbNode*
dbr_accnt_find_giveup_id(DbrAccnt accnt, DbrIden id);

DBR_API struct DbrRbNode*
dbr_accnt_first_giveup(DbrAccnt accnt);

DBR_API struct DbrRbNode*
dbr_accnt_last_giveup(DbrAccnt accnt);

DBR_API DbrBool
dbr_accnt_empty_giveup(DbrAccnt accnt);

#define DBR_ACCNT_END_ORDER NULL

static inline struct DbrOrder*
dbr_accnt_order_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrOrder, accnt_node_, node);
}

DBR_API struct DbrRbNode*
dbr_accnt_find_order_id(DbrAccnt accnt, DbrIden id);

DBR_API struct DbrOrder*
dbr_accnt_find_order_ref(DbrAccnt accnt, const char* ref);

DBR_API struct DbrRbNode*
dbr_accnt_first_order(DbrAccnt accnt);

DBR_API struct DbrRbNode*
dbr_accnt_last_order(DbrAccnt accnt);

DBR_API DbrBool
dbr_accnt_empty_order(DbrAccnt accnt);

#define DBR_ACCNT_END_TRADE NULL

static inline struct DbrExec*
dbr_accnt_trade_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrExec, accnt_node_, node);
}

DBR_API struct DbrRbNode*
dbr_accnt_find_trade_id(DbrAccnt accnt, DbrIden id);

DBR_API struct DbrRbNode*
dbr_accnt_first_trade(DbrAccnt accnt);

DBR_API struct DbrRbNode*
dbr_accnt_last_trade(DbrAccnt accnt);

DBR_API DbrBool
dbr_accnt_empty_trade(DbrAccnt accnt);

#define DBR_ACCNT_END_POSN NULL

static inline struct DbrPosn*
dbr_accnt_posn_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrPosn, accnt_node_, node);
}

DBR_API struct DbrRbNode*
dbr_accnt_find_posn_id(DbrAccnt accnt, DbrIden id);

DBR_API struct DbrRbNode*
dbr_accnt_first_posn(DbrAccnt accnt);

DBR_API struct DbrRbNode*
dbr_accnt_last_posn(DbrAccnt accnt);

DBR_API DbrBool
dbr_accnt_empty_posn(DbrAccnt accnt);

DBR_API DbrBool
dbr_accnt_logged_on(DbrAccnt accnt);

DBR_API struct DbrSess*
dbr_accnt_sess(DbrAccnt accnt);

/** @} */

#endif // DBR_FIG_ACCNT_H
