/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
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
#include "index.h"

#include <dbr/slnode.h>
#include <dbr/types.h>

#include <stdbool.h>
#include <string.h>

static inline struct DbrOrder*
order_entry_ref(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrOrder, ref_node_, node);
}

static inline DbrBool
equal_ref(const struct DbrOrder* order, DbrIden trid, const char* ref)
{
    return order->trader.rec->id == trid && strncmp(order->ref, ref, DBR_REF_MAX) == 0;
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
fig_index_init(struct FigIndex* index)
{
    // Zero buckets.
    memset(index->buckets, 0, sizeof(index->buckets));
}

DBR_EXTERN void
fig_index_insert(struct FigIndex* index, struct DbrOrder* order)
{
    if (order->ref[0] != '\0') {
        const size_t bucket = hash_ref(order->trader.rec->id, order->ref) % FIG_INDEX_BUCKETS;
        struct DbrStack* refs = &index->buckets[bucket].refs;
        dbr_stack_push(refs, &order->ref_node_);
    }
}

DBR_EXTERN struct DbrOrder*
fig_index_remove(struct FigIndex* index, DbrIden trid, const char* ref)
{
    assert(ref);
    const size_t bucket = hash_ref(trid, ref) % FIG_INDEX_BUCKETS;
    for (struct DbrSlNode** node = &index->buckets[bucket].refs.first;
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
fig_index_find(const struct FigIndex* index, DbrIden trid, const char* ref)
{
    assert(ref);
    const size_t bucket = hash_ref(trid, ref) % FIG_INDEX_BUCKETS;
    for (struct DbrSlNode* node = dbr_stack_first(&index->buckets[bucket].refs);
         node; node = node->next) {
        struct DbrOrder* order = order_entry_ref(node);
        if (equal_ref(order, trid, ref))
            return order;
    }
    return NULL;
}
