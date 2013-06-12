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

#include "accnt.h"
#include "market.h"
#include "pool.h"
#include "trader.h"

#include <assert.h>
#include <stdlib.h>
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
noterm(struct DbrRec* rec)
{
}

static inline void
free_recs(struct ElmPool* pool, struct DbrSlNode* node, void (*term)(struct DbrRec*))
{
    while (node) {
        struct DbrRec* rec = dbr_rec_entry(node);
        node = node->next;
        term(rec);
        elm_pool_free_rec(pool, rec);
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
get_id(const struct ElmCache* cache, int type, DbrIden id)
{
    struct DbrSlNode* node = elm_cache_find_id(cache, type, id);
    assert(node != elm_cache_end(cache));
    return dbr_rec_entry(node);
}

static inline void
insert_id(struct ElmCache* cache, struct DbrRec* rec)
{
    const size_t bucket = hash_id(rec->type, rec->id) % ELM_CACHE_BUCKETS;
    struct AshStack* ids = &cache->buckets[bucket].ids;
    ash_stack_push(ids, &rec->id_node_);
}

static inline void
insert_mnem(struct ElmCache* cache, struct DbrRec* rec)
{
    const size_t bucket = hash_mnem(rec->type, rec->mnem) % ELM_CACHE_BUCKETS;
    struct AshStack* mnems = &cache->buckets[bucket].mnems;
    ash_stack_push(mnems, &rec->mnem_node_);
}

static void
emplace_instr(struct ElmCache* cache, struct DbrSlNode* first, size_t size)
{
    assert(!cache->first_instr);
    cache->first_instr = first;
    cache->instr_size = size;
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrRec* rec = dbr_rec_entry(node);
        insert_id(cache, rec);
        insert_mnem(cache, rec);
    }
}

static void
emplace_market(struct ElmCache* cache, struct DbrSlNode* first, size_t size)
{
    assert(!cache->first_market);
    cache->first_market = first;
    cache->market_size = size;
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrRec* rec = dbr_rec_entry(node);
        rec->market.instr.rec = get_id(cache, DBR_INSTR, rec->market.instr.id);
        insert_id(cache, rec);
        insert_mnem(cache, rec);
    }
}

static void
emplace_trader(struct ElmCache* cache, struct DbrSlNode* first, size_t size)
{
    assert(!cache->first_trader);
    cache->first_trader = first;
    cache->trader_size = size;
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrRec* rec = dbr_rec_entry(node);
        insert_id(cache, rec);
        insert_mnem(cache, rec);
    }
}

static void
emplace_accnt(struct ElmCache* cache, struct DbrSlNode* first, size_t size)
{
    assert(!cache->first_accnt);
    cache->first_accnt = first;
    cache->accnt_size = size;
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrRec* rec = dbr_rec_entry(node);
        insert_id(cache, rec);
        insert_mnem(cache, rec);
    }
}

DBR_EXTERN void
elm_cache_init(struct ElmCache* cache, struct ElmPool* pool)
{
    cache->pool = pool;
    cache->first_instr = NULL;
    cache->instr_size = 0;
    cache->first_market = NULL;
    cache->market_size = 0;
    cache->first_accnt = NULL;
    cache->accnt_size = 0;
    cache->first_trader = NULL;
    cache->trader_size = 0;
    // Zero buckets.
    memset(cache->buckets, 0, sizeof(cache->buckets));
}

DBR_EXTERN void
elm_cache_term(struct ElmCache* cache)
{
    // Traders must be released before markets, because traders subscribe to markets.
    free_recs(cache->pool, cache->first_trader, elm_trader_term);
    free_recs(cache->pool, cache->first_accnt, elm_accnt_term);
    free_recs(cache->pool, cache->first_market, elm_market_term);
    free_recs(cache->pool, cache->first_instr, noterm);
}

DBR_EXTERN void
elm_cache_emplace(struct ElmCache* cache, int type, struct DbrSlNode* first, size_t size)
{
    switch (type) {
    case DBR_INSTR:
        emplace_instr(cache, first, size);
        break;
    case DBR_MARKET:
        emplace_market(cache, first, size);
        break;
    case DBR_TRADER:
        emplace_trader(cache, first, size);
        break;
    case DBR_ACCNT:
        emplace_accnt(cache, first, size);
        break;
    default:
        assert(false);
        break;
    }
}

DBR_EXTERN struct DbrSlNode*
elm_cache_entries(struct ElmCache* cache, int type, size_t* size)
{
    struct DbrSlNode* first;
    switch (type) {
    case DBR_INSTR:
        first = cache->first_instr;
        if (size)
            *size = cache->instr_size;
        break;
    case DBR_MARKET:
        first = cache->first_market;
        if (size)
            *size = cache->market_size;
        break;
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
    default:
        assert(false);
        first = NULL;
        if (size)
            *size = 0;
    }
    return first;
}

DBR_EXTERN struct DbrSlNode*
elm_cache_find_id(const struct ElmCache* cache, int type, DbrIden id)
{
    const size_t bucket = hash_id(type, id) % ELM_CACHE_BUCKETS;
    for (struct DbrSlNode* node = ash_stack_first(&cache->buckets[bucket].ids);
         node; node = node->next) {
        struct DbrRec* rec = rec_entry_id(node);
        if (equal_id(rec, type, id))
            return &rec->model_node_;
    }
    return NULL;
}

DBR_EXTERN struct DbrSlNode*
elm_cache_find_mnem(const struct ElmCache* cache, int type, const char* mnem)
{
    assert(mnem);
    const size_t bucket = hash_mnem(type, mnem) % ELM_CACHE_BUCKETS;
    for (struct DbrSlNode* node = ash_stack_first(&cache->buckets[bucket].mnems);
         node; node = node->next) {
        struct DbrRec* rec = rec_entry_mnem(node);
        if (equal_mnem(rec, type, mnem))
            return &rec->model_node_;
    }
    return NULL;
}
