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

#include <dbr/defs.h>
#include <dbr/types.h>

#include <zmq.h>

#include <stddef.h>

typedef struct ElmPool* DbrPool;

/**
 * @addtogroup Clnt
 * @{
 */

typedef struct FigClnt* DbrClnt;

struct DbrEvent {
    DbrIden req_id;
    /**
     * Message type received.
     */
    int type;
    union {
        struct {
            /**
             * If DbrEvent::type is #DBR_STATUS_REP, then this is set to the status number.
             */
            int num;
            /**
             * If DbrEvent::type is #DBR_STATUS_REP, then this is set to the status message.
             */
            char msg[DBR_ERRMSG_MAX];
        } status_rep;
        struct {
            /**
             * If DbrEvent::type is #DBR_EXEC_REP, then this is set to the execution. The
             * execution's reference count is not incremented, so dbr_exec_incref() must be used to
             * extend the lifetime beyond the next call to dbr_clnt_clear().
             */
            struct DbrExec* exec;
        } exec_rep;
    };
};

/**
 * Create ZMQite3 model.
 *
 * @param ctx ZeroMQ context.
 *
 * @param dealer_addr Address of dealer endpoint.
 *
 * @param sub_addr Address of subscriber endpoint.
 *
 * @param trader Trader mnemonic.
 *
 * @param seed Seed identifier. Must be positive.
 *
 * @param pool Pool.
 */

DBR_API DbrClnt
dbr_clnt_create(void* ctx, const char* dealer_addr, const char* sub_addr, const char* trader,
                DbrIden seed, DbrPool pool);

// No-op if clnt is null.

DBR_API void
dbr_clnt_destroy(DbrClnt clnt);

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
dbr_clnt_trader(DbrClnt clnt);

DBR_API DbrAccnt
dbr_clnt_accnt(DbrClnt clnt, struct DbrRec* arec);

/**
 * Place order.
 * All members of result are set to zero on failure.
 */

DBR_API DbrIden
dbr_clnt_place(DbrClnt clnt, const char* accnt, const char* contr, DbrDate settl_date,
               const char* ref, int action, DbrTicks ticks, DbrLots lots, DbrLots min_lots,
               DbrMillis ms);

// Assumes that order already belongs to this side.
// Reduced lots must not be:
// 1. less than executed lots;
// 2. less than min lots;
// 3. greater than original lots.

DBR_API DbrIden
dbr_clnt_revise_id(DbrClnt clnt, DbrIden id, DbrLots lots, DbrMillis ms);

DBR_API DbrIden
dbr_clnt_revise_ref(DbrClnt clnt, const char* ref, DbrLots lots, DbrMillis ms);

DBR_API DbrIden
dbr_clnt_cancel_id(DbrClnt clnt, DbrIden id, DbrMillis ms);

DBR_API DbrIden
dbr_clnt_cancel_ref(DbrClnt clnt, const char* ref, DbrMillis ms);

// Invalidates any pointers to the trade.

DBR_API DbrIden
dbr_clnt_ack_trade(DbrClnt clnt, DbrIden id, DbrMillis ms);

DBR_API DbrBool
dbr_clnt_ready(DbrClnt clnt);

DBR_API DbrIden
dbr_clnt_settimer(DbrClnt clnt, DbrMillis absms);

DBR_API void
dbr_clnt_canceltimer(DbrClnt clnt, DbrIden id);

DBR_API zmq_pollitem_t*
dbr_clnt_setitems(DbrClnt clnt, zmq_pollitem_t* items, int nitems);

DBR_API int
dbr_clnt_poll(DbrClnt clnt, DbrMillis ms, struct DbrEvent* event);

DBR_API void
dbr_clnt_clear(DbrClnt clnt);

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
