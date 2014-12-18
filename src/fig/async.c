/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "async.h"

#include <sc/ash/err.h>
#include <sc/elm/types.h>

#include <uuid/uuid.h>

#include <zmq.h>

SC_EXTERN void*
fig_async_create(void* zctx, const ScUuid uuid)
{
    void* sock = zmq_socket(zctx, ZMQ_REQ);
    if (sc_unlikely(!sock)) {
        sc_err_setf(SC_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    const int opt = 0;
    zmq_setsockopt(sock, ZMQ_LINGER, &opt, sizeof(opt));

    enum { INPROC_LEN = sizeof("inproc://") - 1 };
    char addr[INPROC_LEN + SC_UUID_MAX + 1];
    __builtin_memcpy(addr, "inproc://", INPROC_LEN);
    uuid_unparse_lower(uuid, addr + INPROC_LEN);

    if (sc_unlikely(zmq_connect(sock, addr) < 0)) {
        sc_err_setf(SC_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    return sock;
 fail2:
    zmq_close(sock);
 fail1:
    return NULL;
}

SC_EXTERN void
fig_async_destroy(void* sock)
{
    if (sock)
        zmq_close(sock);
}

SC_EXTERN ScBool
fig_async_send(void* sock, void* val)
{
    // FIXME: is this required with zmq_send?
    sc_wmb();
    if (sc_unlikely(zmq_send(sock, &val, sizeof(void*), 0) != sizeof(void*))) {
        sc_err_setf(SC_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        return SC_FALSE;
    }
    return SC_TRUE;
}

SC_EXTERN ScBool
fig_async_recv(void* sock, void** val)
{
    if (sc_unlikely(zmq_recv(sock, val, sizeof(void*), 0) != sizeof(void*))) {
        const int num = zmq_errno() == EINTR ? SC_EINTR : SC_EIO;
        sc_err_setf(num, "zmq_recv() failed: %s", zmq_strerror(zmq_errno()));
        return SC_FALSE;
    }
    // FIXME: is this required with zmq_recv?
    sc_rmb();
    return SC_TRUE;
}

SC_EXTERN ScBool
fig_async_close(void* sock)
{
    if (!fig_async_send(sock, FIG_ASYNC_CLOSE))
        return SC_FALSE;
    void* val;
    return fig_async_recv(sock, &val);
}

SC_API ScAsync
sc_async_create(void* zctx, const ScUuid uuid)
{
    void* sock = zmq_socket(zctx, ZMQ_REQ);
    if (sc_unlikely(!sock)) {
        sc_err_setf(SC_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    const int opt = 0;
    zmq_setsockopt(sock, ZMQ_LINGER, &opt, sizeof(opt));

    enum { INPROC_LEN = sizeof("inproc://") - 1 };
    char addr[INPROC_LEN + SC_UUID_MAX + 1];
    __builtin_memcpy(addr, "inproc://", INPROC_LEN);
    uuid_unparse_lower(uuid, addr + INPROC_LEN);

    if (sc_unlikely(zmq_connect(sock, addr) < 0)) {
        sc_err_setf(SC_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    return sock;
 fail2:
    zmq_close(sock);
 fail1:
    return NULL;
}

SC_API void
sc_async_destroy(ScAsync async)
{
    fig_async_destroy(async);
}

SC_API ScBool
sc_async_send(ScAsync async, void* val)
{
    return fig_async_send(async, val);
}

SC_API ScBool
sc_async_recv(ScAsync async, void** val)
{
    return fig_async_recv(async, val);
}
