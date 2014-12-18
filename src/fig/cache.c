/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "cache.h"

#include <sc/elm/pool.h>

#include <sc/ash/err.h>

#include <stdlib.h> // abort()
#include <string.h>

static inline struct ScRec*
rec_entry_id(struct ScSlNode* node)
{
    return sc_implof(struct ScRec, id_node_, node);
}

static inline struct ScRec*
rec_entry_mnem(struct ScSlNode* node)
{
    return sc_implof(struct ScRec, mnem_node_, node);
}

static void
term_state_noop(struct ScRec* rec)
{
}

static inline void
free_rec_list(struct ScSlNode* node, void (*term_state)(struct ScRec*), ScPool pool)
{
    while (node) {
        struct ScRec* rec = sc_shared_rec_entry(node);
        node = node->next;
        term_state(rec);
        sc_pool_free_rec(pool, rec);
    }
}

static inline ScBool
equal_id(const struct ScRec* rec, int type, ScIden id)
{
    return rec->type == type && rec->id == id;
}

static inline ScBool
equal_mnem(const struct ScRec* rec, int type, const char* mnem)
{
    return rec->type == type && strncmp(rec->mnem, mnem, SC_MNEM_MAX) == 0;
}

static inline size_t
hash_id(int type, ScIden id)
{
    size_t h = type;
    h ^= id + 0x9e3779b9 + (h << 6) + (h >> 2);
    return h;
}

static inline size_t
hash_mnem(int type, const char* mnem)
{
    size_t h = type;
    for (int i = 0; *mnem != '\0' && i < SC_MNEM_MAX; ++mnem, ++i)
        h = 31 * h + *mnem;
    return h;
}

static inline void
insert_id(struct FigCache* cache, struct ScRec* rec)
{
    const size_t bucket = hash_id(rec->type, rec->id) % FIG_CACHE_BUCKETS;
    struct ScStack* ids = &cache->buckets[bucket].ids;
    sc_stack_push(ids, &rec->id_node_);
}

static inline void
insert_mnem(struct FigCache* cache, struct ScRec* rec)
{
    const size_t bucket = hash_mnem(rec->type, rec->mnem) % FIG_CACHE_BUCKETS;
    struct ScStack* mnems = &cache->buckets[bucket].mnems;
    sc_stack_push(mnems, &rec->mnem_node_);
}

static void
emplace_accnt(struct FigCache* cache, struct ScSlNode* first, size_t size)
{
    assert(!cache->first_accnt);
    cache->first_accnt = first;
    cache->accnt_size = size;
    for (struct ScSlNode* node = first; node; node = node->next) {
        struct ScRec* rec = sc_shared_rec_entry(node);
        insert_id(cache, rec);
        insert_mnem(cache, rec);
    }
}

static void
emplace_contr(struct FigCache* cache, struct ScSlNode* first, size_t size)
{
    assert(!cache->first_contr);
    cache->first_contr = first;
    cache->contr_size = size;
    for (struct ScSlNode* node = first; node; node = node->next) {
        struct ScRec* rec = sc_shared_rec_entry(node);
        insert_id(cache, rec);
        insert_mnem(cache, rec);
    }
}

SC_EXTERN void
fig_cache_init(struct FigCache* cache, void (*term_state)(struct ScRec*), ScPool pool)
{
    cache->term_state = term_state ? term_state : term_state_noop;
    cache->pool = pool;
    cache->first_accnt = NULL;
    cache->accnt_size = 0;
    cache->first_contr = NULL;
    cache->contr_size = 0;
    // Zero buckets.
    memset(cache->buckets, 0, sizeof(cache->buckets));
}

SC_EXTERN void
fig_cache_term(struct FigCache* cache)
{
    free_rec_list(cache->first_contr, cache->term_state, cache->pool);
    free_rec_list(cache->first_accnt, cache->term_state, cache->pool);
}

SC_EXTERN void
fig_cache_reset(struct FigCache* cache)
{
    void (*term_state)(struct ScRec*) = cache->term_state;
    ScPool pool = cache->pool;
    fig_cache_term(cache);
    fig_cache_init(cache, term_state, pool);
}

SC_EXTERN void
fig_cache_emplace_rec_list(struct FigCache* cache, int type, struct ScSlNode* first, size_t size)
{
    switch (type) {
    case SC_ENTITY_ACCNT:
        emplace_accnt(cache, first, size);
        break;
    case SC_ENTITY_CONTR:
        emplace_contr(cache, first, size);
        break;
    default:
        assert(SC_FALSE);
        break;
    }
}

SC_EXTERN struct ScSlNode*
fig_cache_find_rec_id(const struct FigCache* cache, int type, ScIden id)
{
    const size_t bucket = hash_id(type, id) % FIG_CACHE_BUCKETS;
    for (struct ScSlNode* node = sc_stack_first(&cache->buckets[bucket].ids);
         node; node = node->next) {
        struct ScRec* rec = rec_entry_id(node);
        if (equal_id(rec, type, id))
            return &rec->shared_node_;
    }
    return NULL;
}

SC_EXTERN struct ScSlNode*
fig_cache_find_rec_mnem(const struct FigCache* cache, int type, const char* mnem)
{
    assert(mnem);
    const size_t bucket = hash_mnem(type, mnem) % FIG_CACHE_BUCKETS;
    for (struct ScSlNode* node = sc_stack_first(&cache->buckets[bucket].mnems);
         node; node = node->next) {
        struct ScRec* rec = rec_entry_mnem(node);
        if (equal_mnem(rec, type, mnem))
            return &rec->shared_node_;
    }
    return NULL;
}

SC_EXTERN struct ScSlNode*
fig_cache_first_rec(struct FigCache* cache, int type, size_t* size)
{
    struct ScSlNode* first;
    switch (type) {
    case SC_ENTITY_ACCNT:
        first = cache->first_accnt;
        if (size)
            *size = cache->accnt_size;
        break;
    case SC_ENTITY_CONTR:
        first = cache->first_contr;
        if (size)
            *size = cache->contr_size;
        break;
    default:
        abort();
    }
    return first;
}

SC_EXTERN ScBool
fig_cache_empty_rec(struct FigCache* cache, int type)
{
    struct ScSlNode* first;
    switch (type) {
    case SC_ENTITY_ACCNT:
        first = cache->first_accnt;
        break;
    case SC_ENTITY_CONTR:
        first = cache->first_contr;
        break;
    default:
        abort();
    }
    return first == FIG_CACHE_END_REC;
}
