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
#ifndef FIG_POOL_H
#define FIG_POOL_H

#include <dbr/dlnode.h>
#include <dbr/log.h>
#include <dbr/rbnode.h>
#include <dbr/slnode.h>
#include <dbr/types.h>

struct FigSmallBlock;
struct FigLargeBlock;

struct FigPool {
    struct {
        size_t nodes_per_block;
        // List of allocated memory blocks.
        struct FigSmallBlock* first_block;
        // Free list.
        struct FigSmallNode* first_node;
    } small;
    struct {
        size_t nodes_per_block;
        // List of allocated memory blocks.
        struct FigLargeBlock* first_block;
        // Free list.
        struct FigLargeNode* first_node;
    } large;
#if !defined(DBR_DEBUG_ALLOC)
    // Defensively maintain consistent memory layout.
    long allocs__;
    unsigned long checksum__;
#else  // DBR_DEBUG_ALLOC
    long allocs;
    unsigned long checksum;
#endif // DBR_DEBUG_ALLOC
};

struct FigSmallNode {
    union {
        struct FigSmallNode* next;
        // Small data structures.
        struct DbrLevel level;
        struct DbrMatch match;
        struct DbrMemb memb;
        struct DbrPosn posn;
        struct DbrSub sub;
    };
#if !defined(DBR_DEBUG_ALLOC)
    // Defensively maintain consistent memory layout.
    const char* file__;
    int line__;
#else  // DBR_DEBUG_ALLOC
    const char* file;
    int line;
#endif // DBR_DEBUG_ALLOC
};

struct FigLargeNode {
    union {
        struct FigLargeNode* next;
        // Large data structures.
        struct DbrRec rec;
        struct DbrOrder order;
        struct DbrTrade trade;
    };
#if !defined(DBR_DEBUG_ALLOC)
    // Defensively maintain consistent memory layout.
    const char* file__;
    int line__;
#else  // DBR_DEBUG_ALLOC
    const char* file;
    int line;
#endif // DBR_DEBUG_ALLOC
};

struct FigSmallBlock {
    struct FigSmallBlock* next;
    struct FigSmallNode nodes[];
};

struct FigLargeBlock {
    struct FigLargeBlock* next;
    struct FigLargeNode nodes[];
};

// Error fields are set on failure.

DBR_EXTERN DbrBool
fig_pool_init(struct FigPool* pool);

// Assumes that pointer is not null.

DBR_EXTERN void
fig_pool_term(struct FigPool* pool);

DBR_EXTERN void
fig_pool_free_small(struct FigPool* pool, struct FigSmallNode* node);

DBR_EXTERN void
fig_pool_free_large(struct FigPool* pool, struct FigLargeNode* node);

#if !defined(DBR_DEBUG_ALLOC)

DBR_EXTERN struct FigSmallNode*
fig_pool_alloc_small(struct FigPool* pool);

DBR_EXTERN struct FigLargeNode*
fig_pool_alloc_large(struct FigPool* pool);

static inline struct DbrRec*
fig_pool_alloc_rec(struct FigPool* pool)
{
    struct FigLargeNode* node = fig_pool_alloc_large(pool);
    if (!node)
        return NULL;
    // Initialise private section.
    struct DbrRec* rec = &node->rec;
    dbr_slnode_init(&rec->model_node_);
    dbr_slnode_init(&rec->id_node_);
    dbr_slnode_init(&rec->mnem_node_);
    return rec;
}

static inline void
fig_pool_free_rec(struct FigPool* pool, struct DbrRec* rec)
{
    struct FigLargeNode* node = (struct FigLargeNode*)rec;
    fig_pool_free_large(pool, node);
}

static inline struct DbrLevel*
fig_pool_alloc_level(struct FigPool* pool, DbrKey key)
{
    struct FigSmallNode* node = fig_pool_alloc_small(pool);
    if (!node)
        return NULL;
    struct DbrLevel* level = &node->level;
    dbr_rbnode_init(&level->side_node_, key);
    return level;
}

