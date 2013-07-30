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
#ifndef DBR_POOL_H
#define DBR_POOL_H

#include <dbr/defs.h>

struct DbrSlNode;

/**
 * @addtogroup Pool
 * @{
 */

typedef struct ElmPool* DbrPool;

DBR_API DbrPool
dbr_pool_create(void);

DBR_API void
dbr_pool_destroy(DbrPool pool);

DBR_API struct DbrRec*
dbr_pool_alloc_rec(DbrPool pool);

DBR_API void
dbr_pool_free_rec(DbrPool pool, struct DbrRec* rec);

DBR_API struct DbrLevel*
dbr_pool_alloc_level(DbrPool pool, DbrKey key);

DBR_API void
dbr_pool_free_level(DbrPool pool, struct DbrLevel* level);

DBR_API struct DbrMatch*
dbr_pool_alloc_match(DbrPool pool);

DBR_API void
dbr_pool_free_match(DbrPool pool, struct DbrMatch* match);

DBR_API struct DbrOrder*
dbr_pool_alloc_order(DbrPool pool, DbrKey key);

DBR_API void
dbr_pool_free_order(DbrPool pool, struct DbrOrder* order);

DBR_API struct DbrMemb*
dbr_pool_alloc_memb(DbrPool pool, DbrKey key);

DBR_API void
dbr_pool_free_memb(DbrPool pool, struct DbrMemb* memb);

DBR_API struct DbrTrade*
dbr_pool_alloc_trade(DbrPool pool, DbrKey key);

DBR_API void
dbr_pool_free_trade(DbrPool pool, struct DbrTrade* trade);

DBR_API struct DbrPosn*
dbr_pool_alloc_posn(DbrPool pool, DbrKey key);

DBR_API void
dbr_pool_free_posn(DbrPool pool, struct DbrPosn* posn);

DBR_API struct DbrSub*
dbr_pool_alloc_sub(DbrPool pool, DbrKey key);

DBR_API void
dbr_pool_free_sub(DbrPool pool, struct DbrSub* sub);

/** @} */

DBR_API void
dbr_pool_free_list(DbrPool pool, int type, struct DbrSlNode* first);

#endif // DBR_POOL_H
