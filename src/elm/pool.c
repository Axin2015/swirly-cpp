/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "pool.h"

#include <sc/ash/err.h>

#include <errno.h>
#include <string.h> // strerror()
#include <stdlib.h> // malloc()

#include <unistd.h> // sysconf()

#include <sys/types.h>
#include <sys/mman.h>

#if SC_DEBUG_ALLOC
static inline struct ElmSmallEntry*
small_entry(struct ScRbNode* node)
{
    return sc_implof(struct ElmSmallEntry, pool_node_, node);
}

static inline struct ElmLargeEntry*
large_entry(struct ScRbNode* node)
{
    return sc_implof(struct ElmLargeEntry, pool_node_, node);
}
#endif // SC_DEBUG_ALLOC

static void*
alloc_mem(struct ElmPool* pool, size_t size)
{
    const size_t new_used = pool->used + size;
    if (new_used > pool->capacity) {
        sc_err_set(SC_ENOMEM, "out of memory");
        return NULL;
    }
#if SC_DEBUG_LEVEL >= 1
    const size_t page = new_used / pool->pagesize;
    if (pool->used == 0 || page > pool->used / pool->pagesize)
        sc_log_debug1("allocating page %zu", page + 1);
#endif // SC_DEBUG_LEVEL >= 1
    // Allocate entry from virtual memory.
    void* addr = pool->addr + pool->used;
    pool->used = new_used;
    return addr;
}

