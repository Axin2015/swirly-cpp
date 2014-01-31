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
#include <dbr/sess.h>
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
        dbr_sess_term(sess);
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

DBR_EXTERN void
fig_sessidx_init(struct FigSessIdx* sessidx, DbrPool pool)
{
    sessidx->pool = pool;
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

DBR_EXTERN struct DbrSess*
fig_sessidx_lazy(struct FigSessIdx* sessidx, const char* mnem)
{
    assert(mnem);

    struct DbrSess* sess;

    const size_t bucket = hash_mnem(mnem) % FIG_SESSIDX_BUCKETS;
    for (struct DbrSlNode* node = dbr_stack_first(&sessidx->buckets[bucket].mnems);
         node; node = node->next) {
        sess = sess_entry_mnem(node);
        if (equal_mnem(sess, mnem))
            goto done;
    }
    if (!(sess = dbr_pool_alloc_sess(sessidx->pool)))
        goto done;

    dbr_sess_init(sess);
    strncpy(sess->mnem, mnem, DBR_MNEM_MAX);
    dbr_tree_init(&sess->traders);
    dbr_tree_init(&sess->subs);

    dbr_stack_push(&sessidx->buckets[bucket].mnems, &sess->mnem_node_);
 done:
    return sess;
}
