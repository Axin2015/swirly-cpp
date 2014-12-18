/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_FIG_SESS_H
#define DBR_FIG_SESS_H

#include <dbr/elm/types.h>

/**
 * @addtogroup Sess
 * @{
 */

DBR_API void
dbr_sess_init(struct DbrSess* sess, const DbrUuid uuid, DbrPool pool);

DBR_API void
dbr_sess_term(struct DbrSess* sess);

DBR_API void
dbr_sess_reset(struct DbrSess* sess);

DBR_API DbrBool
dbr_sess_logon(struct DbrSess* sess, DbrAccnt accnt);

DBR_API void
dbr_sess_logoff(struct DbrSess* sess, DbrAccnt accnt);

/**
 * Logoff and reset state associated with @a accnt.
 */

DBR_API void
dbr_sess_logoff_and_reset(struct DbrSess* sess, DbrAccnt accnt);

DBR_API int
dbr_sess_subs(struct DbrSess* sess, DbrAccnt accnt);

#define DBR_SESS_END_ACCNT NULL

DBR_API DbrAccnt
dbr_sess_accnt_entry(struct DbrRbNode* node);

static inline struct DbrRbNode*
dbr_sess_find_accnt(const struct DbrSess* sess, DbrIden id)
{
    return dbr_tree_find(&sess->accnts, id);
}

static inline struct DbrRbNode*
dbr_sess_first_accnt(const struct DbrSess* sess)
{
    return dbr_tree_first(&sess->accnts);
}

static inline struct DbrRbNode*
dbr_sess_last_accnt(const struct DbrSess* sess)
{
    return dbr_tree_last(&sess->accnts);
}

static inline DbrBool
dbr_sess_empty_accnt(const struct DbrSess* sess)
{
    return dbr_tree_empty(&sess->accnts);
}

/** @} */

#endif // DBR_FIG_SESS_H
