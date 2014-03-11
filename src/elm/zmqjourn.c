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

#include "msg.h"
#include "pool.h"

#include <dbr/err.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strerror()

#include <pthread.h>
#include <zmq.h>

enum { HWM = 0 };

struct FirZmqJourn {
    void* sock;
    pthread_t thread;
    struct DbrIJourn i_journ;
};

DBR_EXTERN DbrBool
send_body(void* sock, struct DbrBody* body, DbrBool enriched)
{
    const size_t len = dbr_body_len(body, enriched);
    char buf[len];

    if (!elm_write_body(buf, body, enriched)) {
        dbr_err_set(DBR_EIO, "elm_write_body() failed");
        goto fail1;
    }
    if (zmq_send(sock, buf, len, ZMQ_DONTWAIT) != len) {
        dbr_err_setf(DBR_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

static inline struct FirZmqJourn*
journ_implof(DbrJourn journ)
{
    return dbr_implof(struct FirZmqJourn, i_journ, journ);
}

static void
destroy(DbrJourn journ)
{
    struct FirZmqJourn* impl = journ_implof(journ);

    for (;;) {
        // Send poison pill.
        struct DbrBody body = { .req_id = 0, .type = DBR_SESS_NOOP };
        if (dbr_likely(elm_send_body(impl->sock, &body, DBR_FALSE)))
            break;
        if (dbr_unlikely(dbr_err_num() != DBR_EINTR)) {
            dbr_err_print();
            break;
        }
        // DBR_EINTR
    }
    zmq_close(impl->sock);

    void* ret;
    const int err = pthread_join(impl->thread, &ret);
    if (dbr_unlikely(err)) {
        dbr_err_setf(DBR_EIO, "pthread_create() failed: %s", strerror(err));
        dbr_err_print();
    }
    free(impl);
}

static DbrBool
insert_exec_list(DbrJourn journ, struct DbrSlNode* first, DbrBool enriched)
{
    struct FirZmqJourn* impl = journ_implof(journ);
    struct DbrBody body = { .req_id = 0, .type = DBR_INSERT_EXEC_LIST_REQ,
                            .insert_exec_list_req = { .first = first, .count_ = 0 } };
    return send_body(impl->sock, &body, enriched);
}

static DbrBool
insert_exec(DbrJourn journ, struct DbrExec* exec, DbrBool enriched)
{
    struct FirZmqJourn* impl = journ_implof(journ);
    struct DbrBody body = { .req_id = 0, .type = DBR_INSERT_EXEC_REQ,
                            .insert_exec_req = { .exec = exec } };
    return send_body(impl->sock, &body, enriched);
}

static DbrBool
update_exec(DbrJourn journ, DbrIden id, DbrMillis modified)
{
    struct FirZmqJourn* impl = journ_implof(journ);
    struct DbrBody body = { .req_id = 0, .type = DBR_UPDATE_EXEC_REQ,
                            .update_exec_req = { .id = id, .modified = modified } };
    return send_body(impl->sock, &body, DBR_FALSE);
}

static const struct DbrJournVtbl JOURN_VTBL = {
    .destroy = destroy,
    .insert_exec_list = insert_exec_list,
    .insert_exec = insert_exec,
    .update_exec = update_exec
};

struct State {
    void* ctx;
    size_t capacity;
    DbrJourn (*factory)(void*);
    void* arg;
    DbrLogger logger;
    // Sizeof string literal includes null terminator.
    char addr[sizeof("inproc://0x0123456789ABCDEF")];
};

static void*
start_routine(void* arg)
{
    struct State* state = arg;

    // Inherit parent's logger.
    dbr_log_setlogger(state->logger);

    struct ElmPool pool;
    if (!elm_pool_init(&pool, 8 * 1024 * 1024))
        goto exit1;

    // The socket cannot be created in the parent. The zguide says:
    // Remember: Do not use or close sockets except in the thread that created them.

    void* sock = zmq_socket(state->ctx, ZMQ_PAIR);
    if (dbr_unlikely(!sock)) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        dbr_err_print();
        goto exit2;
    }

    const int hwm = HWM;
    if (dbr_unlikely(zmq_setsockopt(sock, ZMQ_SNDHWM, &hwm, sizeof(int))) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_setsockopt() failed: %s", zmq_strerror(zmq_errno()));
        goto exit3;
    }

    if (dbr_unlikely(zmq_connect(sock, state->addr) < 0)) {
        dbr_err_setf(DBR_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        dbr_err_print();
        goto exit3;
    }

    struct DbrBody body = { .req_id = 0, .type = DBR_STATUS_REP,
                            .status_rep = { .num = 0, .msg = "" } };
    DbrJourn journ = state->factory(state->arg);
    // Nullify parent pointer as precaution after use.
    state->arg = NULL;

    if (dbr_unlikely(!journ)) {
        dbr_err_prints("state->factory() failed");
        // Reply to parent with error status.
        body.status_rep.num = dbr_err_num();
        strncpy(body.status_rep.msg, dbr_err_msg(), DBR_ERRMSG_MAX);
        if (dbr_unlikely(!elm_send_body(sock, &body, DBR_FALSE)))
            dbr_err_print();
        goto exit3;
    }

    if (dbr_unlikely(!elm_send_body(sock, &body, DBR_FALSE))) {
        dbr_err_print();
        goto exit4;
    }

    for (;;) {
        if (dbr_unlikely(!elm_recv_body(sock, &pool, &body))) {
            if (dbr_err_num() == DBR_EINTR)
                continue;
            dbr_err_prints("elm_recv_body() failed");
            goto exit4;
        }
        switch (body.type) {
        case DBR_SESS_NOOP:
            // Poison pill.
            goto exit4;
        case DBR_INSERT_EXEC_LIST_REQ:
            if (!dbr_journ_insert_exec_list(journ, body.insert_exec_list_req.first, DBR_FALSE))
                dbr_err_print();
            elm_pool_free_entity_list(&pool, DBR_ENTITY_EXEC, body.insert_exec_list_req.first);
            break;
        case DBR_INSERT_EXEC_REQ:
            if (!dbr_journ_insert_exec(journ, body.insert_exec_req.exec, DBR_FALSE))
                dbr_err_print();
            elm_pool_free_exec(&pool, body.insert_exec_req.exec);
            break;
        case DBR_UPDATE_EXEC_REQ:
            if (!dbr_journ_update_exec(journ, body.update_exec_req.id,
                                       body.update_exec_req.modified))
                dbr_err_print();
            break;
        }
    }
 exit4:
    dbr_journ_destroy(journ);
 exit3:
    zmq_close(sock);
 exit2:
    elm_pool_term(&pool);
 exit1:
    free(state);
    dbr_log_info("exiting thread");
    return NULL;
}

DBR_API DbrJourn
dbr_zmqjourn_create(void* ctx, size_t capacity, DbrJourn (*factory)(void*), void* arg)
{
    struct FirZmqJourn* impl = malloc(sizeof(struct FirZmqJourn));
    if (dbr_unlikely(!impl)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    struct State* state = malloc(sizeof(struct State));
    if (dbr_unlikely(!state)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail2;
    }

    state->ctx = ctx;
    state->capacity = capacity;
    state->factory = factory;
    state->arg = arg;
    state->logger = dbr_log_logger();
    // Socket address is uniquely constructed from memory address.
    sprintf(state->addr, "inproc://%p", impl);

    void* sock = zmq_socket(ctx, ZMQ_PAIR);
    if (dbr_unlikely(!sock)) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }

    const int hwm = HWM;
    if (dbr_unlikely(zmq_setsockopt(sock, ZMQ_RCVHWM, &hwm, sizeof(int))) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_setsockopt() failed: %s", zmq_strerror(zmq_errno()));
        goto fail4;
    }

    if (dbr_unlikely(zmq_bind(sock, state->addr) < 0)) {
        dbr_err_setf(DBR_EIO, "zmq_bind() failed: %s", zmq_strerror(zmq_errno()));
        goto fail4;
    }

    pthread_t thread;
    const int err = pthread_create(&thread, NULL, start_routine, state);
    if (dbr_unlikely(err)) {
        dbr_err_setf(DBR_EIO, "pthread_create() failed: %s", strerror(err));
        goto fail4;
    }

    // Thread now owns state.
    state = NULL;

    for (;;) {
        // Noop does not allocate memory.
        DbrPool pool = NULL;
        struct DbrBody body;
        if (dbr_likely(elm_recv_body(sock, pool, &body))) {
            assert(body.type == DBR_STATUS_REP);
            // The factory function called by the thread may have failed.
            if (dbr_unlikely(body.status_rep.num != 0)) {
                dbr_err_set(body.status_rep.num, body.status_rep.msg);
                void* ret;
                pthread_join(thread, &ret);
                goto fail4;
            }
            break;
        }
        if (dbr_unlikely(dbr_err_num() != DBR_EINTR)) {
            // This unlikely failure scenario is not easily dealt with, so we simply abort.
            abort();
        }
        // DBR_EINTR
    }

    impl->sock = sock;
    impl->thread = thread;
    impl->i_journ.vtbl = &JOURN_VTBL;
    return &impl->i_journ;
 fail4:
    zmq_close(sock);
 fail3:
    free(state);
 fail2:
    free(impl);
 fail1:
    return NULL;
}
