/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_FIG_SESS_H
#define SC_FIG_SESS_H

#include <sc/elm/types.h>

/**
 * @addtogroup Sess
 * @{
 */

SC_API void
sc_sess_init(struct ScSess* sess, const ScUuid uuid, ScPool pool);

SC_API void
sc_sess_term(struct ScSess* sess);

SC_API void
sc_sess_reset(struct ScSess* sess);

SC_API ScBool
sc_sess_logon(struct ScSess* sess, ScAccnt accnt);

SC_API void
sc_sess_logoff(struct ScSess* sess, ScAccnt accnt);

/**
 * Logoff and reset state associated with @a accnt.
 */

SC_API void
sc_sess_logoff_and_reset(struct ScSess* sess, ScAccnt accnt);

SC_API int
sc_sess_subs(struct ScSess* sess, ScAccnt accnt);

#define SC_SESS_END_ACCNT NULL

SC_API ScAccnt
sc_sess_accnt_entry(struct ScRbNode* node);

static inline struct ScRbNode*
sc_sess_find_accnt(const struct ScSess* sess, ScIden id)
{
    return sc_tree_find(&sess->accnts, id);
}

static inline struct ScRbNode*
sc_sess_first_accnt(const struct ScSess* sess)
{
    return sc_tree_first(&sess->accnts);
}

static inline struct ScRbNode*
sc_sess_last_accnt(const struct ScSess* sess)
{
    return sc_tree_last(&sess->accnts);
}

static inline ScBool
sc_sess_empty_accnt(const struct ScSess* sess)
{
    return sc_tree_empty(&sess->accnts);
}

/** @} */

#endif // SC_FIG_SESS_H
