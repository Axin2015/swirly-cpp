/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_FIG_ASYNC_H
#define DBR_FIG_ASYNC_H

#include <dbr/ash/defs.h>

/**
 * @addtogroup Async
 * @{
 */

typedef struct ElmAsync* DbrAsync;

DBR_API DbrAsync
dbr_async_create(void* zctx, const DbrUuid uuid);

DBR_API void
dbr_async_destroy(DbrAsync async);

DBR_API DbrBool
dbr_async_send(DbrAsync async, void* val);

DBR_API DbrBool
dbr_async_recv(DbrAsync async, void** val);

/** @} */

#endif // DBR_FIG_ASYNC_H
