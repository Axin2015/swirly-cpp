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
#ifndef ELM_CACHE_H
#define ELM_CACHE_H

// Record cache.

#include <dbr/defs.h>
#include <dbr/types.h>

#include <ash/stack.h>

#include <assert.h>

#include <sys/types.h> // ssize_t

#ifndef ELM_CACHE_BUCKETS
#define ELM_CACHE_BUCKETS 257
#endif // ELM_CACHE_BUCKETS

struct ElmCache {
    struct ElmPool* pool;
    // Must be set before first_market.
    struct DbrSlNode* first_instr;
    size_t instr_size;
    struct DbrSlNode* first_market;
    size_t market_size;
    // Must be set before first_trader.
    struct DbrSlNode* first_accnt;
    size_t accnt_size;
    struct DbrSlNode* first_trader;
    size_t trader_size;
    struct {
        struct AshStack ids;
        struct AshStack mnems;
    } buckets[ELM_CACHE_BUCKETS];
};

DBR_EXTERN void
elm_cache_init(struct ElmCache* cache, struct ElmPool* pool);

DBR_EXTERN void
elm_cache_term(struct ElmCache* cache);

// Transfer ownership to cache.

DBR_EXTERN void
elm_cache_emplace(struct ElmCache* cache, int type, struct DbrSlNode* first, size_t size);

DBR_EXTERN struct DbrSlNode*
elm_cache_entries(struct ElmCache* cache, int type, size_t* size);

DBR_EXTERN struct DbrSlNode*
elm_cache_find_id(const struct ElmCache* cache, int type, DbrIden id);

DBR_EXTERN struct DbrSlNode*
elm_cache_find_mnem(const struct ElmCache* cache, int type, const char* mnem);

static inline struct DbrSlNode*
elm_cache_end(const struct ElmCache* cache)
{
    return NULL;
}

#endif // ELM_CACHE_H
