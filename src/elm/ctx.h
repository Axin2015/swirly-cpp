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
#ifndef ELM_CTX_H
#define ELM_CTX_H

#include <dbr/types.h>

#include <ash/dlnode.h>
#include <ash/rbnode.h>
#include <ash/slnode.h>

struct ElmSmallBlock;
struct ElmLargeBlock;

struct ElmCtx {
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
elm_ctx_init(struct ElmCtx* ctx);

// Assumes that pointer is not null.

DBR_EXTERN void
elm_ctx_term(struct ElmCtx* ctx);

DBR_EXTERN void
elm_ctx_free_small(struct ElmCtx* ctx, struct ElmSmallNode* node);

DBR_EXTERN void
elm_ctx_free_large(struct ElmCtx* ctx, struct ElmLargeNode* node);

#if !defined(DBR_DEBUG_ALLOC)

DBR_EXTERN struct ElmSmallNode*
elm_ctx_alloc_small(struct ElmCtx* ctx);

DBR_EXTERN struct ElmLargeNode*
elm_ctx_alloc_large(struct ElmCtx* ctx);

static inline struct DbrRec*
elm_ctx_alloc_rec(struct ElmCtx* ctx)
{
    struct ElmLargeNode* node = elm_ctx_alloc_large(ctx);
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
elm_ctx_free_rec(struct ElmCtx* ctx, struct DbrRec* rec)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)rec;
    elm_ctx_free_large(ctx, node);
}

static inline struct DbrLevel*
elm_ctx_alloc_level(struct ElmCtx* ctx, DbrKey key)
{
    struct ElmSmallNode* node = elm_ctx_alloc_small(ctx);
    if (!node)
        return NULL;
    struct DbrLevel* level = &node->level;
    ash_rbnode_init(&level->side_node_, key);
    return level;
}

static inline void
elm_ctx_free_level(struct ElmCtx* ctx, struct DbrLevel* level)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)level;
    elm_ctx_free_small(ctx, node);
}

static inline struct DbrMatch*
elm_ctx_alloc_match(struct ElmCtx* ctx)
{
    struct ElmSmallNode* node = elm_ctx_alloc_small(ctx);
    return node ? &node->match : NULL;
}

static inline void
elm_ctx_free_match(struct ElmCtx* ctx, struct DbrMatch* match)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)match;
    elm_ctx_free_small(ctx, node);
}

static inline struct DbrOrder*
elm_ctx_alloc_order(struct ElmCtx* ctx, DbrKey key)
{
    struct ElmLargeNode* node = elm_ctx_alloc_large(ctx);
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
elm_ctx_free_order(struct ElmCtx* ctx, struct DbrOrder* order)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)order;
    elm_ctx_free_large(ctx, node);
}

static inline struct DbrMemb*
elm_ctx_alloc_memb(struct ElmCtx* ctx, DbrKey key)
{
    struct ElmSmallNode* node = elm_ctx_alloc_small(ctx);
    if (!node)
        return NULL;
    struct DbrMemb* memb = &node->memb;
    ash_slnode_init(&memb->model_node_);
    ash_rbnode_init(&memb->accnt_node_, key);
    return memb;
}

static inline void
elm_ctx_free_memb(struct ElmCtx* ctx, struct DbrMemb* memb)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)memb;
    elm_ctx_free_small(ctx, node);
}

static inline struct DbrTrade*
elm_ctx_alloc_trade(struct ElmCtx* ctx, DbrKey key)
{
    struct ElmLargeNode* node = elm_ctx_alloc_large(ctx);
    if (!node)
        return NULL;
    struct DbrTrade* trade = &node->trade;
    ash_slnode_init(&trade->model_node_);
    ash_rbnode_init(&trade->accnt_node_, key);
    return trade;
}

static inline void
elm_ctx_free_trade(struct ElmCtx* ctx, struct DbrTrade* trade)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)trade;
    elm_ctx_free_large(ctx, node);
}

