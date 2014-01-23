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
#include "sessidx.h"

#include <dbr/err.h>
#include <dbr/slnode.h>
#include <dbr/types.h>

#include <string.h>

static inline struct DbrSess*
sess_entry_mnem(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrSess, mnem_node_, node);
}

static inline void
free_sess_list(struct DbrSlNode* node, DbrPool pool)
{
    while (node) {
        struct DbrSess* sess = sess_entry_mnem(node);
        node = node->next;
        dbr_pool_free_sess(pool, sess);
    }
}

static inline DbrBool
equal_mnem(const struct DbrSess* sess, const char* mnem)
{
    return strncmp(sess->mnem, mnem, DBR_MNEM_MAX) == 0;
}

static inline size_t
hash_mnem(const char* mnem)
{
    size_t h = 0;
    for (int i = 0; *mnem != '\0' && i < DBR_MNEM_MAX; ++mnem, ++i)
        h = 31 * h + *mnem;
    return h;
}

static struct DbrSess*
find_sess(const struct FigSessIdx* sessidx, const char* mnem)
{
    assert(mnem);
    const size_t bucket = hash_mnem(mnem) % FIG_SESSIDX_BUCKETS;
    for (struct DbrSlNode* node = dbr_stack_first(&sessidx->buckets[bucket].mnems);
         node; node = node->next) {
        struct DbrSess* sess = sess_entry_mnem(node);
        if (equal_mnem(sess, mnem))
            return sess;
    }
    return NULL;
}

DBR_EXTERN void
fig_sessidx_init(struct FigSessIdx* sessidx, DbrPool pool)
{
    // Zero buckets.
    memset(sessidx->buckets, 0, sizeof(sessidx->buckets));
}

DBR_EXTERN void
fig_sessidx_term(struct FigSessIdx* sessidx)
{
    // Free each bucket.
    for (size_t i = 0; i < FIG_SESSIDX_BUCKETS; ++i)
        free_sess_list(sessidx->buckets[i].mnems.first, sessidx->pool);
}

DBR_EXTERN DbrBool
fig_sessidx_open(const struct FigSessIdx* sessidx, const char* mnem)
{
    assert(mnem);
    struct DbrSess* sess = find_sess(sessidx, mnem);
    if (!sess) {
        if (!(sess = dbr_pool_alloc_sess(sessidx->pool)))
            return DBR_FALSE;
        dbr_sess_init(sess);
        strncpy(sess->mnem, mnem, DBR_MNEM_MAX);
        dbr_tree_init(&sess->traders);
    } else {
        // Close session if already open.
        fig_sessidx_close(sessidx, mnem);
    }
    return DBR_TRUE;
}

DBR_EXTERN void
fig_sessidx_close(const struct FigSessIdx* sessidx, const char* mnem)
{
}

DBR_EXTERN DbrBool
fig_sessidx_logon(const struct FigSessIdx* sessidx, const char* mnem, DbrTrader trader)
{
    assert(sessidx);
    assert(mnem);
    assert(trader);

    if (trader->sess) {
        dbr_err_setf(DBR_EEXIST, "already logged-on '%.16s'", trader->rec->mnem);
        goto fail1;
    }
    struct DbrSess* sess = find_sess(sessidx, mnem);
    if (!sess) {
        dbr_err_setf(DBR_EINVAL, "no such session '%.16s'", mnem);
        goto fail1;
    }
    trader->sess = sess;
    dbr_tree_insert(&sess->traders, trader->rec->id, &trader->sess_node_);
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

DBR_EXTERN void
fig_sessidx_logoff(const struct FigSessIdx* sessidx, const char* mnem, DbrTrader trader)
{
}
