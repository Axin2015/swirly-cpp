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
#ifndef FIG_CACHE_H
#define FIG_CACHE_H

// Record cache.

#include <dbr/stack.h>
#include <dbr/types.h>

#include <assert.h>

#include <sys/types.h> // ssize_t

#ifndef FIG_CACHE_BUCKETS
#define FIG_CACHE_BUCKETS 257
#endif // FIG_CACHE_BUCKETS

struct FigCache {
    void (*term_state)(struct DbrRec*);
    DbrPool pool;
    struct DbrSlNode* first_trader;
    size_t trader_size;
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
fig_cache_first_rec(struct FigCache* cache, int type, size_t* size);

DBR_EXTERN DbrBool
fig_cache_empty_rec(struct FigCache* cache, int type);

DBR_EXTERN struct DbrSlNode*
fig_cache_find_rec_id(const struct FigCache* cache, int type, DbrIden id);

DBR_EXTERN struct DbrSlNode*
fig_cache_find_rec_mnem(const struct FigCache* cache, int type, const char* mnem);

#endif // FIG_CACHE_H