static inline struct DbrPosn*
elm_ctx_alloc_posn(struct ElmCtx* ctx, DbrKey key)
{
    struct ElmSmallNode* node = elm_ctx_alloc_small(ctx);
    if (!node)
        return NULL;
    struct DbrPosn* posn = &node->posn;
    ash_slnode_init(&posn->model_node_);
    ash_rbnode_init(&posn->accnt_node_, key);
    return posn;
}

static inline void
elm_ctx_free_posn(struct ElmCtx* ctx, struct DbrPosn* posn)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)posn;
    elm_ctx_free_small(ctx, node);
}

static inline struct DbrSub*
elm_ctx_alloc_sub(struct ElmCtx* ctx, DbrKey key)
{
    struct ElmSmallNode* node = elm_ctx_alloc_small(ctx);
    if (!node)
        return NULL;
    struct DbrSub* sub = &node->sub;
    ash_dlnode_init(&sub->market_node_);
    ash_rbnode_init(&sub->trader_node_, key);
    return sub;
}

static inline void
elm_ctx_free_sub(struct ElmCtx* ctx, struct DbrSub* sub)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)sub;
    elm_ctx_free_small(ctx, node);
}

#else  // DBR_DEBUG_ALLOC

DBR_EXTERN struct ElmSmallNode*
elm_ctx_alloc_small(struct ElmCtx* ctx, const char* file, int line);

DBR_EXTERN struct ElmLargeNode*
elm_ctx_alloc_large(struct ElmCtx* ctx, const char* file, int line);

static inline struct DbrRec*
elm_ctx_alloc_rec_(struct ElmCtx* ctx, const char* file, int line)
{
    struct ElmLargeNode* node = elm_ctx_alloc_large(ctx, file, line);
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
elm_ctx_free_rec(struct ElmCtx* ctx, struct DbrRec* rec)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)rec;
    DBR_DEBUG3F("freeing rec %p from %s at %d", node, node->file, node->line);
    elm_ctx_free_large(ctx, node);
}

static inline struct DbrLevel*
elm_ctx_alloc_level_(struct ElmCtx* ctx, DbrKey key, const char* file, int line)
{
    struct ElmSmallNode* node = elm_ctx_alloc_small(ctx, file, line);
    DBR_DEBUG3F("allocating level %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrLevel* level = &node->level;
    ash_rbnode_init(&level->side_node_, key);
    return level;
}

static inline void
elm_ctx_free_level(struct ElmCtx* ctx, struct DbrLevel* level)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)level;
    DBR_DEBUG3F("freeing level %p from %s at %d", node, node->file, node->line);
    elm_ctx_free_small(ctx, node);
}

static inline struct DbrMatch*
elm_ctx_alloc_match_(struct ElmCtx* ctx, const char* file, int line)
{
    struct ElmSmallNode* node = elm_ctx_alloc_small(ctx, file, line);
    DBR_DEBUG3F("allocating match %p in %s at %d", node, file, line);
    return node ? &node->match : NULL;
}

static inline void
elm_ctx_free_match(struct ElmCtx* ctx, struct DbrMatch* match)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)match;
    DBR_DEBUG3F("freeing match %p from %s at %d", node, node->file, node->line);
    elm_ctx_free_small(ctx, node);
}

static inline struct DbrOrder*
elm_ctx_alloc_order_(struct ElmCtx* ctx, DbrKey key, const char* file, int line)
{
    struct ElmLargeNode* node = elm_ctx_alloc_large(ctx, file, line);
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
elm_ctx_free_order(struct ElmCtx* ctx, struct DbrOrder* order)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)order;
    DBR_DEBUG3F("freeing order %p from %s at %d", node, node->file, node->line);
    elm_ctx_free_large(ctx, node);
}

static inline struct DbrMemb*
elm_ctx_alloc_memb_(struct ElmCtx* ctx, DbrKey key, const char* file, int line)
{
    struct ElmSmallNode* node = elm_ctx_alloc_small(ctx, file, line);
    DBR_DEBUG3F("allocating memb %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrMemb* memb = &node->memb;
    ash_slnode_init(&memb->model_node_);
    ash_rbnode_init(&memb->accnt_node_, key);
    return memb;
}

static inline void
elm_ctx_free_memb(struct ElmCtx* ctx, struct DbrMemb* memb)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)memb;
    DBR_DEBUG3F("freeing memb %p from %s at %d", node, node->file, node->line);
    elm_ctx_free_small(ctx, node);
}

