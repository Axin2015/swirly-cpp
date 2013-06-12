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
#ifndef ELM_POOL_H
#define ELM_POOL_H

#include <dbr/types.h>

#include <ash/dlnode.h>
#include <ash/rbnode.h>
#include <ash/slnode.h>

struct ElmSmallBlock;
struct ElmLargeBlock;

struct ElmPool {
    struct {
        size_t nodes_per_block;
        // List of allocated memory blocks.
        struct ElmSmallBlock* first_block;
        // Free list.
        struct ElmSmallNode* first_node;
    } small;
    struct {
        size_t nodes_per_block;
        // List of allocated memory blocks.
        struct ElmLargeBlock* first_block;
        // Free list.
        struct ElmLargeNode* first_node;
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

struct ElmSmallNode {
    union {
        struct ElmSmallNode* next;
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

struct ElmLargeNode {
    union {
        struct ElmLargeNode* next;
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

struct ElmSmallBlock {
    struct ElmSmallBlock* next;
    struct ElmSmallNode nodes[];
};

struct ElmLargeBlock {
    struct ElmLargeBlock* next;
    struct ElmLargeNode nodes[];
};

// Error fields are set on failure.

DBR_EXTERN DbrBool
elm_pool_init(struct ElmPool* pool);

// Assumes that pointer is not null.

DBR_EXTERN void
elm_pool_term(struct ElmPool* pool);

DBR_EXTERN void
elm_pool_free_small(struct ElmPool* pool, struct ElmSmallNode* node);

DBR_EXTERN void
elm_pool_free_large(struct ElmPool* pool, struct ElmLargeNode* node);

#if !defined(DBR_DEBUG_ALLOC)

DBR_EXTERN struct ElmSmallNode*
elm_pool_alloc_small(struct ElmPool* pool);

DBR_EXTERN struct ElmLargeNode*
elm_pool_alloc_large(struct ElmPool* pool);

static inline struct DbrRec*
elm_pool_alloc_rec(struct ElmPool* pool)
{
    struct ElmLargeNode* node = elm_pool_alloc_large(pool);
    if (!node)
        return NULL;
    // Initialise private section.
    struct DbrRec* rec = &node->rec;
    ash_slnode_init(&rec->model_node_);
    ash_slnode_init(&rec->id_node_);
    ash_slnode_init(&rec->mnem_node_);
    return rec;
}

static inline void
elm_pool_free_rec(struct ElmPool* pool, struct DbrRec* rec)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)rec;
    elm_pool_free_large(pool, node);
}

static inline struct DbrLevel*
elm_pool_alloc_level(struct ElmPool* pool, DbrKey key)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool);
    if (!node)
        return NULL;
    struct DbrLevel* level = &node->level;
    ash_rbnode_init(&level->side_node_, key);
    return level;
}

static inline void
elm_pool_free_level(struct ElmPool* pool, struct DbrLevel* level)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)level;
    elm_pool_free_small(pool, node);
}

static inline struct DbrMatch*
elm_pool_alloc_match(struct ElmPool* pool)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool);
    return node ? &node->match : NULL;
}

static inline void
elm_pool_free_match(struct ElmPool* pool, struct DbrMatch* match)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)match;
    elm_pool_free_small(pool, node);
}

static inline struct DbrOrder*
elm_pool_alloc_order(struct ElmPool* pool, DbrKey key)
{
    struct ElmLargeNode* node = elm_pool_alloc_large(pool);
    if (!node)
        return NULL;
    struct DbrOrder* order = &node->order;
    ash_slnode_init(&order->model_node_);
    ash_slnode_init(&order->ref_node_);
    ash_dlnode_init(&order->side_node_);
    ash_rbnode_init(&order->trader_node_, key);
    return order;
}

static inline void
elm_pool_free_order(struct ElmPool* pool, struct DbrOrder* order)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)order;
    elm_pool_free_large(pool, node);
}

static inline struct DbrMemb*
elm_pool_alloc_memb(struct ElmPool* pool, DbrKey key)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool);
    if (!node)
        return NULL;
    struct DbrMemb* memb = &node->memb;
    ash_slnode_init(&memb->model_node_);
    ash_rbnode_init(&memb->accnt_node_, key);
    return memb;
}

