/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_FIG_ASYNC_H
#define SC_FIG_ASYNC_H

#include <sc/ash/defs.h>

/**
 * @addtogroup Async
 * @{
 */

typedef struct ElmAsync* ScAsync;

SC_API ScAsync
sc_async_create(void* zctx, const ScUuid uuid);

SC_API void
sc_async_destroy(ScAsync async);

SC_API ScBool
sc_async_send(ScAsync async, void* val);

SC_API ScBool
sc_async_recv(ScAsync async, void** val);

/** @} */

#endif // SC_FIG_ASYNC_H
