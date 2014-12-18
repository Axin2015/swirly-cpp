/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef FIG_ASYNC_H
#define FIG_ASYNC_H

#include <sc/fig/async.h>

#define FIG_ASYNC_CLOSE ((void*)~0)

SC_EXTERN void*
fig_async_create(void* zctx, const ScUuid uuid);

SC_EXTERN void
fig_async_destroy(void* sock);

SC_EXTERN ScBool
fig_async_send(void* sock, void* val);

SC_EXTERN ScBool
fig_async_recv(void* sock, void** val);

SC_EXTERN ScBool
fig_async_close(void* sock);

#endif // FIG_ASYNC_H
