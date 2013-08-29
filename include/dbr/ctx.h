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
#ifndef DBR_CTX_H
#define DBR_CTX_H

#include <dbr/model.h>
#include <dbr/pool.h>

/**
 * @addtogroup Ctx
 * @{
 */

typedef struct DbrCtx_* DbrCtx;

/**
 * @brief Create context.
 *
 * @param pool Pool.
 *
 * @param journ Journal.
 *
 * @param model Model.
 *
 * @return Handle to newly created context or null on failure.
 */

DBR_API DbrCtx
dbr_ctx_create(DbrPool pool, DbrJourn journ, DbrModel model);

// No-op if ctx is null.

DBR_API void
dbr_ctx_destroy(DbrCtx ctx);

/** @} */

/**
 * @addtogroup Rec
 * @{
 */

/**
 * @brief Returns first record of requested type.
 */

// Size is optional.

DBR_API struct DbrSlNode*
dbr_ctx_first_rec(DbrCtx ctx, int type, size_t* size);

// Null if record does not exist.

DBR_API struct DbrSlNode*
dbr_ctx_find_rec_id(DbrCtx ctx, int type, DbrIden id);

// Null if record does not exist.
// This function compares DBR_MNEM_MAX characters of mnem at most.

DBR_API struct DbrSlNode*
dbr_ctx_find_rec_mnem(DbrCtx ctx, int type, const char* mnem);

DBR_API struct DbrSlNode*
dbr_ctx_end_rec(DbrCtx ctx);

/** @} */

/**
 * @addtogroup Book
 * @{
 */

DBR_API struct DbrBook*
dbr_ctx_book(DbrCtx ctx, struct DbrRec* crec, DbrDate settl_date);

/** @} */

/**
 * @addtogroup Trader
 * @{
 */

DBR_API DbrTrader
dbr_ctx_trader(DbrCtx ctx, struct DbrRec* trec);

/** @} */

/**
 * @addtogroup Accnt
 * @{
 */

DBR_API DbrAccnt
dbr_ctx_accnt(DbrCtx ctx, struct DbrRec* arec);

/** @} */

/**
 * @addtogroup Exec
 * @{
 */

/**
 * @brief Submit order.
 * All members of trans are set to zero on failure.
 */

DBR_API struct DbrOrder*
dbr_ctx_submit(DbrCtx ctx, struct DbrRec* trec, struct DbrRec* arec, struct DbrBook* book,
               const char* ref, int action, DbrTicks ticks, DbrLots lots, DbrLots min,
               DbrFlags flags, struct DbrTrans* trans);

// Assumes that order already belongs to this side.
// Reduced lots must not be:
// 1. less than executed lots;
// 2. less than min lots;
// 3. greater than original lots.

DBR_API struct DbrOrder*
dbr_ctx_revise_id(DbrCtx ctx, DbrTrader trader, DbrIden id, DbrLots lots);

DBR_API struct DbrOrder*
dbr_ctx_revise_ref(DbrCtx ctx, DbrTrader trader, const char* ref, DbrLots lots);

DBR_API struct DbrOrder*
dbr_ctx_cancel_id(DbrCtx ctx, DbrTrader trader, DbrIden id);

DBR_API struct DbrOrder*
dbr_ctx_cancel_ref(DbrCtx ctx, DbrTrader trader, const char* ref);

// Invalidates any pointers to the trade.

DBR_API DbrBool
dbr_ctx_archive_order(DbrCtx ctx, DbrTrader trader, DbrIden id);

// Invalidates any pointers to the trade.

DBR_API DbrBool
dbr_ctx_archive_trade(DbrCtx ctx, DbrAccnt accnt, DbrIden id);

DBR_API void
dbr_ctx_free_matches(DbrCtx ctx, struct DbrSlNode* first);

/** @} */

#endif // DBR_CTX_H
