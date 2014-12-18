/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_FIG_CTX_H
#define DBR_FIG_CTX_H

#include <dbr/fig/async.h>
#include <dbr/fig/handler.h>

#include <dbr/elm/types.h>

/**
 * @addtogroup Ctx
 * @{
 */

typedef struct FigCtx* DbrCtx;

DBR_API DbrCtx
dbr_ctx_create(const char* mdaddr, const char* traddr, DbrMillis tmout, size_t capacity,
               DbrHandler handler);

DBR_API void
dbr_ctx_destroy(DbrCtx ctx);

DBR_API DbrAsync
dbr_ctx_async(DbrCtx ctx);

DBR_API const unsigned char*
dbr_ctx_uuid(DbrCtx ctx);

/** @} */

#endif // DBR_FIG_CTX_H
