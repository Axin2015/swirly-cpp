/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_FIG_CLNT_H
#define SC_FIG_CLNT_H

#include <sc/elm/pool.h>

#include <stddef.h>

/**
 * @addtogroup Clnt
 * @{
 */

typedef struct FigClnt* ScClnt;

/**
 * Create client.
 *
 * @param zctx ZeroMQ context.
 *
 * @param uuid Unique session-id.
 *
 * @param mdaddr Address of market-data endpoint.
 *
 * @param traddr Address of trading endpoint.
 *
 * @param tmout Request timeout.
 *
 * @param pool Pool.
 */

SC_API ScClnt
sc_clnt_create(void* zctx, const ScUuid uuid, const char* mdaddr, const char* traddr,
                ScMillis tmout, ScPool pool);

/**
 * No-op if @a clnt is null.
 */

SC_API void
sc_clnt_destroy(ScClnt clnt);

SC_API void
sc_clnt_reset(ScClnt clnt);

SC_API ScIden
sc_clnt_close(ScClnt clnt);

#define SC_CLNT_END_REC NULL

static inline struct ScRec*
sc_clnt_rec_entry(struct ScSlNode* node)
{
    return sc_implof(struct ScRec, shared_node_, node);
}

/**
 * @return NULL if record does not exist.
 */

SC_API struct ScSlNode*
sc_clnt_find_rec_id(ScClnt clnt, int type, ScIden id);

/**
 * Compares #SC_MNEM_MAX characters of mnem at most.
 *
 * @return NULL if record does not exist.
 */

SC_API struct ScSlNode*
sc_clnt_find_rec_mnem(ScClnt clnt, int type, const char* mnem);

/**
 * @return first record of requested type.
 */

/**
 * Size is optional.
 */

SC_API struct ScSlNode*
sc_clnt_first_rec(ScClnt clnt, int type, size_t* size);

SC_API ScBool
sc_clnt_empty_rec(ScClnt clnt, int type);

SC_API ScAccnt
sc_clnt_accnt(ScClnt clnt, struct ScRec* arec);

SC_API ScIden
sc_clnt_logon(ScClnt clnt, ScAccnt accnt);

SC_API ScIden
sc_clnt_logoff(ScClnt clnt, ScAccnt accnt);

/**
 * Place order.
 * All members of result are set to zero on failure.
 */

SC_API ScIden
sc_clnt_place(ScClnt clnt, ScAccnt trader, ScAccnt giveup, struct ScRec* crec,
               ScJd settl_day, const char* ref, int action, ScTicks ticks, ScLots lots,
               ScLots min_lots);

/**
 * Assumes that order already belongs to this side.
 * Reduced lots must not be:
 * - less than executed lots;
 * - less than min lots;
 * - greater than original lots.
 */

SC_API ScIden
sc_clnt_revise_id(ScClnt clnt, ScAccnt trader, ScIden id, ScLots lots);

SC_API ScIden
sc_clnt_revise_ref(ScClnt clnt, ScAccnt trader, const char* ref, ScLots lots);

SC_API ScIden
sc_clnt_cancel_id(ScClnt clnt, ScAccnt trader, ScIden id);

SC_API ScIden
sc_clnt_cancel_ref(ScClnt clnt, ScAccnt trader, const char* ref);

SC_API ScIden
sc_clnt_ack_trade(ScClnt clnt, ScAccnt trader, ScIden id);

SC_API ScIden
sc_clnt_set_timer(ScClnt clnt, ScMillis absms);

SC_API void
sc_clnt_cancel_timer(ScClnt clnt, ScIden id);

#define SC_CLNT_END_VIEW NULL

static inline struct ScView*
sc_clnt_view_entry(struct ScRbNode* node)
{
    return sc_implof(struct ScView, clnt_node_, node);
}

SC_API struct ScRbNode*
sc_clnt_find_view(ScClnt clnt, ScIden cid, ScJd settl_day);

SC_API struct ScRbNode*
sc_clnt_first_view(ScClnt clnt);

SC_API struct ScRbNode*
sc_clnt_last_view(ScClnt clnt);

SC_API ScBool
sc_clnt_empty_view(ScClnt clnt);

SC_API const unsigned char*
sc_clnt_uuid(ScClnt clnt);

SC_API ScBool
sc_clnt_is_closed(ScClnt clnt);

SC_API ScBool
sc_clnt_is_ready(ScClnt clnt);

/** @} */

#endif // SC_FIG_CLNT_H
