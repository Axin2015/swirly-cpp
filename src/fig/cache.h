/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef FIG_CACHE_H
#define FIG_CACHE_H

// Record cache.

#include <dbr/elm/types.h>

#include <dbr/ash/stack.h>

#include <assert.h>

#include <sys/types.h> // ssize_t

#ifndef FIG_CACHE_BUCKETS
#define FIG_CACHE_BUCKETS 257
#endif // FIG_CACHE_BUCKETS

struct FigCache {
    void (*term_state)(struct DbrRec*);
    DbrPool pool;
    struct DbrSlNode* first_accnt;
    size_t accnt_size;
    struct DbrSlNode* first_contr;
    size_t contr_size;
    struct {
        struct DbrStack ids;
        struct DbrStack mnems;
    } buckets[FIG_CACHE_BUCKETS];
};

#define FIG_CACHE_END_REC NULL

DBR_EXTERN void
fig_cache_init(struct FigCache* cache, void (*term_state)(struct DbrRec*), DbrPool pool);

DBR_EXTERN void
fig_cache_term(struct FigCache* cache);

DBR_EXTERN void
fig_cache_reset(struct FigCache* cache);

// Transfer ownership to cache.

DBR_EXTERN void
fig_cache_emplace_rec_list(struct FigCache* cache, int type, struct DbrSlNode* first, size_t size);

DBR_EXTERN struct DbrSlNode*
fig_cache_find_rec_id(const struct FigCache* cache, int type, DbrIden id);

DBR_EXTERN struct DbrSlNode*
fig_cache_find_rec_mnem(const struct FigCache* cache, int type, const char* mnem);

DBR_EXTERN struct DbrSlNode*
fig_cache_first_rec(struct FigCache* cache, int type, size_t* size);

DBR_EXTERN DbrBool
fig_cache_empty_rec(struct FigCache* cache, int type);

#endif // FIG_CACHE_H
