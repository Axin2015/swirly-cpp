/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
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
#include <dbr/async.h>

#include <dbr/err.h>
#include <dbr/types.h>

#include <uuid/uuid.h>

#include <zmq.h>

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
    void* sock = async;
    if (async)
        zmq_close(sock);
}

DBR_API DbrBool
dbr_async_close(DbrAsync async)
{
    if (!dbr_async_send(async, DBR_ASYNC_CLOSE))
        return DBR_FALSE;
#if 0
    // Ignore reply.
    // FIXME: what if there is no reply?
    void* val;
    dbr_async_recv(async, &val);
#endif
    return DBR_TRUE;
}

DBR_API DbrBool
dbr_async_send(DbrAsync async, void* val)
{
    void* sock = async;

    // FIXME: is this required with zmq_send?
    dbr_wmb();
    if (dbr_unlikely(zmq_send(sock, &val, sizeof(void*), 0) != sizeof(void*))) {
        dbr_err_setf(DBR_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        return DBR_FALSE;
    }
    return DBR_TRUE;
}

DBR_API DbrBool
dbr_async_recv(DbrAsync async, void** val)
{
    void* sock = async;
    if (dbr_unlikely(zmq_recv(sock, val, sizeof(void*), 0) != sizeof(void*))) {
        const int num = zmq_errno() == EINTR ? DBR_EINTR : DBR_EIO;
        dbr_err_setf(num, "zmq_recv() failed: %s", zmq_strerror(zmq_errno()));
        return DBR_FALSE;
    }
    // FIXME: is this required with zmq_recv?
    dbr_rmb();
    return DBR_TRUE;
}
