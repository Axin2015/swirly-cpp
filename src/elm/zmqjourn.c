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
#include <dbr/zmqjourn.h>

#include <dbr/err.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strerror()

#include <pthread.h>
#include <zmq.h>

struct FirZmqJourn {
    void* sock;
    pthread_t thread;
    struct DbrIJourn i_journ;
};

static inline struct FirZmqJourn*
journ_implof(DbrJourn journ)
{
    return dbr_implof(struct FirZmqJourn, i_journ, journ);
}

static void
destroy(DbrJourn journ)
{
    struct FirZmqJourn* impl = journ_implof(journ);

    zmq_close(impl->sock);

    void* retval;
    const int err = pthread_join(impl->thread, &retval);
    if (dbr_unlikely(err)) {
        dbr_err_setf(DBR_EIO, "pthread_create() failed: %s", strerror(err));
        dbr_err_print();
    }
    free(impl);
}

static DbrBool
insert_exec_list(DbrJourn journ, struct DbrSlNode* first, DbrBool enriched)
{
    return DBR_TRUE;
}

static DbrBool
insert_exec(DbrJourn journ, struct DbrExec* exec, DbrBool enriched)
{
    return DBR_TRUE;
}

static DbrBool
update_exec(DbrJourn journ, DbrIden id, DbrMillis modified)
{
    return DBR_TRUE;
}

static const struct DbrJournVtbl JOURN_VTBL = {
    .destroy = destroy,
    .insert_exec_list = insert_exec_list,
    .insert_exec = insert_exec,
    .update_exec = update_exec
};

struct State {
    void* ctx;
    // Sizeof string literal includes null terminator.
    char addr[sizeof("inproc://0x0123456789ABCDEF")];
};

static void*
start_routine(void* arg)
{
    struct State* state = arg;

    // The socket cannot be created in the parent. The zguide says:
    // Remember: Do not use or close sockets except in the thread that created them.

    void* sock = zmq_socket(state->ctx, ZMQ_PAIR);
    if (dbr_unlikely(!sock)) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        dbr_err_print();
        goto exit1;
    }

    if (dbr_unlikely(zmq_connect(sock, state->addr) < 0)) {
        dbr_err_setf(DBR_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        dbr_err_print();
        goto exit2;
    }

    if (dbr_unlikely(zmq_send(sock, "", 1, 0) != 1)) {
        dbr_err_setf(DBR_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        dbr_err_print();
        goto exit2;
    }

    // State is now a dangling pointer.

 exit2:
    zmq_close(sock);
 exit1:
    return NULL;
}

DBR_API DbrJourn
dbr_zmqjourn_create(void* ctx, DbrJourn (*factory)(void*), void* arg)
{
    struct FirZmqJourn* impl = malloc(sizeof(struct FirZmqJourn));
    if (dbr_unlikely(!impl)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    void* sock = zmq_socket(ctx, ZMQ_PAIR);
    if (dbr_unlikely(!sock)) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }

    struct State state;
    state.ctx = ctx;
    sprintf(state.addr, "inproc://%p", impl);

    if (dbr_unlikely(zmq_bind(sock, state.addr) < 0)) {
        dbr_err_setf(DBR_EIO, "zmq_bind() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }

    pthread_t thread;
    const int err = pthread_create(&thread, NULL, start_routine, &state);
    if (dbr_unlikely(err)) {
        dbr_err_setf(DBR_EIO, "pthread_create() failed: %s", strerror(err));
        goto fail3;
    }

    for (;;) {
        char buf[1];
        if (dbr_likely(zmq_recv(sock, buf, 1, 0) == 1))
            break;
        if (zmq_errno() == EINTR)
            continue;
        abort();
    }

    impl->sock = sock;
    impl->thread = thread;
    impl->i_journ.vtbl = &JOURN_VTBL;
    return &impl->i_journ;
 fail3:
    zmq_close(sock);
 fail2:
    free(impl);
 fail1:
    return NULL;
}