static inline struct DbrTrade*
elm_ctx_alloc_trade_(struct ElmCtx* ctx, DbrKey key, const char* file, int line)
{
    struct ElmLargeNode* node = elm_ctx_alloc_large(ctx, file, line);
    DBR_DEBUG3F("allocating trade %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrTrade* trade = &node->trade;
    ash_slnode_init(&trade->model_node_);
    ash_rbnode_init(&trade->accnt_node_, key);
    return trade;
}

static inline void
elm_ctx_free_trade(struct ElmCtx* ctx, struct DbrTrade* trade)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)trade;
    DBR_DEBUG3F("freeing trade %p from %s at %d", node, node->file, node->line);
    elm_ctx_free_large(ctx, node);
}

static inline struct DbrPosn*
elm_ctx_alloc_posn_(struct ElmCtx* ctx, DbrKey key, const char* file, int line)
{
    struct ElmSmallNode* node = elm_ctx_alloc_small(ctx, file, line);
    DBR_DEBUG3F("allocating posn %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrPosn* posn = &node->posn;
    ash_slnode_init(&posn->model_node_);
    ash_rbnode_init(&posn->accnt_node_, key);
    return posn;
}

static inline void
elm_ctx_free_posn(struct ElmCtx* ctx, struct DbrPosn* posn)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)posn;
    DBR_DEBUG3F("freeing posn %p from %s at %d", node, node->file, node->line);
    elm_ctx_free_small(ctx, node);
}

static inline struct DbrSub*
elm_ctx_alloc_sub_(struct ElmCtx* ctx, DbrKey key, const char* file, int line)
{
    struct ElmSmallNode* node = elm_ctx_alloc_small(ctx, file, line);
    DBR_DEBUG3F("allocating sub %p in %s at %d", node, file, line);
    if (!node)
        return NULL;
    struct DbrSub* sub = &node->sub;
    ash_dlnode_init(&sub->market_node_);
    ash_rbnode_init(&sub->trader_node_, key);
    return sub;
}

static inline void
elm_ctx_free_sub(struct ElmCtx* ctx, struct DbrSub* sub)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)sub;
    DBR_DEBUG3F("freeing sub %p from %s at %d", node, node->file, node->line);
    elm_ctx_free_small(ctx, node);
}

#define elm_ctx_alloc_rec(ctx)                          \
    elm_ctx_alloc_rec_(ctx, __FILE__, __LINE__)
#define elm_ctx_alloc_level(ctx, key)                   \
    elm_ctx_alloc_level_(ctx, key, __FILE__, __LINE__)
#define elm_ctx_alloc_match(ctx)                        \
    elm_ctx_alloc_match_(ctx, __FILE__, __LINE__)
#define elm_ctx_alloc_order(ctx, key)                   \
    elm_ctx_alloc_order_(ctx, key, __FILE__, __LINE__)
#define elm_ctx_alloc_memb(ctx, key)                    \
    elm_ctx_alloc_memb_(ctx, key, __FILE__, __LINE__)
#define elm_ctx_alloc_trade(ctx, key)                   \
    elm_ctx_alloc_trade_(ctx, key, __FILE__, __LINE__)
#define elm_ctx_alloc_posn(ctx, key)                    \
    elm_ctx_alloc_posn_(ctx, key, __FILE__, __LINE__)
#define elm_ctx_alloc_sub(ctx, key)                     \
    elm_ctx_alloc_sub_(ctx, key, __FILE__, __LINE__)

#endif // DBR_DEBUG_ALLOC

// Free all matches and trades. Used to unwind match failure. No effect if first argument is null.

DBR_EXTERN void
elm_ctx_free_matches(struct ElmCtx* ctx, struct DbrSlNode* first);

#endif // ELM_CTX_H
