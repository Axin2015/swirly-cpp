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
#ifndef DBR_TRADER_H
#define DBR_TRADER_H

#include <dbr/types.h>

/**
 * @addtogroup Trader
 * @{
 */

// Returns the associated record.

DBR_API struct DbrRec*
dbr_trader_rec(DbrTrader trader);

/** @} */

/**
 * @addtogroup TraderOrder
 * @{
 */

#define DBR_TRADER_END_ORDER NULL

static inline struct DbrOrder*
dbr_trader_order_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrOrder, trader_node_, node);
}

DBR_API struct DbrRbNode*
dbr_trader_find_order_id(DbrTrader trader, DbrIden id);

DBR_API struct DbrOrder*
dbr_trader_find_order_ref(DbrTrader trader, const char* ref);

DBR_API struct DbrRbNode*
dbr_trader_first_order(DbrTrader trader);

DBR_API struct DbrRbNode*
dbr_trader_last_order(DbrTrader trader);

DBR_API DbrBool
dbr_trader_empty_order(DbrTrader trader);

/** @} */

/**
 * @addtogroup TraderTrade
 * @{
 */

#define DBR_TRADER_END_TRADE NULL

static inline struct DbrExec*
dbr_trader_trade_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrExec, trader_node_, node);
}

DBR_API struct DbrRbNode*
dbr_trader_find_trade_id(DbrTrader trader, DbrIden id);

DBR_API struct DbrRbNode*
dbr_trader_first_trade(DbrTrader trader);

DBR_API struct DbrRbNode*
dbr_trader_last_trade(DbrTrader trader);

DBR_API DbrBool
dbr_trader_empty_trade(DbrTrader trader);

/** @} */

/**
 * @addtogroup TraderMemb
 * @{
 */

#define DBR_TRADER_END_MEMB NULL

static inline struct DbrMemb*
dbr_trader_memb_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrMemb, trader_node_, node);
}

DBR_API struct DbrRbNode*
dbr_trader_find_memb_id(DbrTrader trader, DbrIden id);

DBR_API struct DbrRbNode*
dbr_trader_first_memb(DbrTrader trader);

DBR_API struct DbrRbNode*
dbr_trader_last_memb(DbrTrader trader);

DBR_API DbrBool
dbr_trader_empty_memb(DbrTrader trader);

/** @} */

/**
 * @addtogroup Trader
 * @{
 */

DBR_API DbrBool
dbr_trader_logged_on(DbrTrader trader);

/** @} */

#endif // DBR_TRADER_H
