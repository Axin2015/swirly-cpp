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
    return order->c.user.rec->id == trid && strncmp(order->c.ref, ref, DBR_REF_MAX) == 0;
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
    if (order->c.ref[0] != '\0') {
        const size_t bucket = hash_ref(order->c.user.rec->id, order->c.ref) % FIG_ORDIDX_BUCKETS;
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
