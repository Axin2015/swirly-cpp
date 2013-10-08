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
#ifndef DBR_EXCH_H
#define DBR_EXCH_H

#include <dbr/model.h>
#include <dbr/pool.h>

/**
 * @addtogroup Exch
 * @{
 */

typedef struct DbrExch_* DbrExch;

/**
 * @brief Create Exchange.
 *
 * @param journ Journal.
 *
 * @param model Model.
 *
 * @param pool Pool.
 *
 * @return Handle to newly created Exchange or null on failure.
 */

DBR_API DbrExch
dbr_exch_create(DbrJourn journ, DbrModel model, DbrPool pool);

// No-op if exch is null.

DBR_API void
dbr_exch_destroy(DbrExch exch);

/** @} */

/**
 * @addtogroup Data
 * @{
 */

/**
 * @brief Returns first record of requested type.
 */

// Size is optional.

DBR_API struct DbrSlNode*
dbr_exch_first_rec(DbrExch exch, int type, size_t* size);

// Null if record does not exist.

DBR_API struct DbrSlNode*
dbr_exch_find_rec_id(DbrExch exch, int type, DbrIden id);

// Null if record does not exist.
// This function compares DBR_MNEM_MAX characters of mnem at most.

DBR_API struct DbrSlNode*
dbr_exch_find_rec_mnem(DbrExch exch, int type, const char* mnem);

DBR_API struct DbrSlNode*
dbr_exch_end_rec(DbrExch exch);

/** @} */

/**
 * @addtogroup Trader
 * @{
 */

DBR_API DbrTrader
dbr_exch_trader(DbrExch exch, struct DbrRec* trec);

/** @} */

/**
 * @addtogroup Accnt
 * @{
 */

DBR_API DbrAccnt
dbr_exch_accnt(DbrExch exch, struct DbrRec* arec);

/** @} */

/**
 * @addtogroup Book
 * @{
 */

DBR_API struct DbrBook*
dbr_exch_book(DbrExch exch, struct DbrRec* crec, DbrDate settl_date);

/** @} */

/**
 * @addtogroup Exec
 * @{
 */

/**
 * @brief Place order.
 * All members of result are set to zero on failure.
 */

DBR_API struct DbrOrder*
dbr_exch_place(DbrExch exch, DbrTrader trader, DbrAccnt accnt, struct DbrBook* book,
               const char* ref, int action, DbrTicks ticks, DbrLots lots, DbrLots min,
               DbrFlags flags, struct DbrResult* result);

// Assumes that order already belongs to this side.
// Reduced lots must not be:
// 1. less than executed lots;
// 2. less than min lots;
// 3. greater than original lots.

DBR_API struct DbrOrder*
dbr_exch_revise_id(DbrExch exch, DbrTrader trader, DbrIden id, DbrLots lots);

DBR_API struct DbrOrder*
dbr_exch_revise_ref(DbrExch exch, DbrTrader trader, const char* ref, DbrLots lots);

DBR_API struct DbrOrder*
dbr_exch_cancel_id(DbrExch exch, DbrTrader trader, DbrIden id);

DBR_API struct DbrOrder*
dbr_exch_cancel_ref(DbrExch exch, DbrTrader trader, const char* ref);

// Invalidates any pointers to the trade.

DBR_API DbrBool
dbr_exch_archive_order(DbrExch exch, DbrTrader trader, DbrIden id);

// Invalidates any pointers to the trade.

DBR_API DbrBool
dbr_exch_archive_trade(DbrExch exch, DbrTrader trader, DbrIden id);

/** @} */

#endif // DBR_EXCH_H
