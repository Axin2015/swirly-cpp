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

#include <errno.h>
#include <string.h> // strerror()
#include <stdlib.h> // malloc()

#include <unistd.h> // sysconf()

#include <sys/types.h>
#include <sys/mman.h>

#if DBR_DEBUG_ALLOC
static inline struct ElmSmallEntry*
small_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct ElmSmallEntry, pool_node_, node);
}

static inline struct ElmLargeEntry*
large_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct ElmLargeEntry, pool_node_, node);
}
#endif // DBR_DEBUG_ALLOC

static void*
alloc_mem(struct ElmPool* pool, size_t size)
{
    const size_t new_used = pool->used + size;
    if (new_used > pool->capacity) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return NULL;
    }
#if DBR_DEBUG_LEVEL >= 1
    const size_t page = new_used / pool->pagesize;
    if (pool->used == 0 || page > pool->used / pool->pagesize)
        dbr_log_info("allocating page %zu", page + 1);
#endif // DBR_DEBUG_LEVEL >= 1
    // Allocate entry from virtual memory.
    void* addr = pool->addr + pool->used;
    pool->used = new_used;
    return addr;
}

DBR_EXTERN DbrBool
elm_pool_init(struct ElmPool* pool, DbrIden seed, size_t capacity)
{
    void* addr = mmap(NULL, capacity, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (addr == MAP_FAILED) {
        dbr_err_setf(DBR_ENOMEM, "mmap() failed: %s", strerror(errno));
        return DBR_FALSE;
    }

    pool->id = seed;
    pool->addr = addr;
    pool->used = 0;
    pool->capacity = capacity;
    //pool->pagesize = sysconf(_SC_PAGESIZE);
    pool->pagesize = 1024;
    pool->first_small = NULL;
    pool->first_large = NULL;

#if DBR_DEBUG_ALLOC
    dbr_tree_init(&pool->allocs_small);
    dbr_tree_init(&pool->allocs_large);
#endif // DBR_DEBUG_ALLOC

    // Small entrys.
    dbr_log_debug1("sizeof DbrRbNode: %zu", sizeof(struct DbrRbNode));
    dbr_log_debug1("sizeof DbrLevel:  %zu", sizeof(struct DbrLevel));
    dbr_log_debug1("sizeof DbrMatch:  %zu", sizeof(struct DbrMatch));
    dbr_log_debug1("sizeof DbrMemb:   %zu", sizeof(struct DbrMemb));
    dbr_log_debug1("sizeof DbrSub:    %zu", sizeof(struct DbrSub));
    dbr_log_debug1("sizeof DbrSess:   %zu", sizeof(struct DbrSess));

    // Large entrys.
    dbr_log_debug1("sizeof DbrRec:    %zu", sizeof(struct DbrRec));
    dbr_log_debug1("sizeof DbrOrder:  %zu", sizeof(struct DbrOrder));
    dbr_log_debug1("sizeof DbrExec:   %zu", sizeof(struct DbrExec));
    dbr_log_debug1("sizeof DbrPosn:   %zu", sizeof(struct DbrPosn));
    dbr_log_debug1("sizeof DbrView:   %zu", sizeof(struct DbrView));
    dbr_log_debug1("sizeof DbrBook:   %zu", sizeof(struct DbrBook));

    return DBR_TRUE;
}

DBR_EXTERN void
elm_pool_term(struct ElmPool* pool)
{
    assert(pool);
    munmap(pool->addr, pool->capacity);
}

DBR_EXTERN struct ElmSmallEntry*
#if !DBR_DEBUG_ALLOC
elm_pool_alloc_small(struct ElmPool* pool)
#else  // DBR_DEBUG_ALLOC
elm_pool_alloc_small(struct ElmPool* pool, const char* file, int line)
#endif // DBR_DEBUG_ALLOC
{
    struct ElmSmallEntry* entry;
    if (pool->first_small) {
        // Pop from free-list.
        entry = pool->first_small;
        pool->first_small = entry->next;
    } else if (!(entry = alloc_mem(pool, sizeof(struct ElmSmallEntry))))
        return NULL;

#if DBR_DEBUG_ALLOC
    entry->file = file;
    entry->line = line;
    dbr_tree_insert(&pool->allocs_small, (DbrKey)entry, &entry->pool_node_);
#endif // DBR_DEBUG_ALLOC

    return entry;
}

DBR_EXTERN struct ElmLargeEntry*
#if !DBR_DEBUG_ALLOC
elm_pool_alloc_large(struct ElmPool* pool)
#else  // DBR_DEBUG_ALLOC
elm_pool_alloc_large(struct ElmPool* pool, const char* file, int line)
#endif // DBR_DEBUG_ALLOC
{
    struct ElmLargeEntry* entry;
    if (pool->first_large) {
        // Pop from free-list.
        entry = pool->first_large;
        pool->first_large = entry->next;
    } else if (!(entry = alloc_mem(pool, sizeof(struct ElmLargeEntry))))
        return NULL;

#if DBR_DEBUG_ALLOC
    entry->file = file;
    entry->line = line;
    dbr_tree_insert(&pool->allocs_large, (DbrKey)entry, &entry->pool_node_);
#endif // DBR_DEBUG_ALLOC

    return entry;
}

DBR_EXTERN void
elm_pool_free_small(struct ElmPool* pool, struct ElmSmallEntry* entry)
{
    if (entry) {
#if DBR_DEBUG_ALLOC
        dbr_tree_remove(&pool->allocs_small, &entry->pool_node_);
#endif // DBR_DEBUG_ALLOC
        // Push onto free-list.
        entry->next = pool->first_small;
        pool->first_small = entry;
    }
}

DBR_EXTERN void
elm_pool_free_large(struct ElmPool* pool, struct ElmLargeEntry* entry)
{
    if (entry) {
#if DBR_DEBUG_ALLOC
        dbr_tree_remove(&pool->allocs_large, &entry->pool_node_);
#endif // DBR_DEBUG_ALLOC
        // Push onto free-list.
        entry->next = pool->first_large;
        pool->first_large = entry;
    }
}

DBR_EXTERN void
elm_pool_free_entity_list(struct ElmPool* pool, int type, struct DbrSlNode* first)
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

DBR_API DbrPool
dbr_pool_create(DbrIden seed, size_t capacity)
{
    DbrPool pool = malloc(sizeof(struct ElmPool));
    if (dbr_unlikely(!pool)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    if (dbr_unlikely(!elm_pool_init(pool, seed, capacity)))
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
#if DBR_DEBUG_ALLOC
        for (struct DbrRbNode* node = dbr_tree_first(&pool->allocs_small);
             node != DBR_TREE_END; node = dbr_rbnode_next(node)) {
            struct ElmSmallEntry* small = small_entry(node);
            dbr_log_warn("memory leak in %s at %d", small->file, small->line);
        }
        for (struct DbrRbNode* node = dbr_tree_first(&pool->allocs_large);
             node != DBR_TREE_END; node = dbr_rbnode_next(node)) {
            struct ElmLargeEntry* large = large_entry(node);
            dbr_log_warn("memory leak in %s at %d", large->file, large->line);
        }
#endif // DBR_DEBUG_ALLOC
        elm_pool_term(pool);
        free(pool);
    }
}

DBR_API DbrIden
dbr_pool_alloc_id(DbrPool pool)
{
    return elm_pool_alloc_id(pool);
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

DBR_API struct DbrSub*
dbr_pool_alloc_sub(DbrPool pool)
{
    return elm_pool_alloc_sub(pool);
}

DBR_API void
dbr_pool_free_sub(DbrPool pool, struct DbrSub* sub)
{
    elm_pool_free_sub(pool, sub);
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
    elm_pool_free_entity_list(pool, type, first);
}
