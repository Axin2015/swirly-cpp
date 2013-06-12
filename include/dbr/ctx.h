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
#ifndef DBR_CTX_H
#define DBR_CTX_H

#include <dbr/defs.h>
#include <dbr/node.h>

/**
 * @addtogroup Ctx
 * @{
 */

typedef struct ElmCtx* DbrCtx;

DBR_API DbrCtx
dbr_ctx_create(void);

DBR_API void
dbr_ctx_destroy(DbrCtx ctx);

/** @} */

/**
 * @addtogroup Alloc
 * @{
 */

DBR_API struct DbrRec*
dbr_ctx_alloc_rec(DbrCtx ctx);

DBR_API void
dbr_ctx_free_rec(DbrCtx ctx, struct DbrRec* rec);

DBR_API struct DbrLevel*
dbr_ctx_alloc_level(DbrCtx ctx, DbrKey key);

DBR_API void
dbr_ctx_free_level(DbrCtx ctx, struct DbrLevel* level);

DBR_API struct DbrMatch*
dbr_ctx_alloc_match(DbrCtx ctx);

DBR_API void
dbr_ctx_free_match(DbrCtx ctx, struct DbrMatch* match);

DBR_API struct DbrOrder*
dbr_ctx_alloc_order(DbrCtx ctx, DbrKey key);

DBR_API void
dbr_ctx_free_order(DbrCtx ctx, struct DbrOrder* order);

DBR_API struct DbrMemb*
dbr_ctx_alloc_memb(DbrCtx ctx, DbrKey key);

DBR_API void
dbr_ctx_free_memb(DbrCtx ctx, struct DbrMemb* memb);

DBR_API struct DbrTrade*
dbr_ctx_alloc_trade(DbrCtx ctx, DbrKey key);

DBR_API void
dbr_ctx_free_trade(DbrCtx ctx, struct DbrTrade* trade);

DBR_API struct DbrPosn*
dbr_ctx_alloc_posn(DbrCtx ctx, DbrKey key);

DBR_API void
dbr_ctx_free_posn(DbrCtx ctx, struct DbrPosn* posn);

DBR_API struct DbrSub*
dbr_ctx_alloc_sub(DbrCtx ctx, DbrKey key);

DBR_API void
dbr_ctx_free_sub(DbrCtx ctx, struct DbrSub* sub);

/** @} */

#endif // DBR_CTX_H
