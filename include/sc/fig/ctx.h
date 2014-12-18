/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_FIG_CTX_H
#define SC_FIG_CTX_H

#include <sc/fig/async.h>
#include <sc/fig/handler.h>

#include <sc/elm/types.h>

/**
 * @addtogroup Ctx
 * @{
 */

typedef struct FigCtx* ScCtx;

SC_API ScCtx
sc_ctx_create(const char* mdaddr, const char* traddr, ScMillis tmout, size_t capacity,
               ScHandler handler);

SC_API void
sc_ctx_destroy(ScCtx ctx);

SC_API ScAsync
sc_ctx_async(ScCtx ctx);

SC_API const unsigned char*
sc_ctx_uuid(ScCtx ctx);

/** @} */

#endif // SC_FIG_CTX_H
