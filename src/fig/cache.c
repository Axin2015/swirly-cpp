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
#include "cache.h"

#include <dbr/err.h>
#include <dbr/types.h>

#include <stdbool.h>
#include <string.h>

static inline struct DbrRec*
rec_entry_id(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrRec, id_node_, node);
}

static inline struct DbrRec*
rec_entry_mnem(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrRec, mnem_node_, node);
}

static void
term_state_noop(struct DbrRec* rec)
{
}

static inline void
free_rec_list(struct DbrSlNode* node, void (*term_state)(struct DbrRec*), DbrPool pool)
{
    while (node) {
        struct DbrRec* rec = dbr_shared_rec_entry(node);
        node = node->next;
        term_state(rec);
        dbr_pool_free_rec(pool, rec);
    }
}

static inline DbrBool
equal_id(const struct DbrRec* rec, int type, DbrIden id)
{
    return rec->type == type && rec->id == id;
}

static inline DbrBool
equal_mnem(const struct DbrRec* rec, int type, const char* mnem)
{
    return rec->type == type && strncmp(rec->mnem, mnem, DBR_MNEM_MAX) == 0;
}

static inline size_t
hash_id(int type, DbrIden id)
{
    size_t h = type;
    h ^= id + 0x9e3779b9 + (h << 6) + (h >> 2);
    return h;
}

static inline size_t
hash_mnem(int type, const char* mnem)
{
    size_t h = type;
    for (int i = 0; *mnem != '\0' && i < DBR_MNEM_MAX; ++mnem, ++i)
        h = 31 * h + *mnem;
    return h;
}

static inline struct DbrRec*
get_id(const struct FigCache* cache, int type, DbrIden id)
{
    struct DbrSlNode* node = fig_cache_find_rec_id(cache, type, id);
    assert(node != FIG_CACHE_END_REC);
    return dbr_shared_rec_entry(node);
}

static inline void
insert_id(struct FigCache* cache, struct DbrRec* rec)
{
    const size_t bucket = hash_id(rec->type, rec->id) % FIG_CACHE_BUCKETS;
    struct DbrStack* ids = &cache->buckets[bucket].ids;
    dbr_stack_push(ids, &rec->id_node_);
}

static inline void
insert_mnem(struct FigCache* cache, struct DbrRec* rec)
{
    const size_t bucket = hash_mnem(rec->type, rec->mnem) % FIG_CACHE_BUCKETS;
    struct DbrStack* mnems = &cache->buckets[bucket].mnems;
    dbr_stack_push(mnems, &rec->mnem_node_);
}

static void
emplace_trader(struct FigCache* cache, struct DbrSlNode* first, size_t size)
{
    assert(!cache->first_trader);
    cache->first_trader = first;
    cache->trader_size = size;
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrRec* rec = dbr_shared_rec_entry(node);
        insert_id(cache, rec);
        insert_mnem(cache, rec);
    }
}

static void
emplace_accnt(struct FigCache* cache, struct DbrSlNode* first, size_t size)
{
    assert(!cache->first_accnt);
    cache->first_accnt = first;
    cache->accnt_size = size;
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrRec* rec = dbr_shared_rec_entry(node);
        insert_id(cache, rec);
        insert_mnem(cache, rec);
    }
}

static void
emplace_contr(struct FigCache* cache, struct DbrSlNode* first, size_t size)
{
    assert(!cache->first_contr);
    cache->first_contr = first;
    cache->contr_size = size;
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrRec* rec = dbr_shared_rec_entry(node);
        insert_id(cache, rec);
        insert_mnem(cache, rec);
    }
}

DBR_EXTERN void
fig_cache_init(struct FigCache* cache, void (*term_state)(struct DbrRec*), DbrPool pool)
{
    cache->term_state = term_state ? term_state : term_state_noop;
    cache->pool = pool;
    cache->first_trader = NULL;
    cache->trader_size = 0;
    cache->first_accnt = NULL;
    cache->accnt_size = 0;
    cache->first_contr = NULL;
    cache->contr_size = 0;
    // Zero buckets.
    memset(cache->buckets, 0, sizeof(cache->buckets));
}

DBR_EXTERN void
fig_cache_term(struct FigCache* cache)
{
    free_rec_list(cache->first_contr, cache->term_state, cache->pool);
    free_rec_list(cache->first_accnt, cache->term_state, cache->pool);
    free_rec_list(cache->first_trader, cache->term_state, cache->pool);
}

DBR_EXTERN void
fig_cache_emplace_rec_list(struct FigCache* cache, int type, struct DbrSlNode* first, size_t size)
{
    switch (type) {
    case DBR_TRADER:
        emplace_trader(cache, first, size);
        break;
    case DBR_ACCNT:
        emplace_accnt(cache, first, size);
        break;
    case DBR_CONTR:
        emplace_contr(cache, first, size);
        break;
    default:
        assert(false);
        break;
    }
}

DBR_EXTERN struct DbrSlNode*
fig_cache_first_rec(struct FigCache* cache, int type, size_t* size)
{
    struct DbrSlNode* first;
    switch (type) {
    case DBR_TRADER:
        first = cache->first_trader;
        if (size)
            *size = cache->trader_size;
        break;
    case DBR_ACCNT:
        first = cache->first_accnt;
        if (size)
            *size = cache->accnt_size;
        break;
    case DBR_CONTR:
        first = cache->first_contr;
        if (size)
            *size = cache->contr_size;
        break;
    default:
        assert(false);
        first = NULL;
        if (size)
            *size = 0;
    }
    return first;
}

DBR_EXTERN struct DbrSlNode*
fig_cache_find_rec_id(const struct FigCache* cache, int type, DbrIden id)
{
    const size_t bucket = hash_id(type, id) % FIG_CACHE_BUCKETS;
    for (struct DbrSlNode* node = dbr_stack_first(&cache->buckets[bucket].ids);
         node; node = node->next) {
        struct DbrRec* rec = rec_entry_id(node);
        if (equal_id(rec, type, id))
            return &rec->shared_node_;
    }
    return NULL;
}

DBR_EXTERN struct DbrSlNode*
fig_cache_find_rec_mnem(const struct FigCache* cache, int type, const char* mnem)
{
    assert(mnem);
    const size_t bucket = hash_mnem(type, mnem) % FIG_CACHE_BUCKETS;
    for (struct DbrSlNode* node = dbr_stack_first(&cache->buckets[bucket].mnems);
         node; node = node->next) {
        struct DbrRec* rec = rec_entry_mnem(node);
        if (equal_mnem(rec, type, mnem))
            return &rec->shared_node_;
    }
    return NULL;
}
