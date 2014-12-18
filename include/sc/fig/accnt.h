/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_FIG_ACCNT_H
#define SC_FIG_ACCNT_H

/**
 * @file
 * Accounts represent both traders and giveups.
 */

#include <sc/elm/types.h>

/**
 * @addtogroup Accnt
 * @{
 */

/**
 * @return the associated record.
 */

SC_API struct ScRec*
sc_accnt_rec(ScAccnt accnt);

#define SC_ACCNT_END_TRADER NULL

static inline struct ScPerm*
sc_accnt_trader_entry(struct ScRbNode* node)
{
    return sc_implof(struct ScPerm, giveup_node_, node);
}

SC_API struct ScRbNode*
sc_accnt_find_trader_id(ScAccnt accnt, ScIden id);

SC_API struct ScRbNode*
sc_accnt_first_trader(ScAccnt accnt);

SC_API struct ScRbNode*
sc_accnt_last_trader(ScAccnt accnt);

SC_API ScBool
sc_accnt_empty_trader(ScAccnt accnt);

#define SC_ACCNT_END_GIVEUP NULL

static inline struct ScPerm*
sc_accnt_giveup_entry(struct ScRbNode* node)
{
    return sc_implof(struct ScPerm, trader_node_, node);
}

SC_API struct ScRbNode*
sc_accnt_find_giveup_id(ScAccnt accnt, ScIden id);

SC_API struct ScRbNode*
sc_accnt_first_giveup(ScAccnt accnt);

SC_API struct ScRbNode*
sc_accnt_last_giveup(ScAccnt accnt);

SC_API ScBool
sc_accnt_empty_giveup(ScAccnt accnt);

#define SC_ACCNT_END_ORDER NULL

static inline struct ScOrder*
sc_accnt_order_entry(struct ScRbNode* node)
{
    return sc_implof(struct ScOrder, accnt_node_, node);
}

SC_API struct ScRbNode*
sc_accnt_find_order_id(ScAccnt accnt, ScIden id);

SC_API struct ScOrder*
sc_accnt_find_order_ref(ScAccnt accnt, const char* ref);

SC_API struct ScRbNode*
sc_accnt_first_order(ScAccnt accnt);

SC_API struct ScRbNode*
sc_accnt_last_order(ScAccnt accnt);

SC_API ScBool
sc_accnt_empty_order(ScAccnt accnt);

#define SC_ACCNT_END_TRADE NULL

static inline struct ScExec*
sc_accnt_trade_entry(struct ScRbNode* node)
{
    return sc_implof(struct ScExec, accnt_node_, node);
}

SC_API struct ScRbNode*
sc_accnt_find_trade_id(ScAccnt accnt, ScIden id);

SC_API struct ScRbNode*
sc_accnt_first_trade(ScAccnt accnt);

SC_API struct ScRbNode*
sc_accnt_last_trade(ScAccnt accnt);

SC_API ScBool
sc_accnt_empty_trade(ScAccnt accnt);

#define SC_ACCNT_END_POSN NULL

static inline struct ScPosn*
sc_accnt_posn_entry(struct ScRbNode* node)
{
    return sc_implof(struct ScPosn, accnt_node_, node);
}

SC_API struct ScRbNode*
sc_accnt_find_posn_id(ScAccnt accnt, ScIden id);

SC_API struct ScRbNode*
sc_accnt_first_posn(ScAccnt accnt);

SC_API struct ScRbNode*
sc_accnt_last_posn(ScAccnt accnt);

SC_API ScBool
sc_accnt_empty_posn(ScAccnt accnt);

SC_API ScBool
sc_accnt_logged_on(ScAccnt accnt);

SC_API struct ScSess*
sc_accnt_sess(ScAccnt accnt);

/** @} */

#endif // SC_FIG_ACCNT_H