static inline void
elm_pool_free_memb(struct ElmPool* pool, struct DbrMemb* memb)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)memb;
    elm_pool_free_small(pool, node);
}

static inline struct DbrTrade*
elm_pool_alloc_trade(struct ElmPool* pool, DbrKey key)
{
    struct ElmLargeNode* node = elm_pool_alloc_large(pool);
    if (!node)
        return NULL;
    struct DbrTrade* trade = &node->trade;
    ash_slnode_init(&trade->model_node_);
    ash_rbnode_init(&trade->accnt_node_, key);
    return trade;
}

static inline void
elm_pool_free_trade(struct ElmPool* pool, struct DbrTrade* trade)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)trade;
    elm_pool_free_large(pool, node);
}

static inline struct DbrPosn*
elm_pool_alloc_posn(struct ElmPool* pool, DbrKey key)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool);
    if (!node)
        return NULL;
    struct DbrPosn* posn = &node->posn;
    ash_slnode_init(&posn->model_node_);
    ash_rbnode_init(&posn->accnt_node_, key);
    return posn;
}

static inline void
elm_pool_free_posn(struct ElmPool* pool, struct DbrPosn* posn)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)posn;
    elm_pool_free_small(pool, node);
}

static inline struct DbrSub*
elm_pool_alloc_sub(struct ElmPool* pool, DbrKey key)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool);
    if (!node)
        return NULL;
    struct DbrSub* sub = &node->sub;
    ash_dlnode_init(&sub->market_node_);
    ash_rbnode_init(&sub->trader_node_, key);
    return sub;
}

static inline void
elm_pool_free_sub(struct ElmPool* pool, struct DbrSub* sub)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)sub;
    elm_pool_free_small(pool, node);
}

#else  // DBR_DEBUG_ALLOC

DBR_EXTERN struct ElmSmallNode*
elm_pool_alloc_small(struct ElmPool* pool, const char* file, int line);

DBR_EXTERN struct ElmLargeNode*
elm_pool_alloc_large(struct ElmPool* pool, const char* file, int line);

static inline struct DbrRec*
elm_pool_alloc_rec_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmLargeNode* node = elm_pool_alloc_large(pool, file, line);
    DBR_DEBUG3F("allocating %p rec in %s at %d", node, file, line);
    if (!node)
        return NULL;
    // Initialise private section.
    struct DbrRec* rec = &node->rec;
    ash_slnode_init(&rec->model_node_);
    ash_slnode_init(&rec->id_node_);
    ash_slnode_init(&rec->mnem_node_);
    return rec;
}

static inline void
elm_pool_free_rec(struct ElmPool* pool, struct DbrRec* rec)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)rec;
    DBR_DEBUG3F("freeing rec %p from %s at %d", node, node->file, node->line);
    elm_pool_free_large(pool, node);
}

static inline struct DbrLevel*
elm_pool_alloc_level_(struct ElmPool* pool, DbrKey key, const char* file, int line)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool, file, line);
    DBR_DEBUG3F("allocating level %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrLevel* level = &node->level;
    ash_rbnode_init(&level->side_node_, key);
    return level;
}

static inline void
elm_pool_free_level(struct ElmPool* pool, struct DbrLevel* level)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)level;
    DBR_DEBUG3F("freeing level %p from %s at %d", node, node->file, node->line);
    elm_pool_free_small(pool, node);
}

static inline struct DbrMatch*
elm_pool_alloc_match_(struct ElmPool* pool, const char* file, int line)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool, file, line);
    DBR_DEBUG3F("allocating match %p in %s at %d", node, file, line);
    return node ? &node->match : NULL;
}

static inline void
elm_pool_free_match(struct ElmPool* pool, struct DbrMatch* match)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)match;
    DBR_DEBUG3F("freeing match %p from %s at %d", node, node->file, node->line);
    elm_pool_free_small(pool, node);
}

static inline struct DbrOrder*
elm_pool_alloc_order_(struct ElmPool* pool, DbrKey key, const char* file, int line)
{
    struct ElmLargeNode* node = elm_pool_alloc_large(pool, file, line);
    DBR_DEBUG3F("allocating order %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrOrder* order = &node->order;
    ash_slnode_init(&order->model_node_);
    ash_slnode_init(&order->ref_node_);
    ash_dlnode_init(&order->side_node_);
    ash_rbnode_init(&order->trader_node_, key);
    return order;
}

static inline void
elm_pool_free_order(struct ElmPool* pool, struct DbrOrder* order)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)order;
    DBR_DEBUG3F("freeing order %p from %s at %d", node, node->file, node->line);
    elm_pool_free_large(pool, node);
}

