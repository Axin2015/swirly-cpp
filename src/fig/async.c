/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "async.h"

#include <dbr/ash/err.h>
#include <dbr/elm/types.h>

#include <uuid/uuid.h>

#include <zmq.h>

DBR_EXTERN void*
fig_async_create(void* zctx, const DbrUuid uuid)
{
    void* sock = zmq_socket(zctx, ZMQ_REQ);
    if (dbr_unlikely(!sock)) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    const int opt = 0;
    zmq_setsockopt(sock, ZMQ_LINGER, &opt, sizeof(opt));

    enum { INPROC_LEN = sizeof("inproc://") - 1 };
    char addr[INPROC_LEN + DBR_UUID_MAX + 1];
    __builtin_memcpy(addr, "inproc://", INPROC_LEN);
    uuid_unparse_lower(uuid, addr + INPROC_LEN);

    if (dbr_unlikely(zmq_connect(sock, addr) < 0)) {
        dbr_err_setf(DBR_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    return sock;
 fail2:
    zmq_close(sock);
 fail1:
    return NULL;
}

DBR_EXTERN void
fig_async_destroy(void* sock)
{
    if (sock)
        zmq_close(sock);
}

DBR_EXTERN DbrBool
fig_async_send(void* sock, void* val)
{
    // FIXME: is this required with zmq_send?
    dbr_wmb();
    if (dbr_unlikely(zmq_send(sock, &val, sizeof(void*), 0) != sizeof(void*))) {
        dbr_err_setf(DBR_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        return DBR_FALSE;
    }
    return DBR_TRUE;
}

DBR_EXTERN DbrBool
fig_async_recv(void* sock, void** val)
{
    if (dbr_unlikely(zmq_recv(sock, val, sizeof(void*), 0) != sizeof(void*))) {
        const int num = zmq_errno() == EINTR ? DBR_EINTR : DBR_EIO;
        dbr_err_setf(num, "zmq_recv() failed: %s", zmq_strerror(zmq_errno()));
        return DBR_FALSE;
    }
    // FIXME: is this required with zmq_recv?
    dbr_rmb();
    return DBR_TRUE;
}

DBR_EXTERN DbrBool
fig_async_close(void* sock)
{
    if (!fig_async_send(sock, FIG_ASYNC_CLOSE))
        return DBR_FALSE;
    void* val;
    return fig_async_recv(sock, &val);
}

DBR_API DbrAsync
dbr_async_create(void* zctx, const DbrUuid uuid)
{
    void* sock = zmq_socket(zctx, ZMQ_REQ);
    if (dbr_unlikely(!sock)) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    const int opt = 0;
    zmq_setsockopt(sock, ZMQ_LINGER, &opt, sizeof(opt));

    enum { INPROC_LEN = sizeof("inproc://") - 1 };
    char addr[INPROC_LEN + DBR_UUID_MAX + 1];
    __builtin_memcpy(addr, "inproc://", INPROC_LEN);
    uuid_unparse_lower(uuid, addr + INPROC_LEN);

    if (dbr_unlikely(zmq_connect(sock, addr) < 0)) {
        dbr_err_setf(DBR_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    return sock;
 fail2:
    zmq_close(sock);
 fail1:
    return NULL;
}

DBR_API void
dbr_async_destroy(DbrAsync async)
{
    fig_async_destroy(async);
}

DBR_API DbrBool
dbr_async_send(DbrAsync async, void* val)
{
    return fig_async_send(async, val);
}

DBR_API DbrBool
dbr_async_recv(DbrAsync async, void** val)
{
    return fig_async_recv(async, val);
}
