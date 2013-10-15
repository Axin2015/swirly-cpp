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
#include "pool.h"

#include <dbr/err.h>
#include <dbr/pool.h>

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static DbrBool
alloc_small_nodes(struct ElmPool* pool)
{
    struct ElmSmallBlock* block = malloc(sizeof(struct ElmSmallBlock)
                                         + pool->small.nodes_per_block
                                         * sizeof(struct ElmSmallNode));
    if (dbr_unlikely(!block)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return false;
    }

    // Push block.
    block->next = pool->small.first_block;
    pool->small.first_block = block;

    // Link chain of new nodes.
    const int last = pool->small.nodes_per_block - 1;
    for (int i = 0; i < last; ++i) {
        block->nodes[i].next = &block->nodes[i + 1];
#if defined(DBR_DEBUG_ALLOC)
        block->nodes[i].file = NULL;
        block->nodes[i].line = 0;
#endif // defined(DBR_DEBUG_ALLOC)
    }

    // Last node links to any existing nodes.
    block->nodes[last].next = pool->small.first_node;
#if defined(DBR_DEBUG_ALLOC)
    block->nodes[last].file = NULL;
    block->nodes[last].line = 0;
#endif // defined(DBR_DEBUG_ALLOC)

    // Newly allocated nodes are now at the front.
    pool->small.first_node = &block->nodes[0];

    return true;
}

static DbrBool
alloc_large_nodes(struct ElmPool* pool)
{
    struct ElmLargeBlock* block = malloc(sizeof(struct ElmLargeBlock)
                                         + pool->large.nodes_per_block
                                         * sizeof(struct ElmLargeNode));
    if (dbr_unlikely(!block)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return false;
    }

    // Push block.
    block->next = pool->large.first_block;
    pool->large.first_block = block;

    // Link chain of new nodes.
    const int last = pool->large.nodes_per_block - 1;
    for (int i = 0; i < last; ++i) {
        block->nodes[i].next = &block->nodes[i + 1];
#if defined(DBR_DEBUG_ALLOC)
        block->nodes[i].file = NULL;
        block->nodes[i].line = 0;
#endif // defined(DBR_DEBUG_ALLOC)
    }

    // Last node links to any existing nodes.
    block->nodes[last].next = pool->large.first_node;
#if defined(DBR_DEBUG_ALLOC)
    block->nodes[last].file = NULL;
    block->nodes[last].line = 0;
#endif // defined(DBR_DEBUG_ALLOC)

    // Newly allocated nodes are now at the front.
    pool->large.first_node = &block->nodes[0];

    return true;
}

DBR_EXTERN DbrBool
elm_pool_init(struct ElmPool* pool)
{
    // Slightly less than one page of items.
    // ((page_size - header_size) / item_size) - 1

    const long page_size = sysconf(_SC_PAGESIZE);

    // Small.
    pool->small.nodes_per_block = (page_size - sizeof(struct ElmSmallBlock))
        / sizeof(struct ElmSmallNode) - 1;
    pool->small.first_block = NULL;
    pool->small.first_node = NULL;

    // Large.
    pool->large.nodes_per_block = (page_size - sizeof(struct ElmLargeBlock))
        / sizeof(struct ElmLargeNode) - 1;
    pool->large.first_block = NULL;
    pool->large.first_node = NULL;

#if defined(DBR_DEBUG_ALLOC)
    pool->allocs = 0;
    pool->checksum = 0;

    dbr_log_debug2("%zu small nodes per block:", pool->small.nodes_per_block);
    dbr_log_debug2("sizeof DbrLevel=%zu", sizeof(struct DbrLevel));
    dbr_log_debug2("sizeof DbrMatch=%zu", sizeof(struct DbrMatch));
    dbr_log_debug2("sizeof DbrMemb=%zu", sizeof(struct DbrMemb));
    dbr_log_debug2("sizeof DbrPosn=%zu", sizeof(struct DbrPosn));

    dbr_log_debug2("%zu large nodes per block:", pool->large.nodes_per_block);
    dbr_log_debug2("sizeof DbrRec=%zu", sizeof(struct DbrRec));
    dbr_log_debug2("sizeof DbrOrder=%zu", sizeof(struct DbrOrder));
    dbr_log_debug2("sizeof DbrTrade=%zu", sizeof(struct DbrTrade));
    dbr_log_debug2("sizeof DbrStmt=%zu", sizeof(struct DbrStmt));
#endif // defined(DBR_DEBUG_ALLOC)
    if (!alloc_small_nodes(pool))
        goto fail1;

    if (!alloc_large_nodes(pool))
        goto fail2;

    return true;
 fail2:
    // Defensively does not assume single block.
    while (pool->small.first_block) {
        struct ElmSmallBlock* block = pool->small.first_block;
        pool->small.first_block = block->next;
        free(block);
    }
 fail1:
    return false;
}

