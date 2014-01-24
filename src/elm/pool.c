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
#include "pool.h"

#include <dbr/err.h>
#include <dbr/pool.h>

#include <errno.h>
#include <string.h> // strerror()
#include <stdlib.h> // malloc()

#include <sys/types.h>
#include <sys/mman.h>

DBR_EXTERN DbrBool
elm_pool_init(struct ElmPool* pool, size_t capacity)
{
    void* addr = mmap(NULL, capacity, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (!addr) {
        dbr_err_setf(DBR_ENOMEM, "mmap() failed: %s", strerror(errno));
        return DBR_FALSE;
    }

    pool->addr = addr;
    pool->used = 0;
    pool->capacity = capacity;
    pool->first_small = NULL;
    pool->first_large = NULL;
    pool->allocs = 0;
    pool->checksum = 0;

    dbr_log_debug2("%zu small nodes per block:", pool->small.nodes_per_block);
    dbr_log_debug2("sizeof DbrLevel=%zu", sizeof(struct DbrLevel));
    dbr_log_debug2("sizeof DbrMatch=%zu", sizeof(struct DbrMatch));
    dbr_log_debug2("sizeof DbrMemb=%zu", sizeof(struct DbrMemb));
    dbr_log_debug2("sizeof DbrPosn=%zu", sizeof(struct DbrPosn));
    dbr_log_debug2("sizeof DbrSess=%zu", sizeof(struct DbrSess));

    dbr_log_debug2("%zu large nodes per block:", pool->large.nodes_per_block);
    dbr_log_debug2("sizeof DbrRec=%zu", sizeof(struct DbrRec));
    dbr_log_debug2("sizeof DbrOrder=%zu", sizeof(struct DbrOrder));
    dbr_log_debug2("sizeof DbrExec=%zu", sizeof(struct DbrExec));
    dbr_log_debug2("sizeof DbrView=%zu", sizeof(struct DbrView));
    dbr_log_debug2("sizeof DbrBook=%zu", sizeof(struct DbrBook));

    return DBR_TRUE;
}

DBR_EXTERN void
elm_pool_term(struct ElmPool* pool)
{
    assert(pool);
    if (pool->allocs > 0)
        dbr_log_warn("%ld leaks detected", pool->allocs);
    else
        dbr_log_debug2("no leaks detected");
    assert(pool->allocs == 0 && pool->checksum == 0);
    munmap(pool->addr, pool->capacity);
}

DBR_EXTERN struct ElmSmallNode*
elm_pool_alloc_small(struct ElmPool* pool)
{
    struct ElmSmallNode* node;
    if (pool->first_small) {
        // Pop from free-list.
        node = pool->first_small;
        pool->first_small = node->next;
    } else {
        const size_t new_used = pool->used + sizeof(struct ElmSmallNode);
        if (new_used > pool->capacity) {
            dbr_err_set(DBR_ENOMEM, "out of memory");
            return NULL;
        }
        // Allocate node from virtual memory.
        node = pool->addr + pool->used;
        pool->used = new_used;
    }
    ++pool->allocs;
    pool->checksum ^= (unsigned long)node;
    return node;
}

DBR_EXTERN struct ElmLargeNode*
elm_pool_alloc_large(struct ElmPool* pool)
{
    struct ElmLargeNode* node;
    if (pool->first_large) {
        // Pop from free-list.
        node = pool->first_large;
        pool->first_large = node->next;
    } else {
        const size_t new_used = pool->used + sizeof(struct ElmLargeNode);
        if (new_used > pool->capacity) {
            dbr_err_set(DBR_ENOMEM, "out of memory");
            return NULL;
        }
        // Allocate node from virtual memory.
        node = pool->addr + pool->used;
        pool->used = new_used;
    }
    ++pool->allocs;
    pool->checksum ^= (unsigned long)node;
    return node;
}

DBR_EXTERN void
elm_pool_free_small(struct ElmPool* pool, struct ElmSmallNode* node)
{
    if (node) {

        // Push onto free-list.
        node->next = pool->first_small;
        pool->first_small = node;

        --pool->allocs;
        pool->checksum ^= (unsigned long)node;
        assert(pool->allocs > 0 || (pool->allocs == 0 && pool->checksum == 0));
    }
}

DBR_EXTERN void
elm_pool_free_large(struct ElmPool* pool, struct ElmLargeNode* node)
{
    if (node) {

        // Push onto free-list.
        node->next = pool->first_large;
        pool->first_large = node;

        --pool->allocs;
        pool->checksum ^= (unsigned long)node;
        assert(pool->allocs > 0 || (pool->allocs == 0 && pool->checksum == 0));
    }
}

DBR_API DbrPool
dbr_pool_create(size_t capacity)
{
    DbrPool pool = malloc(sizeof(struct ElmPool));
    if (dbr_unlikely(!pool)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    if (dbr_unlikely(!elm_pool_init(pool, capacity)))
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

DBR_API struct DbrOrder*
dbr_pool_alloc_order(DbrPool pool)
{
    return elm_pool_alloc_order(pool);
}

DBR_API void
dbr_pool_free_order(DbrPool pool, struct DbrOrder* order)
{
    elm_pool_free_order(pool, order);
}

DBR_API struct DbrLevel*
dbr_pool_alloc_level(DbrPool pool)
{
    return elm_pool_alloc_level(pool);
}

DBR_API void
dbr_pool_free_level(DbrPool pool, struct DbrLevel* level)
{
    elm_pool_free_level(pool, level);
}

DBR_API struct DbrExec*
dbr_pool_alloc_exec(DbrPool pool)
{
    return elm_pool_alloc_exec(pool);
}

DBR_API void
dbr_pool_free_exec(DbrPool pool, struct DbrExec* exec)
{
    elm_pool_free_exec(pool, exec);
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

DBR_API struct DbrMemb*
dbr_pool_alloc_memb(DbrPool pool)
{
    return elm_pool_alloc_memb(pool);
}

DBR_API void
dbr_pool_free_memb(DbrPool pool, struct DbrMemb* memb)
{
    elm_pool_free_memb(pool, memb);
}

DBR_API struct DbrPosn*
dbr_pool_alloc_posn(DbrPool pool)
{
    return elm_pool_alloc_posn(pool);
}

DBR_API void
dbr_pool_free_posn(DbrPool pool, struct DbrPosn* posn)
{
    elm_pool_free_posn(pool, posn);
}

DBR_API struct DbrView*
dbr_pool_alloc_view(DbrPool pool)
{
    return elm_pool_alloc_view(pool);
}

DBR_API void
dbr_pool_free_view(DbrPool pool, struct DbrView* view)
{
    elm_pool_free_view(pool, view);
}

DBR_API struct DbrBook*
dbr_pool_alloc_book(DbrPool pool)
{
    return elm_pool_alloc_book(pool);
}

DBR_API void
dbr_pool_free_book(DbrPool pool, struct DbrBook* book)
{
    elm_pool_free_book(pool, book);
}

DBR_API struct DbrSess*
dbr_pool_alloc_sess(DbrPool pool)
{
    return elm_pool_alloc_sess(pool);
}

DBR_API void
dbr_pool_free_sess(DbrPool pool, struct DbrSess* sess)
{
    elm_pool_free_sess(pool, sess);
}

DBR_API void
dbr_pool_free_entity_list(DbrPool pool, int type, struct DbrSlNode* first)
{
    struct DbrSlNode* node = first;
    switch (type) {
    case DBR_ENTITY_TRADER:
    case DBR_ENTITY_ACCNT:
    case DBR_ENTITY_CONTR:
        while (node) {
            struct DbrRec* rec = dbr_shared_rec_entry(node);
            node = node->next;
            dbr_pool_free_rec(pool, rec);
        }
        break;
    case DBR_ENTITY_ORDER:
        while (node) {
            struct DbrOrder* order = dbr_shared_order_entry(node);
            node = node->next;
            dbr_pool_free_order(pool, order);
        }
        break;
    case DBR_ENTITY_EXEC:
        while (node) {
            struct DbrExec* exec = dbr_shared_exec_entry(node);
            node = node->next;
            // Inline version of dbr_exec_decref() to avoid cycle.
            assert(exec->refs_ >= 1);
            if (--exec->refs_ == 0)
                dbr_pool_free_exec(pool, exec);
        }
        break;
    case DBR_ENTITY_MEMB:
        while (node) {
            struct DbrMemb* memb = dbr_shared_memb_entry(node);
            node = node->next;
            dbr_pool_free_memb(pool, memb);
        }
        break;
    case DBR_ENTITY_POSN:
        while (node) {
            struct DbrPosn* posn = dbr_shared_posn_entry(node);
            node = node->next;
            dbr_pool_free_posn(pool, posn);
        }
        break;
    default:
        abort();
    }
}
