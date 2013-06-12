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
#include "ctx.h"

#include "err.h"

#include <dbr/conv.h>

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static DbrBool
alloc_small_nodes(struct ElmCtx* ctx)
{
    struct ElmSmallBlock* block = malloc(sizeof(struct ElmSmallBlock)
                                         + ctx->small.nodes_per_block
                                         * sizeof(struct ElmSmallNode));
    if (dbr_unlikely(!block)) {
        elm_err_set(DBR_ENOMEM, "out of memory");
        return false;
    }

    // Push block.
    block->next = ctx->small.first_block;
    ctx->small.first_block = block;

    // Link chain of new nodes.
    const int last = ctx->small.nodes_per_block - 1;
    for (int i = 0; i < last; ++i) {
        block->nodes[i].next = &block->nodes[i + 1];
#if defined(DBR_DEBUG_ALLOC)
        block->nodes[i].file = NULL;
        block->nodes[i].line = 0;
#endif // DBR_DEBUG_ALLOC
    }

    // Last node links to any existing nodes.
    block->nodes[last].next = ctx->small.first_node;
#if defined(DBR_DEBUG_ALLOC)
    block->nodes[last].file = NULL;
    block->nodes[last].line = 0;
#endif // DBR_DEBUG_ALLOC

    // Newly allocated nodes are now at the front.
    ctx->small.first_node = &block->nodes[0];

    return true;
}

static DbrBool
alloc_large_nodes(struct ElmCtx* ctx)
{
    struct ElmLargeBlock* block = malloc(sizeof(struct ElmLargeBlock)
                                         + ctx->large.nodes_per_block
                                         * sizeof(struct ElmLargeNode));
    if (dbr_unlikely(!block)) {
        elm_err_set(DBR_ENOMEM, "out of memory");
        return false;
    }

    // Push block.
    block->next = ctx->large.first_block;
    ctx->large.first_block = block;

    // Link chain of new nodes.
    const int last = ctx->large.nodes_per_block - 1;
    for (int i = 0; i < last; ++i) {
        block->nodes[i].next = &block->nodes[i + 1];
#if defined(DBR_DEBUG_ALLOC)
        block->nodes[i].file = NULL;
        block->nodes[i].line = 0;
#endif // DBR_DEBUG_ALLOC
    }

    // Last node links to any existing nodes.
    block->nodes[last].next = ctx->large.first_node;
#if defined(DBR_DEBUG_ALLOC)
    block->nodes[last].file = NULL;
    block->nodes[last].line = 0;
#endif // DBR_DEBUG_ALLOC

    // Newly allocated nodes are now at the front.
    ctx->large.first_node = &block->nodes[0];

    return true;
}

DBR_EXTERN DbrBool
elm_ctx_init(struct ElmCtx* ctx)
{
    // Slightly less than one page of items.
    // ((page_size - header_size) / item_size) - 1

    const long page_size = sysconf(_SC_PAGESIZE);

    // Small.
    ctx->small.nodes_per_block = (page_size - sizeof(struct ElmSmallBlock))
        / sizeof(struct ElmSmallNode) - 1;
    ctx->small.first_block = NULL;
    ctx->small.first_node = NULL;

    // Large.
    ctx->large.nodes_per_block = (page_size - sizeof(struct ElmLargeBlock))
        / sizeof(struct ElmLargeNode) - 1;
    ctx->large.first_block = NULL;
    ctx->large.first_node = NULL;

#if defined(DBR_DEBUG_ALLOC)
    ctx->allocs = 0;
    ctx->checksum = 0;
#if DBR_DEBUG_LEVEL >= 1
    fprintf(stderr, "%zu small nodes per block:\n", ctx->small.nodes_per_block);
    fprintf(stderr, "sizeof DbrLevel=%zu\n", sizeof(struct DbrLevel));
    fprintf(stderr, "sizeof DbrMatch=%zu\n", sizeof(struct DbrMatch));
    fprintf(stderr, "sizeof DbrMemb=%zu\n", sizeof(struct DbrMemb));
    fprintf(stderr, "sizeof DbrPosn=%zu\n", sizeof(struct DbrPosn));
    fprintf(stderr, "sizeof DbrSub=%zu\n", sizeof(struct DbrSub));

    fprintf(stderr, "%zu large nodes per block:\n", ctx->large.nodes_per_block);
    fprintf(stderr, "sizeof DbrRec=%zu\n", sizeof(struct DbrRec));
    fprintf(stderr, "sizeof DbrOrder=%zu\n", sizeof(struct DbrOrder));
    fprintf(stderr, "sizeof DbrTrade=%zu\n", sizeof(struct DbrTrade));
#endif // DBR_DEBUG_LEVEL >= 1
#endif // DBR_DEBUG_ALLOC
    if (!alloc_small_nodes(ctx))
        goto fail1;

    if (!alloc_large_nodes(ctx))
        goto fail2;

    return true;
 fail2:
    // Defensively does not assume single block.
    while (ctx->small.first_block) {
        struct ElmSmallBlock* block = ctx->small.first_block;
        ctx->small.first_block = block->next;
        free(block);
    }
 fail1:
    return false;
}

