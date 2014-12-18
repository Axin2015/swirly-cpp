/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "ordidx.h"

#include <dbr/elm/types.h>

#include <dbr/ash/slnode.h>

#include <string.h>

static inline struct DbrOrder*
order_entry_ref(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrOrder, ref_node_, node);
}

static inline DbrBool
equal_ref(const struct DbrOrder* order, DbrIden trid, const char* ref)
{
    return order->i.trader.rec->id == trid && strncmp(order->i.ref, ref, DBR_REF_MAX) == 0;
}

static inline size_t
hash_ref(DbrIden trid, const char* ref)
{
    size_t h = trid;
    for (int i = 0; *ref != '\0' && i < DBR_REF_MAX; ++ref, ++i)
        h = 31 * h + *ref;
    return h;
}

DBR_EXTERN void
fig_ordidx_init(struct FigOrdIdx* ordidx)
{
    // Zero buckets.
    memset(ordidx->buckets, 0, sizeof(ordidx->buckets));
}

DBR_EXTERN void
fig_ordidx_insert(struct FigOrdIdx* ordidx, struct DbrOrder* order)
{
    if (order->i.ref[0] != '\0') {
        const size_t bucket = hash_ref(order->i.trader.rec->id, order->i.ref) % FIG_ORDIDX_BUCKETS;
        struct DbrStack* refs = &ordidx->buckets[bucket].refs;
        dbr_stack_push(refs, &order->ref_node_);
    }
}

DBR_EXTERN struct DbrOrder*
fig_ordidx_remove(struct FigOrdIdx* ordidx, DbrIden trid, const char* ref)
{
    assert(ref);
    const size_t bucket = hash_ref(trid, ref) % FIG_ORDIDX_BUCKETS;
    for (struct DbrSlNode** node = &ordidx->buckets[bucket].refs.first;
         *node; node = &(*node)->next) {
        struct DbrOrder* order = order_entry_ref(*node);
        if (equal_ref(order, trid, ref)) {
            *node = (*node)->next;
            dbr_slnode_init(&order->ref_node_);
            return order;
        }
    }
    return NULL;
}

DBR_EXTERN struct DbrOrder*
fig_ordidx_find(const struct FigOrdIdx* ordidx, DbrIden trid, const char* ref)
{
    assert(ref);
    const size_t bucket = hash_ref(trid, ref) % FIG_ORDIDX_BUCKETS;
    for (struct DbrSlNode* node = dbr_stack_first(&ordidx->buckets[bucket].refs);
         node; node = node->next) {
        struct DbrOrder* order = order_entry_ref(node);
        if (equal_ref(order, trid, ref))
            return order;
    }
    return NULL;
}