DBR_EXTERN void
elm_pool_term(struct ElmPool* pool)
{
    assert(pool);

    // Large.
    while (pool->large.first_block) {
        struct ElmLargeBlock* block = pool->large.first_block;
#if defined(DBR_DEBUG_ALLOC)
        for (int i = 0; i < pool->large.nodes_per_block; ++i) {
            if (block->nodes[i].file) {
                dbr_log_warn("allocation in %s at %d",
                             block->nodes[i].file, block->nodes[i].line);
            }
        }
#endif // defined(DBR_DEBUG_ALLOC)
        pool->large.first_block = block->next;
        free(block);
    }
    pool->large.first_node = NULL;

    // Small.
    while (pool->small.first_block) {
        struct ElmSmallBlock* block = pool->small.first_block;
#if defined(DBR_DEBUG_ALLOC)
        for (int i = 0; i < pool->small.nodes_per_block; ++i) {
            if (block->nodes[i].file) {
                dbr_log_warn("allocation in %s at %d",
                             block->nodes[i].file, block->nodes[i].line);
            }
        }
#endif // defined(DBR_DEBUG_ALLOC)
        pool->small.first_block = block->next;
        free(block);
    }
    pool->small.first_node = NULL;

#if defined(DBR_DEBUG_ALLOC)
    if (pool->allocs > 0)
        dbr_log_warn("%ld leaks detected", pool->allocs);
    else
        dbr_log_debug2("no leaks detected");
    assert(pool->allocs == 0 && pool->checksum == 0);
#endif // defined(DBR_DEBUG_ALLOC)
}

DBR_EXTERN struct ElmSmallNode*
#if !defined(DBR_DEBUG_ALLOC)
elm_pool_alloc_small(struct ElmPool* pool)
#else  // defined(DBR_DEBUG_ALLOC)
elm_pool_alloc_small(struct ElmPool* pool, const char* file, int line)
#endif // defined(DBR_DEBUG_ALLOC)
{
    if (dbr_unlikely(!pool->small.first_node && !alloc_small_nodes(pool))) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return false;
    }
    struct ElmSmallNode* node = pool->small.first_node;
    pool->small.first_node = node->next;

#if defined(DBR_DEBUG_ALLOC)
    node->file = file;
    node->line = line;

    ++pool->allocs;
    pool->checksum ^= (unsigned long)node;
#endif // defined(DBR_DEBUG_ALLOC)
    return node;
}

DBR_EXTERN struct ElmLargeNode*
#if !defined(DBR_DEBUG_ALLOC)
elm_pool_alloc_large(struct ElmPool* pool)
#else  // defined(DBR_DEBUG_ALLOC)
elm_pool_alloc_large(struct ElmPool* pool, const char* file, int line)
#endif // defined(DBR_DEBUG_ALLOC)
{
    if (dbr_unlikely(!pool->large.first_node && !alloc_large_nodes(pool))) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return false;
    }
    struct ElmLargeNode* node = pool->large.first_node;
    pool->large.first_node = node->next;

