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
#ifndef DBR_SERV_H
#define DBR_SERV_H

#include <dbr/journ.h>
#include <dbr/model.h>

/**
 * @addtogroup Serv
 * @{
 */

typedef struct FigServ* DbrServ;

/**
 * Create Server.
 *
 * @param journ Journal.
 *
 * @param model Model.
 *
 * @param pool Pool.
 *
 * @return Handle to newly created Servange or null on failure.
 */

DBR_API DbrServ
dbr_serv_create(DbrJourn journ, DbrModel model, DbrPool pool);

// No-op if serv is null.

DBR_API void
dbr_serv_destroy(DbrServ serv);

/**
 * @addtogroup ServRec
 * @{
 */

#define DBR_SERV_END_REC NULL

static inline struct DbrRec*
dbr_serv_rec_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrRec, shared_node_, node);
}

// Null if record does not exist.

DBR_API struct DbrSlNode*
dbr_serv_find_rec_id(DbrServ serv, int type, DbrIden id);

// Null if record does not exist.
// This function compares DBR_MNEM_MAX characters of mnem at most.

DBR_API struct DbrSlNode*
dbr_serv_find_rec_mnem(DbrServ serv, int type, const char* mnem);

/**
 * Returns first record of requested type.
 */

// Size is optional.

DBR_API struct DbrSlNode*
dbr_serv_first_rec(DbrServ serv, int type, size_t* size);

DBR_API DbrBool
dbr_serv_empty_rec(DbrServ serv, int type);

/** @} */

DBR_API DbrTrader
dbr_serv_trader(DbrServ serv, struct DbrRec* trec);

DBR_API DbrAccnt
dbr_serv_accnt(DbrServ serv, struct DbrRec* arec);

DBR_API struct DbrBook*
dbr_serv_book(DbrServ serv, struct DbrRec* crec, DbrDate settl_date);

DBR_API struct DbrSess*
dbr_serv_sess(DbrServ serv, const char* mnem);

/**
 * Place order.
 * All members of result are set to zero on failure.
 */

DBR_API struct DbrOrder*
dbr_serv_place(DbrServ serv, DbrTrader trader, DbrAccnt accnt, struct DbrBook* book,
               const char* ref, int action, DbrTicks ticks, DbrLots lots, DbrLots min_lots);

// Assumes that order already belongs to this side.
// Reduced lots must not be:
// 1. less than executed lots;
// 2. less than min lots;
// 3. greater than original lots.

DBR_API struct DbrOrder*
dbr_serv_revise_id(DbrServ serv, DbrTrader trader, DbrIden id, DbrLots lots);

DBR_API struct DbrOrder*
dbr_serv_revise_ref(DbrServ serv, DbrTrader trader, const char* ref, DbrLots lots);

DBR_API struct DbrOrder*
dbr_serv_cancel_id(DbrServ serv, DbrTrader trader, DbrIden id);

DBR_API struct DbrOrder*
dbr_serv_cancel_ref(DbrServ serv, DbrTrader trader, const char* ref);

// Invalidates any pointers to the trade.

DBR_API DbrBool
dbr_serv_ack_trade(DbrServ serv, DbrTrader trader, DbrIden id);

DBR_API void
dbr_serv_mdclear(DbrServ serv);

DBR_API void
dbr_serv_trclear(DbrServ serv);

static inline void
dbr_serv_clear(DbrServ serv)
{
    dbr_serv_mdclear(serv);
    dbr_serv_trclear(serv);
}

/**
 * @addtogroup ServExec
 * @{
 */

#define DBR_SERV_END_EXEC NULL

static inline struct DbrExec*
dbr_serv_exec_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrExec, shared_node_, node);
}

DBR_API struct DbrSlNode*
dbr_serv_first_exec(DbrServ serv);

DBR_API DbrBool
dbr_serv_empty_exec(DbrServ serv);

/** @} */

/**
 * @addtogroup ServPosn
 * @{
 */

#define DBR_SERV_END_POSNUP NULL

static inline struct DbrPosn*
dbr_serv_posnup_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrPosn, update_node_, node);
}

DBR_API struct DbrRbNode*
dbr_serv_first_posnup(DbrServ serv);

DBR_API struct DbrRbNode*
dbr_serv_last_posnup(DbrServ serv);

DBR_API DbrBool
dbr_serv_empty_posnup(DbrServ serv);

/** @} */

/**
 * @addtogroup ServBook
 * @{
 */

#define DBR_SERV_END_BOOK NULL

static inline struct DbrBook*
dbr_serv_book_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrBook, serv_node_, node);
}

DBR_API struct DbrRbNode*
dbr_serv_find_book(DbrServ serv, DbrIden cid, DbrDate settl_date);

DBR_API struct DbrRbNode*
dbr_serv_first_book(DbrServ serv);

DBR_API struct DbrRbNode*
dbr_serv_last_book(DbrServ serv);

DBR_API DbrBool
dbr_serv_empty_book(DbrServ serv);

#define DBR_SERV_END_BOOKUP NULL

static inline struct DbrBook*
dbr_serv_bookup_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrBook, update_node_, node);
}

DBR_API struct DbrRbNode*
dbr_serv_first_bookup(DbrServ serv);

DBR_API struct DbrRbNode*
dbr_serv_last_bookup(DbrServ serv);

DBR_API DbrBool
dbr_serv_empty_bookup(DbrServ serv);

/** @} */

/** @} */

#endif // DBR_SERV_H
