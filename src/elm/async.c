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

#include <zmq.h>

#include <stdlib.h> // malloc()

struct ElmAsync {
    void* sock;
};

DBR_API DbrAsync
dbr_async_create(void* ctx, const char* sess)
{
    DbrAsync async = malloc(sizeof(struct ElmAsync));
    if (dbr_unlikely(!async)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }
    void* sock = zmq_socket(ctx, ZMQ_REQ);
    if (!sock) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    // Sizeof string literal includes null terminator.
    char addr[sizeof("inproc://") + DBR_MNEM_MAX];
    sprintf(addr, "inproc://%.16s", sess);
    if (zmq_connect(sock, addr) < 0) {
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
dbr_async_send(DbrAsync async, void* arg)
{
    if (zmq_send(async->sock, &arg, sizeof(void*), 0) != sizeof(void*)) {
        dbr_err_setf(DBR_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        return DBR_FALSE;
    }
    return DBR_TRUE;
}

DBR_API DbrBool
dbr_async_recv(DbrAsync async, void** arg)
{
    if (zmq_recv(async->sock, arg, sizeof(void*), 0) != sizeof(void*)) {
        const int num = zmq_errno() == EINTR ? DBR_EINTR : DBR_EIO;
        dbr_err_setf(num, "zmq_msg_recv() failed: %s", zmq_strerror(zmq_errno()));
        return DBR_FALSE;
    }
    return DBR_TRUE;
}