static inline struct DbrMemb*
elm_pool_alloc_memb_(struct ElmPool* pool, DbrKey key, const char* file, int line)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool, file, line);
    DBR_DEBUG3F("allocating memb %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrMemb* memb = &node->memb;
    ash_slnode_init(&memb->model_node_);
    ash_rbnode_init(&memb->accnt_node_, key);
    return memb;
}

static inline void
elm_pool_free_memb(struct ElmPool* pool, struct DbrMemb* memb)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)memb;
    DBR_DEBUG3F("freeing memb %p from %s at %d", node, node->file, node->line);
    elm_pool_free_small(pool, node);
}

static inline struct DbrTrade*
elm_pool_alloc_trade_(struct ElmPool* pool, DbrKey key, const char* file, int line)
{
    struct ElmLargeNode* node = elm_pool_alloc_large(pool, file, line);
    DBR_DEBUG3F("allocating trade %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrTrade* trade = &node->trade;
    ash_slnode_init(&trade->model_node_);
    ash_rbnode_init(&trade->accnt_node_, key);
    return trade;
}

static inline void
elm_pool_free_trade(struct ElmPool* pool, struct DbrTrade* trade)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)trade;
    DBR_DEBUG3F("freeing trade %p from %s at %d", node, node->file, node->line);
    elm_pool_free_large(pool, node);
}

static inline struct DbrPosn*
elm_pool_alloc_posn_(struct ElmPool* pool, DbrKey key, const char* file, int line)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool, file, line);
    DBR_DEBUG3F("allocating posn %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrPosn* posn = &node->posn;
    ash_slnode_init(&posn->model_node_);
    ash_rbnode_init(&posn->accnt_node_, key);
    return posn;
}

static inline void
elm_pool_free_posn(struct ElmPool* pool, struct DbrPosn* posn)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)posn;
    DBR_DEBUG3F("freeing posn %p from %s at %d", node, node->file, node->line);
    elm_pool_free_small(pool, node);
}

static inline struct DbrSub*
elm_pool_alloc_sub_(struct ElmPool* pool, DbrKey key, const char* file, int line)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool, file, line);
    DBR_DEBUG3F("allocating sub %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrSub* sub = &node->sub;
    ash_dlnode_init(&sub->market_node_);
    ash_rbnode_init(&sub->trader_node_, key);
    return sub;
}

static inline void
elm_pool_free_sub(struct ElmPool* pool, struct DbrSub* sub)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)sub;
    DBR_DEBUG3F("freeing sub %p from %s at %d", node, node->file, node->line);
    elm_pool_free_small(pool, node);
}

#define elm_pool_alloc_rec(pool)                            \
    elm_pool_alloc_rec_(pool, __FILE__, __LINE__)
#define elm_pool_alloc_level(pool, key)                     \
    elm_pool_alloc_level_(pool, key, __FILE__, __LINE__)
#define elm_pool_alloc_match(pool)                          \
    elm_pool_alloc_match_(pool, __FILE__, __LINE__)
#define elm_pool_alloc_order(pool, key)                     \
    elm_pool_alloc_order_(pool, key, __FILE__, __LINE__)
#define elm_pool_alloc_memb(pool, key)                      \
    elm_pool_alloc_memb_(pool, key, __FILE__, __LINE__)
#define elm_pool_alloc_trade(pool, key)                     \
    elm_pool_alloc_trade_(pool, key, __FILE__, __LINE__)
#define elm_pool_alloc_posn(pool, key)                      \
    elm_pool_alloc_posn_(pool, key, __FILE__, __LINE__)
#define elm_pool_alloc_sub(pool, key)                       \
    elm_pool_alloc_sub_(pool, key, __FILE__, __LINE__)

#endif // DBR_DEBUG_ALLOC

// Free all matches and trades. Used to unwind match failure. No effect if first argument is null.

DBR_EXTERN void
elm_pool_free_matches(struct ElmPool* pool, struct DbrSlNode* first);

#endif // ELM_POOL_H
