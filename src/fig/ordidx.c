/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "ordidx.h"

#include <sc/elm/types.h>

#include <sc/ash/slnode.h>

#include <string.h>

static inline struct ScOrder*
order_entry_ref(struct ScSlNode* node)
{
    return sc_implof(struct ScOrder, ref_node_, node);
}

static inline ScBool
equal_ref(const struct ScOrder* order, ScIden trid, const char* ref)
{
    return order->i.trader.rec->id == trid && strncmp(order->i.ref, ref, SC_REF_MAX) == 0;
}

static inline size_t
hash_ref(ScIden trid, const char* ref)
{
    size_t h = trid;
    for (int i = 0; *ref != '\0' && i < SC_REF_MAX; ++ref, ++i)
        h = 31 * h + *ref;
    return h;
}

SC_EXTERN void
fig_ordidx_init(struct FigOrdIdx* ordidx)
{
    // Zero buckets.
    memset(ordidx->buckets, 0, sizeof(ordidx->buckets));
}

SC_EXTERN void
fig_ordidx_insert(struct FigOrdIdx* ordidx, struct ScOrder* order)
{
    if (order->i.ref[0] != '\0') {
        const size_t bucket = hash_ref(order->i.trader.rec->id, order->i.ref) % FIG_ORDIDX_BUCKETS;
        struct ScStack* refs = &ordidx->buckets[bucket].refs;
        sc_stack_push(refs, &order->ref_node_);
    }
}

SC_EXTERN struct ScOrder*
fig_ordidx_remove(struct FigOrdIdx* ordidx, ScIden trid, const char* ref)
{
    assert(ref);
    const size_t bucket = hash_ref(trid, ref) % FIG_ORDIDX_BUCKETS;
    for (struct ScSlNode** node = &ordidx->buckets[bucket].refs.first;
         *node; node = &(*node)->next) {
        struct ScOrder* order = order_entry_ref(*node);
        if (equal_ref(order, trid, ref)) {
            *node = (*node)->next;
            sc_slnode_init(&order->ref_node_);
            return order;
        }
    }
    return NULL;
}

SC_EXTERN struct ScOrder*
fig_ordidx_find(const struct FigOrdIdx* ordidx, ScIden trid, const char* ref)
{
    assert(ref);
    const size_t bucket = hash_ref(trid, ref) % FIG_ORDIDX_BUCKETS;
    for (struct ScSlNode* node = sc_stack_first(&ordidx->buckets[bucket].refs);
         node; node = node->next) {
        struct ScOrder* order = order_entry_ref(node);
        if (equal_ref(order, trid, ref))
            return order;
    }
    return NULL;
}