static inline void
fig_pool_free_level(struct FigPool* pool, struct DbrLevel* level)
{
    struct FigSmallNode* node = (struct FigSmallNode*)level;
    fig_pool_free_small(pool, node);
}

static inline struct DbrMatch*
fig_pool_alloc_match(struct FigPool* pool)
{
    struct FigSmallNode* node = fig_pool_alloc_small(pool);
    return node ? &node->match : NULL;
}

static inline void
fig_pool_free_match(struct FigPool* pool, struct DbrMatch* match)
{
    struct FigSmallNode* node = (struct FigSmallNode*)match;
    fig_pool_free_small(pool, node);
}

static inline struct DbrOrder*
fig_pool_alloc_order(struct FigPool* pool, DbrKey key)
{
    struct FigLargeNode* node = fig_pool_alloc_large(pool);
    if (!node)
        return NULL;
    struct DbrOrder* order = &node->order;
    dbr_slnode_init(&order->model_node_);
    dbr_slnode_init(&order->ref_node_);
    dbr_dlnode_init(&order->side_node_);
    dbr_rbnode_init(&order->trader_node_, key);
    return order;
}

static inline void
fig_pool_free_order(struct FigPool* pool, struct DbrOrder* order)
{
    struct FigLargeNode* node = (struct FigLargeNode*)order;
    fig_pool_free_large(pool, node);
}

static inline struct DbrMemb*
fig_pool_alloc_memb(struct FigPool* pool, DbrKey key)
{
    struct FigSmallNode* node = fig_pool_alloc_small(pool);
    if (!node)
        return NULL;
    struct DbrMemb* memb = &node->memb;
    dbr_slnode_init(&memb->model_node_);
    dbr_rbnode_init(&memb->accnt_node_, key);
    return memb;
}

static inline void
fig_pool_free_memb(struct FigPool* pool, struct DbrMemb* memb)
{
    struct FigSmallNode* node = (struct FigSmallNode*)memb;
    fig_pool_free_small(pool, node);
}

static inline struct DbrTrade*
fig_pool_alloc_trade(struct FigPool* pool, DbrKey key)
{
    struct FigLargeNode* node = fig_pool_alloc_large(pool);
    if (!node)
        return NULL;
    struct DbrTrade* trade = &node->trade;
    dbr_slnode_init(&trade->model_node_);
    dbr_rbnode_init(&trade->accnt_node_, key);
    return trade;
}

static inline void
fig_pool_free_trade(struct FigPool* pool, struct DbrTrade* trade)
{
    struct FigLargeNode* node = (struct FigLargeNode*)trade;
    fig_pool_free_large(pool, node);
}

static inline struct DbrPosn*
fig_pool_alloc_posn(struct FigPool* pool, DbrKey key)
{
    struct FigSmallNode* node = fig_pool_alloc_small(pool);
    if (!node)
        return NULL;
    struct DbrPosn* posn = &node->posn;
    dbr_slnode_init(&posn->model_node_);
    dbr_rbnode_init(&posn->accnt_node_, key);
    return posn;
}

static inline void
fig_pool_free_posn(struct FigPool* pool, struct DbrPosn* posn)
{
    struct FigSmallNode* node = (struct FigSmallNode*)posn;
    fig_pool_free_small(pool, node);
}

static inline struct DbrSub*
fig_pool_alloc_sub(struct FigPool* pool, DbrKey key)
{
    struct FigSmallNode* node = fig_pool_alloc_small(pool);
    if (!node)
        return NULL;
    struct DbrSub* sub = &node->sub;
    dbr_dlnode_init(&sub->market_node_);
    dbr_rbnode_init(&sub->trader_node_, key);
    return sub;
}

static inline void
fig_pool_free_sub(struct FigPool* pool, struct DbrSub* sub)
{
    struct FigSmallNode* node = (struct FigSmallNode*)sub;
    fig_pool_free_small(pool, node);
}

