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
#ifndef DBR_ASH_POOL_H
#define DBR_ASH_POOL_H

/**
 * @file
 * Memory pool.
 */

#include <dbr/elm/types.h>

struct DbrSlNode;

/**
 * @addtogroup Pool
 * @{
 */

DBR_API DbrPool
dbr_pool_create(size_t capacity);

DBR_API void
dbr_pool_destroy(DbrPool pool);

DBR_API struct DbrRec*
dbr_pool_alloc_rec(DbrPool pool);

DBR_API void
dbr_pool_free_rec(DbrPool pool, struct DbrRec* rec);

DBR_API struct DbrPerm*
dbr_pool_alloc_perm(DbrPool pool);

DBR_API void
dbr_pool_free_perm(DbrPool pool, struct DbrPerm* perm);

DBR_API struct DbrOrder*
dbr_pool_alloc_order(DbrPool pool);

DBR_API void
dbr_pool_free_order(DbrPool pool, struct DbrOrder* order);

DBR_API struct DbrLevel*
dbr_pool_alloc_level(DbrPool pool);

DBR_API void
dbr_pool_free_level(DbrPool pool, struct DbrLevel* level);

DBR_API struct DbrExec*
dbr_pool_alloc_exec(DbrPool pool);

DBR_API void
dbr_pool_free_exec(DbrPool pool, struct DbrExec* exec);

DBR_API struct DbrMatch*
dbr_pool_alloc_match(DbrPool pool);

DBR_API void
dbr_pool_free_match(DbrPool pool, struct DbrMatch* match);

DBR_API struct DbrPosn*
dbr_pool_alloc_posn(DbrPool pool);

DBR_API void
dbr_pool_free_posn(DbrPool pool, struct DbrPosn* posn);

DBR_API struct DbrView*
dbr_pool_alloc_view(DbrPool pool);

DBR_API void
dbr_pool_free_view(DbrPool pool, struct DbrView* view);

DBR_API struct DbrBook*
dbr_pool_alloc_book(DbrPool pool);

DBR_API void
dbr_pool_free_book(DbrPool pool, struct DbrBook* book);

DBR_API struct DbrSub*
dbr_pool_alloc_sub(DbrPool pool);

DBR_API void
dbr_pool_free_sub(DbrPool pool, struct DbrSub* sub);

DBR_API struct DbrSess*
dbr_pool_alloc_sess(DbrPool pool);

DBR_API void
dbr_pool_free_sess(DbrPool pool, struct DbrSess* sess);

DBR_API void
dbr_pool_free_entity_list(DbrPool pool, int type, struct DbrSlNode* first);

/** @} */

#endif // DBR_ASH_POOL_H
