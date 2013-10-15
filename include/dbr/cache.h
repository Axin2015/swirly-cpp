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
#ifndef DBR_CACHE_H
#define DBR_CACHE_H

#include <dbr/pool.h>

#include <sys/types.h> // ssize_t

struct DbrRec;

/**
 * @addtogroup Cache
 * @{
 */

typedef struct ElmCache* DbrCache;

#define DBR_CACHE_END_REC NULL

DBR_API DbrCache
dbr_cache_create(void (*term_state)(struct DbrRec*), DbrPool pool);

DBR_API void
dbr_cache_destroy(DbrCache cache);

DBR_API void
dbr_cache_emplace_recs(DbrCache cache, int type, struct DbrSlNode* first, size_t size);

DBR_API struct DbrSlNode*
dbr_cache_first_rec(DbrCache cache, int type, size_t* size);

DBR_API struct DbrSlNode*
dbr_cache_find_rec_id(DbrCache cache, int type, DbrIden id);

DBR_API struct DbrSlNode*
dbr_cache_find_rec_mnem(DbrCache cache, int type, const char* mnem);

/** @} */

#endif // DBR_CACHE_H