#else  // DBR_DEBUG_ALLOC

DBR_EXTERN struct FigSmallNode*
fig_pool_alloc_small(struct FigPool* pool, const char* file, int line);

DBR_EXTERN struct FigLargeNode*
fig_pool_alloc_large(struct FigPool* pool, const char* file, int line);

static inline struct DbrRec*
fig_pool_alloc_rec_(struct FigPool* pool, const char* file, int line)
{
    struct FigLargeNode* node = fig_pool_alloc_large(pool, file, line);
    dbr_log_debug3("allocating %p rec in %s at %d", node, file, line);
    if (!node)
        return NULL;
    // Initialise private section.
    struct DbrRec* rec = &node->rec;
    dbr_slnode_init(&rec->model_node_);
    dbr_slnode_init(&rec->id_node_);
    dbr_slnode_init(&rec->mnem_node_);
    return rec;
}

static inline void
fig_pool_free_rec(struct FigPool* pool, struct DbrRec* rec)
{
    struct FigLargeNode* node = (struct FigLargeNode*)rec;
    dbr_log_debug3("freeing rec %p from %s at %d", node, node->file, node->line);
    fig_pool_free_large(pool, node);
}

static inline struct DbrLevel*
fig_pool_alloc_level_(struct FigPool* pool, DbrKey key, const char* file, int line)
{
    struct FigSmallNode* node = fig_pool_alloc_small(pool, file, line);
    dbr_log_debug3("allocating level %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrLevel* level = &node->level;
    dbr_rbnode_init(&level->side_node_, key);
    return level;
}

static inline void
fig_pool_free_level(struct FigPool* pool, struct DbrLevel* level)
{
    struct FigSmallNode* node = (struct FigSmallNode*)level;
    dbr_log_debug3("freeing level %p from %s at %d", node, node->file, node->line);
    fig_pool_free_small(pool, node);
}

static inline struct DbrMatch*
fig_pool_alloc_match_(struct FigPool* pool, const char* file, int line)
{
    struct FigSmallNode* node = fig_pool_alloc_small(pool, file, line);
    dbr_log_debug3("allocating match %p in %s at %d", node, file, line);
    return node ? &node->match : NULL;
}

static inline void
fig_pool_free_match(struct FigPool* pool, struct DbrMatch* match)
{
    struct FigSmallNode* node = (struct FigSmallNode*)match;
    dbr_log_debug3("freeing match %p from %s at %d", node, node->file, node->line);
    fig_pool_free_small(pool, node);
}

static inline struct DbrOrder*
fig_pool_alloc_order_(struct FigPool* pool, DbrKey key, const char* file, int line)
{
    struct FigLargeNode* node = fig_pool_alloc_large(pool, file, line);
    dbr_log_debug3("allocating order %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrOrder* order = &node->order;
    dbr_slnode_init(&order->model_node_);
    dbr_slnode_init(&order->ref_node_);
    dbr_dlnode_init(&order->side_node_);
    dbr_rbnode_init(&order->trader_node_, key);
    return order;
}

static inline void
fig_pool_free_order(struct FigPool* pool, struct DbrOrder* order)
{
    struct FigLargeNode* node = (struct FigLargeNode*)order;
    dbr_log_debug3("freeing order %p from %s at %d", node, node->file, node->line);
    fig_pool_free_large(pool, node);
}

static inline struct DbrMemb*
fig_pool_alloc_memb_(struct FigPool* pool, DbrKey key, const char* file, int line)
{
    struct FigSmallNode* node = fig_pool_alloc_small(pool, file, line);
    dbr_log_debug3("allocating memb %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrMemb* memb = &node->memb;
    dbr_slnode_init(&memb->model_node_);
    dbr_rbnode_init(&memb->accnt_node_, key);
    return memb;
}

static inline void
fig_pool_free_memb(struct FigPool* pool, struct DbrMemb* memb)
{
    struct FigSmallNode* node = (struct FigSmallNode*)memb;
    dbr_log_debug3("freeing memb %p from %s at %d", node, node->file, node->line);
    fig_pool_free_small(pool, node);
}

static inline struct DbrTrade*
fig_pool_alloc_trade_(struct FigPool* pool, DbrKey key, const char* file, int line)
{
    struct FigLargeNode* node = fig_pool_alloc_large(pool, file, line);
    dbr_log_debug3("allocating trade %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrTrade* trade = &node->trade;
    dbr_slnode_init(&trade->model_node_);
    dbr_rbnode_init(&trade->accnt_node_, key);
    return trade;
}

static inline void
fig_pool_free_trade(struct FigPool* pool, struct DbrTrade* trade)
{
    struct FigLargeNode* node = (struct FigLargeNode*)trade;
    dbr_log_debug3("freeing trade %p from %s at %d", node, node->file, node->line);
    fig_pool_free_large(pool, node);
}

static inline struct DbrPosn*
fig_pool_alloc_posn_(struct FigPool* pool, DbrKey key, const char* file, int line)
{
    struct FigSmallNode* node = fig_pool_alloc_small(pool, file, line);
    dbr_log_debug3("allocating posn %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrPosn* posn = &node->posn;
    dbr_slnode_init(&posn->model_node_);
    dbr_rbnode_init(&posn->accnt_node_, key);
    return posn;
}

static inline void
fig_pool_free_posn(struct FigPool* pool, struct DbrPosn* posn)
{
    struct FigSmallNode* node = (struct FigSmallNode*)posn;
    dbr_log_debug3("freeing posn %p from %s at %d", node, node->file, node->line);
    fig_pool_free_small(pool, node);
}

static inline struct DbrSub*
fig_pool_alloc_sub_(struct FigPool* pool, DbrKey key, const char* file, int line)
{
    struct FigSmallNode* node = fig_pool_alloc_small(pool, file, line);
    dbr_log_debug3("allocating sub %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrSub* sub = &node->sub;
    dbr_dlnode_init(&sub->market_node_);
    dbr_rbnode_init(&sub->trader_node_, key);
    return sub;
}

static inline void
fig_pool_free_sub(struct FigPool* pool, struct DbrSub* sub)
{
    struct FigSmallNode* node = (struct FigSmallNode*)sub;
    dbr_log_debug3("freeing sub %p from %s at %d", node, node->file, node->line);
    fig_pool_free_small(pool, node);
}

#define fig_pool_alloc_rec(pool)                            \
    fig_pool_alloc_rec_(pool, __FILE__, __LINE__)
#define fig_pool_alloc_level(pool, key)                     \
    fig_pool_alloc_level_(pool, key, __FILE__, __LINE__)
#define fig_pool_alloc_match(pool)                          \
    fig_pool_alloc_match_(pool, __FILE__, __LINE__)
#define fig_pool_alloc_order(pool, key)                     \
    fig_pool_alloc_order_(pool, key, __FILE__, __LINE__)
#define fig_pool_alloc_memb(pool, key)                      \
    fig_pool_alloc_memb_(pool, key, __FILE__, __LINE__)
#define fig_pool_alloc_trade(pool, key)                     \
    fig_pool_alloc_trade_(pool, key, __FILE__, __LINE__)
#define fig_pool_alloc_posn(pool, key)                      \
    fig_pool_alloc_posn_(pool, key, __FILE__, __LINE__)
#define fig_pool_alloc_sub(pool, key)                       \
    fig_pool_alloc_sub_(pool, key, __FILE__, __LINE__)

#endif // DBR_DEBUG_ALLOC

// Free all matches and trades. Used to unwind match failure. No effect if first argument is null.

DBR_EXTERN void
fig_pool_free_matches(struct FigPool* pool, struct DbrSlNode* first);

#endif // FIG_POOL_H
