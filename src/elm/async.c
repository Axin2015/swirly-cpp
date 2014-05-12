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

#include <stdlib.h> // malloc()

#include <uuid/uuid.h>
#include <zmq.h>

struct ElmAsync {
    void* sock;
};

DBR_API DbrAsync
dbr_async_create(void* zctx, const DbrUuid uuid)
{
    DbrAsync async = malloc(sizeof(struct ElmAsync));
    if (dbr_unlikely(!async)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }
    void* sock = zmq_socket(zctx, ZMQ_REQ);
    if (dbr_unlikely(!sock)) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    const int opt = 0;
    zmq_setsockopt(sock, ZMQ_LINGER, &opt, sizeof(opt));

    enum { INPROC_LEN = sizeof("inproc://") - 1 };
    char addr[INPROC_LEN + DBR_UUID_MAX + 1];
    __builtin_memcpy(addr, "inproc://", INPROC_LEN);
    uuid_unparse_lower(uuid, addr + INPROC_LEN);

    if (dbr_unlikely(zmq_connect(sock, addr) < 0)) {
        dbr_err_setf(DBR_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }
    async->sock = sock;
    return async;
 fail3:
    zmq_close(sock);
 fail2:
    free(async);
 fail1:
    return NULL;
}

DBR_API void
dbr_async_destroy(DbrAsync async)
{
    if (async) {
        zmq_close(async->sock);
        free(async);
    }
}

DBR_API DbrBool
dbr_async_send(DbrAsync async, void* val)
{
    if (dbr_unlikely(zmq_send(async->sock, &val, sizeof(void*), 0) != sizeof(void*))) {
        dbr_err_setf(DBR_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        return DBR_FALSE;
    }
    return DBR_TRUE;
}

DBR_API DbrBool
dbr_async_recv(DbrAsync async, void** val)
{
    if (dbr_unlikely(zmq_recv(async->sock, val, sizeof(void*), 0) != sizeof(void*))) {
        const int num = zmq_errno() == EINTR ? DBR_EINTR : DBR_EIO;
        dbr_err_setf(num, "zmq_recv() failed: %s", zmq_strerror(zmq_errno()));
        return DBR_FALSE;
    }
    return DBR_TRUE;
}