SC_EXTERN ScBool
elm_pool_init(struct ElmPool* pool, size_t capacity)
{
    void* addr = mmap(NULL, capacity, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (addr == MAP_FAILED) {
        sc_err_setf(SC_ENOMEM, "mmap() failed: %s", strerror(errno));
        return SC_FALSE;
    }

    pool->addr = addr;
    pool->used = 0;
    pool->capacity = capacity;
    //pool->pagesize = sysconf(_SC_PAGESIZE);
    pool->pagesize = 1024;
    pool->first_small = NULL;
    pool->first_large = NULL;

#if SC_DEBUG_ALLOC
    sc_tree_init(&pool->allocs_small);
    sc_tree_init(&pool->allocs_large);
#endif // SC_DEBUG_ALLOC

    // Small entrys.
    sc_log_debug1("small entries:");
    sc_log_debug1("sizeof ScRbNode: %zu", sizeof(struct ScRbNode));
    sc_log_debug1("sizeof ScPerm:   %zu", sizeof(struct ScPerm));
    sc_log_debug1("sizeof ScLevel:  %zu", sizeof(struct ScLevel));
    sc_log_debug1("sizeof ScMatch:  %zu", sizeof(struct ScMatch));
    sc_log_debug1("sizeof ScSub:    %zu", sizeof(struct ScSub));
    sc_log_debug1("sizeof ScSess:   %zu", sizeof(struct ScSess));

    // Large entrys.
    sc_log_debug1("large entries:");
    sc_log_debug1("sizeof ScRec:    %zu", sizeof(struct ScRec));
    sc_log_debug1("sizeof ScOrder:  %zu", sizeof(struct ScOrder));
    sc_log_debug1("sizeof ScExec:   %zu", sizeof(struct ScExec));
    sc_log_debug1("sizeof ScPosn:   %zu", sizeof(struct ScPosn));
    sc_log_debug1("sizeof ScView:   %zu", sizeof(struct ScView));
    sc_log_debug1("sizeof ScBook:   %zu", sizeof(struct ScBook));

    return SC_TRUE;
}

SC_EXTERN void
elm_pool_term(struct ElmPool* pool)
{
    assert(pool);
    munmap(pool->addr, pool->capacity);
}

SC_EXTERN struct ElmSmallEntry*
#if !SC_DEBUG_ALLOC
elm_pool_alloc_small(struct ElmPool* pool)
#else  // SC_DEBUG_ALLOC
elm_pool_alloc_small(struct ElmPool* pool, const char* file, int line)
#endif // SC_DEBUG_ALLOC
{
    struct ElmSmallEntry* entry;
    if (pool->first_small) {
        // Pop from free-list.
        entry = pool->first_small;
        pool->first_small = entry->next;
    } else if (!(entry = alloc_mem(pool, sizeof(struct ElmSmallEntry))))
        return NULL;

#if SC_DEBUG_ALLOC
    entry->file = file;
    entry->line = line;
    sc_tree_insert(&pool->allocs_small, (ScKey)entry, &entry->pool_node_);
#endif // SC_DEBUG_ALLOC

    return entry;
}

SC_EXTERN struct ElmLargeEntry*
#if !SC_DEBUG_ALLOC
elm_pool_alloc_large(struct ElmPool* pool)
#else  // SC_DEBUG_ALLOC
elm_pool_alloc_large(struct ElmPool* pool, const char* file, int line)
#endif // SC_DEBUG_ALLOC
{
    struct ElmLargeEntry* entry;
    if (pool->first_large) {
        // Pop from free-list.
        entry = pool->first_large;
        pool->first_large = entry->next;
    } else if (!(entry = alloc_mem(pool, sizeof(struct ElmLargeEntry))))
        return NULL;

#if SC_DEBUG_ALLOC
    entry->file = file;
    entry->line = line;
    sc_tree_insert(&pool->allocs_large, (ScKey)entry, &entry->pool_node_);
#endif // SC_DEBUG_ALLOC

    return entry;
}

SC_EXTERN void
elm_pool_free_small(struct ElmPool* pool, struct ElmSmallEntry* entry)
{
    if (entry) {
#if SC_DEBUG_ALLOC
        sc_tree_remove(&pool->allocs_small, &entry->pool_node_);
#endif // SC_DEBUG_ALLOC
        // Push onto free-list.
        entry->next = pool->first_small;
        pool->first_small = entry;
    }
}

SC_EXTERN void
elm_pool_free_large(struct ElmPool* pool, struct ElmLargeEntry* entry)
{
    if (entry) {
#if SC_DEBUG_ALLOC
        sc_tree_remove(&pool->allocs_large, &entry->pool_node_);
#endif // SC_DEBUG_ALLOC
        // Push onto free-list.
        entry->next = pool->first_large;
        pool->first_large = entry;
    }
}

SC_EXTERN void
elm_pool_free_entity_list(struct ElmPool* pool, int type, struct ScSlNode* first)
{
    struct ScSlNode* node = first;
    switch (type) {
    case SC_ENTITY_ACCNT:
    case SC_ENTITY_CONTR:
        while (node) {
            struct ScRec* rec = sc_shared_rec_entry(node);
            node = node->next;
            sc_pool_free_rec(pool, rec);
        }
        break;
    case SC_ENTITY_PERM:
        while (node) {
            struct ScPerm* perm = sc_shared_perm_entry(node);
            node = node->next;
            sc_pool_free_perm(pool, perm);
        }
        break;
    case SC_ENTITY_ORDER:
        while (node) {
            struct ScOrder* order = sc_shared_order_entry(node);
            node = node->next;
            sc_pool_free_order(pool, order);
        }
        break;
    case SC_ENTITY_EXEC:
        while (node) {
            struct ScExec* exec = sc_shared_exec_entry(node);
            node = node->next;
            // Inline version of sc_exec_decref() to avoid cyclic dependency.
            assert(exec->refs_ >= 1);
            if (--exec->refs_ == 0)
                sc_pool_free_exec(pool, exec);
        }
        break;
    case SC_ENTITY_POSN:
        while (node) {
            struct ScPosn* posn = sc_shared_posn_entry(node);
            node = node->next;
            sc_pool_free_posn(pool, posn);
        }
        break;
    default:
        abort();
    }
}

SC_API ScPool
sc_pool_create(size_t capacity)
{
    ScPool pool = malloc(sizeof(struct ElmPool));
    if (sc_unlikely(!pool)) {
        sc_err_set(SC_ENOMEM, "out of memory");
        goto fail1;
    }

    if (sc_unlikely(!elm_pool_init(pool, capacity)))
        goto fail2;

    return pool;
 fail2:
    free(pool);
 fail1:
    return NULL;
}

SC_API void
sc_pool_destroy(ScPool pool)
{
    if (pool) {
#if SC_DEBUG_ALLOC
        for (struct ScRbNode* node = sc_tree_first(&pool->allocs_small);
             node != SC_TREE_END; node = sc_rbnode_next(node)) {
            struct ElmSmallEntry* small = small_entry(node);
            sc_log_warn("memory leak in %s at %d", small->file, small->line);
        }
        for (struct ScRbNode* node = sc_tree_first(&pool->allocs_large);
             node != SC_TREE_END; node = sc_rbnode_next(node)) {
            struct ElmLargeEntry* large = large_entry(node);
            sc_log_warn("memory leak in %s at %d", large->file, large->line);
        }
#endif // SC_DEBUG_ALLOC
        elm_pool_term(pool);
        free(pool);
    }
}

SC_API struct ScRec*
sc_pool_alloc_rec(ScPool pool)
{
    return elm_pool_alloc_rec(pool);
}

SC_API void
sc_pool_free_rec(ScPool pool, struct ScRec* rec)
{
    elm_pool_free_rec(pool, rec);
}

SC_API struct ScPerm*
sc_pool_alloc_perm(ScPool pool)
{
    return elm_pool_alloc_perm(pool);
}

SC_API void
sc_pool_free_perm(ScPool pool, struct ScPerm* perm)
{
    elm_pool_free_perm(pool, perm);
}

SC_API struct ScOrder*
sc_pool_alloc_order(ScPool pool)
{
    return elm_pool_alloc_order(pool);
}

SC_API void
sc_pool_free_order(ScPool pool, struct ScOrder* order)
{
    elm_pool_free_order(pool, order);
}

SC_API struct ScLevel*
sc_pool_alloc_level(ScPool pool)
{
    return elm_pool_alloc_level(pool);
}

SC_API void
sc_pool_free_level(ScPool pool, struct ScLevel* level)
{
    elm_pool_free_level(pool, level);
}

SC_API struct ScExec*
sc_pool_alloc_exec(ScPool pool)
{
    return elm_pool_alloc_exec(pool);
}

SC_API void
sc_pool_free_exec(ScPool pool, struct ScExec* exec)
{
    elm_pool_free_exec(pool, exec);
}

SC_API struct ScMatch*
sc_pool_alloc_match(ScPool pool)
{
    return elm_pool_alloc_match(pool);
}

SC_API void
sc_pool_free_match(ScPool pool, struct ScMatch* match)
{
    elm_pool_free_match(pool, match);
}

SC_API struct ScPosn*
sc_pool_alloc_posn(ScPool pool)
{
    return elm_pool_alloc_posn(pool);
}

SC_API void
sc_pool_free_posn(ScPool pool, struct ScPosn* posn)
{
    elm_pool_free_posn(pool, posn);
}

SC_API struct ScView*
sc_pool_alloc_view(ScPool pool)
{
    return elm_pool_alloc_view(pool);
}

SC_API void
sc_pool_free_view(ScPool pool, struct ScView* view)
{
    elm_pool_free_view(pool, view);
}

SC_API struct ScBook*
sc_pool_alloc_book(ScPool pool)
{
    return elm_pool_alloc_book(pool);
}

SC_API void
sc_pool_free_book(ScPool pool, struct ScBook* book)
{
    elm_pool_free_book(pool, book);
}

SC_API struct ScSub*
sc_pool_alloc_sub(ScPool pool)
{
    return elm_pool_alloc_sub(pool);
}

SC_API void
sc_pool_free_sub(ScPool pool, struct ScSub* sub)
{
    elm_pool_free_sub(pool, sub);
}

SC_API struct ScSess*
sc_pool_alloc_sess(ScPool pool)
{
    return elm_pool_alloc_sess(pool);
}

SC_API void
sc_pool_free_sess(ScPool pool, struct ScSess* sess)
{
    elm_pool_free_sess(pool, sess);
}

SC_API void
sc_pool_free_entity_list(ScPool pool, int type, struct ScSlNode* first)
{
    elm_pool_free_entity_list(pool, type, first);
}
