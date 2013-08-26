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
#ifndef DBR_TRADER_H
#define DBR_TRADER_H

#include <dbr/defs.h>
#include <dbr/types.h>

/**
 * @addtogroup Trader
 * @{
 */

// Returns the trader-id for state.

DBR_API DbrIden
dbr_trader_id(DbrTrader trader);

/** @} */

/**
 * @addtogroup TraderOrder
 * @{
 */

DBR_API struct DbrRbNode*
dbr_trader_find_order_id(DbrTrader trader, DbrIden id);

DBR_API struct DbrOrder*
dbr_trader_find_order_ref(DbrTrader trader, const char* ref);

DBR_API struct DbrRbNode*
dbr_trader_first_order(DbrTrader trader);

DBR_API struct DbrRbNode*
dbr_trader_last_order(DbrTrader trader);

DBR_API struct DbrRbNode*
dbr_trader_end_order(DbrTrader trader);

DBR_API DbrBool
dbr_trader_empty_order(DbrTrader trader);

/** @} */

/**
 * @addtogroup TraderSub
 * @{
 */

/**
 * @brief Subscribe to book.
 */

DBR_API DbrBool
dbr_trader_sub(DbrTrader trader, DbrBook book);

/**
 * @brief Unsubscribe from book.
 */

DBR_API void
dbr_trader_unsub(DbrTrader trader, DbrBook book);

/** @} */

/**
 * @addtogroup TraderSess
 * @{
 */

DBR_API void
dbr_trader_set_sess(DbrTrader trader, DbrTraderSess sess);

DBR_API DbrTraderSess
dbr_trader_sess(DbrTrader trader);

/** @} */

#endif // DBR_TRADER_H
