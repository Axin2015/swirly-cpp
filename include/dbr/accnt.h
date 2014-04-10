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
#ifndef DBR_ACCNT_H
#define DBR_ACCNT_H

#include <dbr/types.h>

/**
 * @addtogroup Accnt
 * @{
 */

// Returns the associated record.

DBR_API struct DbrRec*
dbr_accnt_rec(DbrAccnt accnt);

/** @} */

/**
 * @addtogroup AccntGroup
 * @{
 */

#define DBR_ACCNT_END_GROUP NULL

static inline struct DbrMemb*
dbr_accnt_group_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrMemb, user_node_, node);
}

DBR_API struct DbrRbNode*
dbr_accnt_find_group_id(DbrAccnt accnt, DbrIden id);

DBR_API struct DbrRbNode*
dbr_accnt_first_group(DbrAccnt accnt);

DBR_API struct DbrRbNode*
dbr_accnt_last_group(DbrAccnt accnt);

DBR_API DbrBool
dbr_accnt_empty_group(DbrAccnt accnt);

/** @} */

/**
 * @addtogroup AccntMemb
 * @{
 */

#define DBR_ACCNT_END_USER NULL

static inline struct DbrMemb*
dbr_accnt_user_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrMemb, group_node_, node);
}

DBR_API struct DbrRbNode*
dbr_accnt_find_user_id(DbrAccnt accnt, DbrIden id);

DBR_API struct DbrRbNode*
dbr_accnt_first_user(DbrAccnt accnt);

DBR_API struct DbrRbNode*
dbr_accnt_last_user(DbrAccnt accnt);

DBR_API DbrBool
dbr_accnt_empty_user(DbrAccnt accnt);

/** @} */

/**
 * @addtogroup AccntOrder
 * @{
 */

#define DBR_ACCNT_END_ORDER NULL

static inline struct DbrOrder*
dbr_accnt_order_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrOrder, trader_node_, node);
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

/** @} */

/**
 * @addtogroup AccntTrade
 * @{
 */

#define DBR_ACCNT_END_TRADE NULL

static inline struct DbrExec*
dbr_accnt_trade_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrExec, trader_node_, node);
}

DBR_API struct DbrRbNode*
dbr_accnt_find_trade_id(DbrAccnt accnt, DbrIden id);

DBR_API struct DbrRbNode*
dbr_accnt_first_trade(DbrAccnt accnt);

DBR_API struct DbrRbNode*
dbr_accnt_last_trade(DbrAccnt accnt);

DBR_API DbrBool
dbr_accnt_empty_trade(DbrAccnt accnt);

/** @} */

/**
 * @addtogroup AccntPosn
 * @{
 */

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

/** @} */

/**
 * @addtogroup Accnt
 * @{
 */

DBR_API DbrBool
dbr_accnt_logged_on(DbrAccnt accnt);

DBR_API struct DbrSess*
dbr_accnt_sess(DbrAccnt accnt);

/** @} */

#endif // DBR_ACCNT_H
