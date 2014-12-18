/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_FIG_SERV_H
#define SC_FIG_SERV_H

#include <sc/elm/journ.h>
#include <sc/elm/model.h>

/**
 * @addtogroup Serv
 * @{
 */

typedef struct FigServ* ScServ;

/**
 * Create Server.
 *
 * @param bank Bank.
 *
 * @param journ Journal.
 *
 * @param pool Pool.
 *
 * @return Handle to newly created Server or null on failure.
 */

SC_API ScServ
sc_serv_create(const char* bank, ScJourn journ, ScPool pool);

/**
 * No-op if @a serv is null.
 */

SC_API void
sc_serv_destroy(ScServ serv);

SC_API ScBool
sc_serv_load(ScServ serv, ScModel model);

#define SC_SERV_END_REC NULL

static inline struct ScRec*
sc_serv_rec_entry(struct ScSlNode* node)
{
    return sc_implof(struct ScRec, shared_node_, node);
}

/**
 * @return NULL if record does not exist.
 */

SC_API struct ScSlNode*
sc_serv_find_rec_id(ScServ serv, int type, ScIden id);

/**
 * This function compares #SC_MNEM_MAX characters of mnem at most.
 *
 * @return NULL if record does not exist.
 */

SC_API struct ScSlNode*
sc_serv_find_rec_mnem(ScServ serv, int type, const char* mnem);

/**
 * @return first record of requested type.
 */

/**
 * Size is optional.
 */

SC_API struct ScSlNode*
sc_serv_first_rec(ScServ serv, int type, size_t* size);

SC_API ScBool
sc_serv_empty_rec(ScServ serv, int type);

SC_API ScAccnt
sc_serv_accnt(ScServ serv, struct ScRec* arec);

SC_API struct ScBook*
sc_serv_book(ScServ serv, struct ScRec* crec, ScJd settl_day);

SC_API struct ScSess*
sc_serv_sess(ScServ serv, const ScUuid uuid);

/**
 * Place order.
 * All members of result are set to zero on failure.
 */

SC_API struct ScOrder*
sc_serv_place(ScServ serv, ScAccnt trader, ScAccnt giveup, struct ScBook* book,
               const char* ref, int action, ScTicks ticks, ScLots lots, ScLots min_lots);

/**
 * Assumes that order already belongs to this side.
 * Reduced lots must not be:
 * - less than executed lots;
 * - less than min lots;
 * - greater than original lots.
 */

SC_API struct ScOrder*
sc_serv_revise_id(ScServ serv, ScAccnt trader, ScIden id, ScLots lots);

SC_API struct ScOrder*
sc_serv_revise_ref(ScServ serv, ScAccnt trader, const char* ref, ScLots lots);

SC_API struct ScOrder*
sc_serv_cancel_id(ScServ serv, ScAccnt trader, ScIden id);

SC_API struct ScOrder*
sc_serv_cancel_ref(ScServ serv, ScAccnt trader, const char* ref);

SC_API ScBool
sc_serv_ack_trade(ScServ serv, ScAccnt trader, ScIden id);

SC_API void
sc_serv_clear_md(ScServ serv);

SC_API void
sc_serv_clear_tr(ScServ serv);

static inline void
sc_serv_clear(ScServ serv)
{
    sc_serv_clear_md(serv);
    sc_serv_clear_tr(serv);
}

#define SC_SERV_END_EXEC NULL

static inline struct ScExec*
sc_serv_exec_entry(struct ScSlNode* node)
{
    return sc_implof(struct ScExec, shared_node_, node);
}

SC_API struct ScSlNode*
sc_serv_first_exec(ScServ serv);

SC_API ScBool
sc_serv_empty_exec(ScServ serv);

#define SC_SERV_END_POSNUP NULL

static inline struct ScPosn*
sc_serv_posnup_entry(struct ScRbNode* node)
{
    return sc_implof(struct ScPosn, update_node_, node);
}

SC_API struct ScRbNode*
sc_serv_first_posnup(ScServ serv);

SC_API struct ScRbNode*
sc_serv_last_posnup(ScServ serv);

SC_API ScBool
sc_serv_empty_posnup(ScServ serv);

#define SC_SERV_END_BOOK NULL

static inline struct ScBook*
sc_serv_book_entry(struct ScRbNode* node)
{
    return sc_implof(struct ScBook, serv_node_, node);
}

SC_API struct ScRbNode*
sc_serv_find_book(ScServ serv, ScIden cid, ScJd settl_day);

SC_API struct ScRbNode*
sc_serv_first_book(ScServ serv);

SC_API struct ScRbNode*
sc_serv_last_book(ScServ serv);

SC_API ScBool
sc_serv_empty_book(ScServ serv);

#define SC_SERV_END_BOOKUP NULL

static inline struct ScBook*
sc_serv_bookup_entry(struct ScRbNode* node)
{
    return sc_implof(struct ScBook, update_node_, node);
}

SC_API struct ScRbNode*
sc_serv_first_bookup(ScServ serv);

SC_API struct ScRbNode*
sc_serv_last_bookup(ScServ serv);

SC_API ScBool
sc_serv_empty_bookup(ScServ serv);

/** @} */

#endif // SC_FIG_SERV_H
