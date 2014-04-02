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
#ifndef DBR_CLNT_H
#define DBR_CLNT_H

#include <dbr/handler.h>
#include <dbr/types.h>

#include <stddef.h>

typedef struct ElmPool* DbrPool;

/**
 * @addtogroup Clnt
 * @{
 */

typedef struct FigClnt* DbrClnt;

/**
 * Create client.
 *
 * @param ctx ZeroMQ context.
 *
 * @param sess Session mnemonic.
 *
 * @param mdaddr Address of market-data endpoint.
 *
 * @param traddr Address of trading endpoint.
 *
 * @param seed Seed identifier. Must be positive.
 *
 * @param tmout Request timeout.
 *
 * @param pool Pool.
 */

DBR_API DbrClnt
dbr_clnt_create(void* ctx, const char* sess, const char* mdaddr, const char* traddr,
                DbrIden seed, DbrMillis tmout, DbrPool pool);

// No-op if clnt is null.

DBR_API void
dbr_clnt_destroy(DbrClnt clnt);

DBR_API void
dbr_clnt_reset(DbrClnt clnt);

DBR_API DbrIden
dbr_clnt_close(DbrClnt clnt);

/**
 * @addtogroup ClntRec
 * @{
 */

#define DBR_CLNT_END_REC NULL

static inline struct DbrRec*
dbr_clnt_rec_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrRec, shared_node_, node);
}

// Null if record does not exist.

DBR_API struct DbrSlNode*
dbr_clnt_find_rec_id(DbrClnt clnt, int type, DbrIden id);

// Null if record does not exist.
// This function compares DBR_MNEM_MAX characters of mnem at most.

DBR_API struct DbrSlNode*
dbr_clnt_find_rec_mnem(DbrClnt clnt, int type, const char* mnem);

/**
 * Returns first record of requested type.
 */

// Size is optional.

DBR_API struct DbrSlNode*
dbr_clnt_first_rec(DbrClnt clnt, int type, size_t* size);

DBR_API DbrBool
dbr_clnt_empty_rec(DbrClnt clnt, int type);

/** @} */

DBR_API DbrTrader
dbr_clnt_trader(DbrClnt clnt, struct DbrRec* trec);

DBR_API DbrAccnt
dbr_clnt_accnt(DbrClnt clnt, struct DbrRec* arec);

DBR_API DbrIden
dbr_clnt_logon(DbrClnt clnt, DbrTrader trader);

DBR_API DbrIden
dbr_clnt_logoff(DbrClnt clnt, DbrTrader trader);

/**
 * Place order.
 * All members of result are set to zero on failure.
 */

DBR_API DbrIden
dbr_clnt_place(DbrClnt clnt, DbrTrader trader, DbrAccnt accnt, struct DbrRec* crec,
               DbrDate settl_date, const char* ref, int action, DbrTicks ticks, DbrLots lots,
               DbrLots min_lots);

// Assumes that order already belongs to this side.
// Reduced lots must not be:
// 1. less than executed lots;
// 2. less than min lots;
// 3. greater than original lots.

DBR_API DbrIden
dbr_clnt_revise_id(DbrClnt clnt, DbrTrader trader, DbrIden id, DbrLots lots);

DBR_API DbrIden
dbr_clnt_revise_ref(DbrClnt clnt, DbrTrader trader, const char* ref, DbrLots lots);

DBR_API DbrIden
dbr_clnt_cancel_id(DbrClnt clnt, DbrTrader trader, DbrIden id);

DBR_API DbrIden
dbr_clnt_cancel_ref(DbrClnt clnt, DbrTrader trader, const char* ref);

// Invalidates any pointers to the trade.

DBR_API DbrIden
dbr_clnt_ack_trade(DbrClnt clnt, DbrTrader trader, DbrIden id);

DBR_API DbrBool
dbr_clnt_is_ready(DbrClnt clnt);

DBR_API DbrIden
dbr_clnt_settimer(DbrClnt clnt, DbrMillis absms);

DBR_API void
dbr_clnt_canceltimer(DbrClnt clnt, DbrIden id);

DBR_API int
dbr_clnt_dispatch(DbrClnt clnt, DbrMillis ms, DbrHandler handler);

DBR_API void
dbr_clnt_mdclear(DbrClnt clnt);

DBR_API void
dbr_clnt_trclear(DbrClnt clnt);

static inline void
dbr_clnt_clear(DbrClnt clnt)
{
    dbr_clnt_mdclear(clnt);
    dbr_clnt_trclear(clnt);
}

/**
 * @addtogroup ClntExec
 * @{
 */

#define DBR_CLNT_END_EXEC NULL

static inline struct DbrExec*
dbr_clnt_exec_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrExec, shared_node_, node);
}

DBR_API struct DbrSlNode*
dbr_clnt_first_exec(DbrClnt clnt);

DBR_API DbrBool
dbr_clnt_empty_exec(DbrClnt clnt);

/** @} */

/**
 * @addtogroup ClntPosn
 * @{
 */

#define DBR_CLNT_END_POSNUP NULL

static inline struct DbrPosn*
dbr_clnt_posnup_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrPosn, update_node_, node);
}

DBR_API struct DbrRbNode*
dbr_clnt_first_posnup(DbrClnt clnt);

DBR_API struct DbrRbNode*
dbr_clnt_last_posnup(DbrClnt clnt);

DBR_API DbrBool
dbr_clnt_empty_posnup(DbrClnt clnt);

/** @} */

/**
 * @addtogroup ClntView
 * @{
 */

#define DBR_CLNT_END_VIEW NULL

static inline struct DbrView*
dbr_clnt_view_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrView, clnt_node_, node);
}

DBR_API struct DbrRbNode*
dbr_clnt_find_view(DbrClnt clnt, DbrIden cid, DbrDate settl_date);

DBR_API struct DbrRbNode*
dbr_clnt_first_view(DbrClnt clnt);

DBR_API struct DbrRbNode*
dbr_clnt_last_view(DbrClnt clnt);

DBR_API DbrBool
dbr_clnt_empty_view(DbrClnt clnt);

#define DBR_CLNT_END_VIEWUP NULL

static inline struct DbrView*
dbr_clnt_viewup_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrView, update_node_, node);
}

DBR_API struct DbrRbNode*
dbr_clnt_first_viewup(DbrClnt clnt);

DBR_API struct DbrRbNode*
dbr_clnt_last_viewup(DbrClnt clnt);

DBR_API DbrBool
dbr_clnt_empty_viewup(DbrClnt clnt);

/** @} */

/** @} */

#endif // DBR_CLNT_H
