/**
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
#include <dbr/ctx.h>

#include <elm/ctx.h>

#include <stdlib.h> // malloc()

DBR_API DbrCtx
dbr_ctx_create(long seedid)
{
    DbrCtx ctx = malloc(sizeof(struct ElmCtx));
    if (dbr_unlikely(!ctx))
        goto fail1;

    if (dbr_unlikely(!elm_ctx_init(ctx, seedid)))
        goto fail2;

    return ctx;
 fail2:
    free(ctx);
 fail1:
    return NULL;
}

DBR_API void
dbr_ctx_destroy(DbrCtx ctx)
{
    if (ctx) {
        elm_ctx_term(ctx);
        free(ctx);
    }
}

DBR_API long
dbr_ctx_next_id(DbrCtx ctx)
{
    return ctx->id++;
}

DBR_API struct DbrErr*
dbr_ctx_err(DbrCtx ctx)
{
    return &ctx->err;
}

DBR_API void
dbr_ctx_perror(DbrCtx ctx, FILE* stream, const char* s)
{
    fprintf(stream, "%s: %s (%d)\n", s, ctx->err.msg, ctx->err.num);
}

DBR_API struct DbrRec*
dbr_ctx_alloc_rec(DbrCtx ctx)
{
    return elm_ctx_alloc_rec(ctx);
}

DBR_API void
dbr_ctx_free_rec(DbrCtx ctx, struct DbrRec* rec)
{
    elm_ctx_free_rec(ctx, rec);
}

DBR_API struct DbrLevel*
dbr_ctx_alloc_level(DbrCtx ctx, DbrKey key)
{
    return elm_ctx_alloc_level(ctx, key);
}

DBR_API void
dbr_ctx_free_level(DbrCtx ctx, struct DbrLevel* level)
{
    elm_ctx_free_level(ctx, level);
}

DBR_API struct DbrMatch*
dbr_ctx_alloc_match(DbrCtx ctx)
{
    return elm_ctx_alloc_match(ctx);
}

DBR_API void
dbr_ctx_free_match(DbrCtx ctx, struct DbrMatch* match)
{
    elm_ctx_free_match(ctx, match);
}

DBR_API struct DbrOrder*
dbr_ctx_alloc_order(DbrCtx ctx, DbrKey key)
{
    return elm_ctx_alloc_order(ctx, key);
}

DBR_API void
dbr_ctx_free_order(DbrCtx ctx, struct DbrOrder* order)
{
    elm_ctx_free_order(ctx, order);
}

DBR_API struct DbrMemb*
dbr_ctx_alloc_memb(DbrCtx ctx, DbrKey key)
{
    return elm_ctx_alloc_memb(ctx, key);
}

DBR_API void
dbr_ctx_free_memb(DbrCtx ctx, struct DbrMemb* memb)
{
    elm_ctx_free_memb(ctx, memb);
}

DBR_API struct DbrTrade*
dbr_ctx_alloc_trade(DbrCtx ctx, DbrKey key)
{
    return elm_ctx_alloc_trade(ctx, key);
}

DBR_API void
dbr_ctx_free_trade(DbrCtx ctx, struct DbrTrade* trade)
{
    elm_ctx_free_trade(ctx, trade);
}

DBR_API struct DbrPosn*
dbr_ctx_alloc_posn(DbrCtx ctx, DbrKey key)
{
    return elm_ctx_alloc_posn(ctx, key);
}

DBR_API void
dbr_ctx_free_posn(DbrCtx ctx, struct DbrPosn* posn)
{
    elm_ctx_free_posn(ctx, posn);
}

DBR_API struct DbrSub*
dbr_ctx_alloc_sub(DbrCtx ctx, DbrKey key)
{
    return elm_ctx_alloc_sub(ctx, key);
}

DBR_API void
dbr_ctx_free_sub(DbrCtx ctx, struct DbrSub* sub)
{
    elm_ctx_free_sub(ctx, sub);
}
