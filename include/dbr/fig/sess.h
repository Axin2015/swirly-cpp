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
dbr_sess_logon(struct DbrSess* sess, DbrAccnt trader);

DBR_API void
dbr_sess_logoff(struct DbrSess* sess, DbrAccnt trader);

/**
 * Logoff and reset state associated with @a trader.
 */

DBR_API void
dbr_sess_logoff_and_reset(struct DbrSess* sess, DbrAccnt trader);

DBR_API int
dbr_sess_subs(struct DbrSess* sess, DbrAccnt trader);

#define DBR_SESS_END_TRADER NULL

DBR_API DbrAccnt
dbr_sess_trader_entry(struct DbrRbNode* node);

static inline struct DbrRbNode*
dbr_sess_find_trader(const struct DbrSess* sess, DbrIden id)
{
    return dbr_tree_find(&sess->traders, id);
}

static inline struct DbrRbNode*
dbr_sess_first_trader(const struct DbrSess* sess)
{
    return dbr_tree_first(&sess->traders);
}

static inline struct DbrRbNode*
dbr_sess_last_trader(const struct DbrSess* sess)
{
    return dbr_tree_last(&sess->traders);
}

static inline DbrBool
dbr_sess_empty_trader(const struct DbrSess* sess)
{
    return dbr_tree_empty(&sess->traders);
}

/** @} */

#endif // DBR_FIG_SESS_H
