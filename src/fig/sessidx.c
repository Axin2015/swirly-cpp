/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "sessidx.h"

#include <sc/fig/sess.h>

#include <sc/elm/pool.h>
#include <sc/elm/types.h>

#include <sc/ash/err.h>
#include <sc/ash/slnode.h>

#include <string.h>

static inline struct ScSess*
sess_entry_uuid(struct ScSlNode* node)
{
    return sc_implof(struct ScSess, uuid_node_, node);
}

static inline void
free_sess_list(struct ScSlNode* node, ScPool pool)
{
    while (node) {
        struct ScSess* sess = sess_entry_uuid(node);
        node = node->next;
        sc_sess_term(sess);
        sc_pool_free_sess(pool, sess);
    }
}

static inline ScBool
equal_uuid(const struct ScSess* sess, const ScUuid uuid)
{
    return __builtin_memcmp(sess->uuid, uuid, 16) == 0;
}

static inline size_t
hash_uuid(const ScUuid uuid)
{
    size_t h = 0;
    for (int i = 0; i < 16; ++uuid, ++i)
        h = 31 * h + *uuid;
    return h;
}

SC_EXTERN void
fig_sessidx_init(struct FigSessIdx* sessidx, ScPool pool)
{
    sessidx->pool = pool;
    // Zero buckets.
    memset(sessidx->buckets, 0, sizeof(sessidx->buckets));
}

SC_EXTERN void
fig_sessidx_term(struct FigSessIdx* sessidx)
{
    // Free each bucket.
    for (size_t i = 0; i < FIG_SESSIDX_BUCKETS; ++i)
        free_sess_list(sessidx->buckets[i].uuids.first, sessidx->pool);
}

SC_EXTERN struct ScSess*
fig_sessidx_lazy(struct FigSessIdx* sessidx, const ScUuid uuid)
{
    assert(uuid);

    struct ScSess* sess;

    const size_t bucket = hash_uuid(uuid) % FIG_SESSIDX_BUCKETS;
    for (struct ScSlNode* node = sc_stack_first(&sessidx->buckets[bucket].uuids);
         node; node = node->next) {
        sess = sess_entry_uuid(node);
        if (equal_uuid(sess, uuid))
            goto done;
    }
    if (!(sess = sc_pool_alloc_sess(sessidx->pool)))
        goto done;

    sc_sess_init(sess, uuid, sessidx->pool);
    sc_stack_push(&sessidx->buckets[bucket].uuids, &sess->uuid_node_);
 done:
    return sess;
}