#if defined(DBR_DEBUG_ALLOC)
    node->file = file;
    node->line = line;

    ++pool->allocs;
    pool->checksum ^= (unsigned long)node;
#endif // defined(DBR_DEBUG_ALLOC)
    return node;
}

DBR_EXTERN void
elm_pool_free_small(struct ElmPool* pool, struct ElmSmallNode* node)
{
    if (node) {

        node->next = pool->small.first_node;
        pool->small.first_node = node;

#if defined(DBR_DEBUG_ALLOC)
        node->file = NULL;
        node->line = 0;

        --pool->allocs;
        pool->checksum ^= (unsigned long)node;

        assert(pool->allocs > 0 || (pool->allocs == 0 && pool->checksum == 0));
#endif // defined(DBR_DEBUG_ALLOC)
    }
}

DBR_EXTERN void
elm_pool_free_large(struct ElmPool* pool, struct ElmLargeNode* node)
{
    if (node) {

        node->next = pool->large.first_node;
        pool->large.first_node = node;

#if defined(DBR_DEBUG_ALLOC)
        node->file = NULL;
        node->line = 0;

        --pool->allocs;
        pool->checksum ^= (unsigned long)node;

        assert(pool->allocs > 0 || (pool->allocs == 0 && pool->checksum == 0));
#endif // defined(DBR_DEBUG_ALLOC)
    }
}

DBR_API DbrPool
dbr_pool_create(void)
{
    DbrPool pool = malloc(sizeof(struct ElmPool));
    if (dbr_unlikely(!pool)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

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
dbr_pool_alloc_level(DbrPool pool)
{
    return elm_pool_alloc_level(pool);
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
dbr_pool_alloc_order(DbrPool pool)
{
    return elm_pool_alloc_order(pool);
}

DBR_API void
dbr_pool_free_order(DbrPool pool, struct DbrOrder* order)
{
    elm_pool_free_order(pool, order);
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

DBR_API struct DbrTrade*
dbr_pool_alloc_trade(DbrPool pool)
{
    return elm_pool_alloc_trade(pool);
}

DBR_API void
dbr_pool_free_trade(DbrPool pool, struct DbrTrade* trade)
{
    elm_pool_free_trade(pool, trade);
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

DBR_API struct DbrStmt*
dbr_pool_alloc_stmt(DbrPool pool)
{
    return elm_pool_alloc_stmt(pool);
}

DBR_API void
dbr_pool_free_stmt(DbrPool pool, struct DbrStmt* stmt)
{
    elm_pool_free_stmt(pool, stmt);
}

DBR_API void
dbr_pool_free_entities(DbrPool pool, int type, struct DbrSlNode* first)
{
    struct DbrSlNode* node = first;
    switch (type) {
    case DBR_TRADER:
    case DBR_ACCNT:
    case DBR_CONTR:
        while (node) {
            struct DbrRec* rec = dbr_rec_entry(node);
            node = node->next;
            dbr_pool_free_rec(pool, rec);
        }
        break;
    case DBR_ORDER:
        while (node) {
            struct DbrOrder* order = dbr_order_entry(node);
            node = node->next;
            dbr_pool_free_order(pool, order);
        }
        break;
    case DBR_MEMB:
        while (node) {
            struct DbrMemb* memb = dbr_memb_entry(node);
            node = node->next;
            dbr_pool_free_memb(pool, memb);
        }
        break;
    case DBR_TRADE:
        while (node) {
            struct DbrTrade* trade = dbr_trade_entry(node);
            node = node->next;
            dbr_pool_free_trade(pool, trade);
        }
        break;
    case DBR_POSN:
        while (node) {
            struct DbrPosn* posn = dbr_posn_entry(node);
            node = node->next;
            dbr_pool_free_posn(pool, posn);
        }
        break;
    default:
        abort();
    }
}
