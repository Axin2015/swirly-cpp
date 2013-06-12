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
#include <dbr/pool.h>

#include <elm/pool.h>

#include <stdlib.h> // malloc()

DBR_API DbrPool
dbr_pool_create(void)
{
    DbrPool pool = malloc(sizeof(struct ElmPool));
    if (dbr_unlikely(!pool))
        goto fail1;

    if (dbr_unlikely(!elm_pool_init(pool)))
        goto fail2;

    return pool;
 fail2:
    free(pool);
 fail1:
    return NULL;
}

DBR_API void
dbr_pool_destroy(DbrPool pool)
{
    if (pool) {
        elm_pool_term(pool);
        free(pool);
    }
}

DBR_API struct DbrRec*
dbr_pool_alloc_rec(DbrPool pool)
{
    return elm_pool_alloc_rec(pool);
}

DBR_API void
dbr_pool_free_rec(DbrPool pool, struct DbrRec* rec)
{
    elm_pool_free_rec(pool, rec);
}

DBR_API struct DbrLevel*
dbr_pool_alloc_level(DbrPool pool, DbrKey key)
{
    return elm_pool_alloc_level(pool, key);
}

DBR_API void
dbr_pool_free_level(DbrPool pool, struct DbrLevel* level)
{
    elm_pool_free_level(pool, level);
}

DBR_API struct DbrMatch*
dbr_pool_alloc_match(DbrPool pool)
{
    return elm_pool_alloc_match(pool);
}

DBR_API void
dbr_pool_free_match(DbrPool pool, struct DbrMatch* match)
{
    elm_pool_free_match(pool, match);
}

DBR_API struct DbrOrder*
dbr_pool_alloc_order(DbrPool pool, DbrKey key)
{
    return elm_pool_alloc_order(pool, key);
}

DBR_API void
dbr_pool_free_order(DbrPool pool, struct DbrOrder* order)
{
    elm_pool_free_order(pool, order);
}

DBR_API struct DbrMemb*
dbr_pool_alloc_memb(DbrPool pool, DbrKey key)
{
    return elm_pool_alloc_memb(pool, key);
}

DBR_API void
dbr_pool_free_memb(DbrPool pool, struct DbrMemb* memb)
{
    elm_pool_free_memb(pool, memb);
}

DBR_API struct DbrTrade*
dbr_pool_alloc_trade(DbrPool pool, DbrKey key)
{
    return elm_pool_alloc_trade(pool, key);
}

DBR_API void
dbr_pool_free_trade(DbrPool pool, struct DbrTrade* trade)
{
    elm_pool_free_trade(pool, trade);
}

DBR_API struct DbrPosn*
dbr_pool_alloc_posn(DbrPool pool, DbrKey key)
{
    return elm_pool_alloc_posn(pool, key);
}

DBR_API void
dbr_pool_free_posn(DbrPool pool, struct DbrPosn* posn)
{
    elm_pool_free_posn(pool, posn);
}

DBR_API struct DbrSub*
dbr_pool_alloc_sub(DbrPool pool, DbrKey key)
{
    return elm_pool_alloc_sub(pool, key);
}

DBR_API void
dbr_pool_free_sub(DbrPool pool, struct DbrSub* sub)
{
    elm_pool_free_sub(pool, sub);
}
