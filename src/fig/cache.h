/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef FIG_CACHE_H
#define FIG_CACHE_H

// Record cache.

#include <sc/elm/types.h>

#include <sc/ash/stack.h>

#include <assert.h>

#include <sys/types.h> // ssize_t

#ifndef FIG_CACHE_BUCKETS
#define FIG_CACHE_BUCKETS 257
#endif // FIG_CACHE_BUCKETS

struct FigCache {
    void (*term_state)(struct ScRec*);
    ScPool pool;
    struct ScSlNode* first_accnt;
    size_t accnt_size;
    struct ScSlNode* first_contr;
    size_t contr_size;
    struct {
        struct ScStack ids;
        struct ScStack mnems;
    } buckets[FIG_CACHE_BUCKETS];
};

#define FIG_CACHE_END_REC NULL

SC_EXTERN void
fig_cache_init(struct FigCache* cache, void (*term_state)(struct ScRec*), ScPool pool);

SC_EXTERN void
fig_cache_term(struct FigCache* cache);

SC_EXTERN void
fig_cache_reset(struct FigCache* cache);

// Transfer ownership to cache.

SC_EXTERN void
fig_cache_emplace_rec_list(struct FigCache* cache, int type, struct ScSlNode* first, size_t size);

SC_EXTERN struct ScSlNode*
fig_cache_find_rec_id(const struct FigCache* cache, int type, ScIden id);

SC_EXTERN struct ScSlNode*
fig_cache_find_rec_mnem(const struct FigCache* cache, int type, const char* mnem);

SC_EXTERN struct ScSlNode*
fig_cache_first_rec(struct FigCache* cache, int type, size_t* size);

SC_EXTERN ScBool
fig_cache_empty_rec(struct FigCache* cache, int type);

#endif // FIG_CACHE_H