DBR_EXTERN void
elm_ctx_term(struct ElmCtx* ctx)
{
    assert(ctx);

    // Large.
    while (ctx->large.first_block) {
        struct ElmLargeBlock* block = ctx->large.first_block;
#if defined(DBR_DEBUG_ALLOC)
        for (int i = 0; i < ctx->large.nodes_per_block; ++i) {
            if (block->nodes[i].file) {
                fprintf(stderr, "allocation in %s at %d\n",
                        block->nodes[i].file, block->nodes[i].line);
            }
        }
#endif // DBR_DEBUG_ALLOC
        ctx->large.first_block = block->next;
        free(block);
    }
    ctx->large.first_node = NULL;

    // Small.
    while (ctx->small.first_block) {
        struct ElmSmallBlock* block = ctx->small.first_block;
#if defined(DBR_DEBUG_ALLOC)
        for (int i = 0; i < ctx->small.nodes_per_block; ++i) {
            if (block->nodes[i].file) {
                fprintf(stderr, "allocation in %s at %d\n",
                        block->nodes[i].file, block->nodes[i].line);
            }
        }
#endif // DBR_DEBUG_ALLOC
        ctx->small.first_block = block->next;
        free(block);
    }
    ctx->small.first_node = NULL;

#if defined(DBR_DEBUG_ALLOC)
    fprintf(stderr, "%ld leaks detected\n", ctx->allocs);
    fflush(stderr);
    assert(ctx->allocs == 0 && ctx->checksum == 0);
#endif // DBR_DEBUG_ALLOC
}

DBR_EXTERN struct ElmSmallNode*
#if !defined(DBR_DEBUG_ALLOC)
elm_ctx_alloc_small(struct ElmCtx* ctx)
#else  // DBR_DEBUG_ALLOC
elm_ctx_alloc_small(struct ElmCtx* ctx, const char* file, int line)
#endif // DBR_DEBUG_ALLOC
{
    if (dbr_unlikely(!ctx->small.first_node && !alloc_small_nodes(ctx))) {
        elm_err_set(DBR_ENOMEM, "out of memory");
        return false;
    }
    struct ElmSmallNode* node = ctx->small.first_node;
    ctx->small.first_node = node->next;

#if defined(DBR_DEBUG_ALLOC)
    node->file = file;
    node->line = line;

    ++ctx->allocs;
    ctx->checksum ^= (unsigned long)node;
#endif // DBR_DEBUG_ALLOC
    return node;
}

DBR_EXTERN struct ElmLargeNode*
#if !defined(DBR_DEBUG_ALLOC)
elm_ctx_alloc_large(struct ElmCtx* ctx)
#else  // DBR_DEBUG_ALLOC
elm_ctx_alloc_large(struct ElmCtx* ctx, const char* file, int line)
#endif // DBR_DEBUG_ALLOC
{
    if (dbr_unlikely(!ctx->large.first_node && !alloc_large_nodes(ctx))) {
        elm_err_set(DBR_ENOMEM, "out of memory");
        return false;
    }
    struct ElmLargeNode* node = ctx->large.first_node;
    ctx->large.first_node = node->next;

#if defined(DBR_DEBUG_ALLOC)
    node->file = file;
    node->line = line;

    ++ctx->allocs;
    ctx->checksum ^= (unsigned long)node;
#endif // DBR_DEBUG_ALLOC
    return node;
}

DBR_EXTERN void
elm_ctx_free_small(struct ElmCtx* ctx, struct ElmSmallNode* node)
{
    if (node) {

        node->next = ctx->small.first_node;
        ctx->small.first_node = node;

#if defined(DBR_DEBUG_ALLOC)
        node->file = NULL;
        node->line = 0;

        --ctx->allocs;
        ctx->checksum ^= (unsigned long)node;

        assert(ctx->allocs > 0 || (ctx->allocs == 0 && ctx->checksum == 0));
#endif // DBR_DEBUG_ALLOC
    }
}

DBR_EXTERN void
elm_ctx_free_large(struct ElmCtx* ctx, struct ElmLargeNode* node)
{
    if (node) {

        node->next = ctx->large.first_node;
        ctx->large.first_node = node;

#if defined(DBR_DEBUG_ALLOC)
        node->file = NULL;
        node->line = 0;

        --ctx->allocs;
        ctx->checksum ^= (unsigned long)node;

        assert(ctx->allocs > 0 || (ctx->allocs == 0 && ctx->checksum == 0));
#endif // DBR_DEBUG_ALLOC
    }
}

DBR_EXTERN void
elm_ctx_free_matches(struct ElmCtx* ctx, struct DbrSlNode* first)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrMatch* match = dbr_trans_match_entry(node);
        node = node->next;
        // Not committed so match object still owns the trades.
        elm_ctx_free_trade(ctx, match->taker_trade);
        elm_ctx_free_trade(ctx, match->maker_trade);
        elm_ctx_free_match(ctx, match);
    }
}
