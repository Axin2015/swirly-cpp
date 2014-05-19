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
#include <dbr/refcount.h>
#include <dbr/log.h>
#include <dbr/pool.h>

#include <uuid/uuid.h>

#include <zmq.h>

#include <stdlib.h> // malloc()
#include <string.h> // strerror()

#include <pthread.h>

enum { HWM = 0 };

struct ZmqJourn {
    // Shared state.
    void* zctx;
    struct DbrIJourn i_journ;

    // Thread state confined to parent.
    void* sock;
    pthread_t child;
};

static DbrBool
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

static inline struct ZmqJourn*
journ_implof(DbrJourn journ)
{
    return dbr_implof(struct ZmqJourn, i_journ, journ);
}

static void
destroy(DbrJourn journ)
{
    struct ZmqJourn* impl = journ_implof(journ);

    for (;;) {
        // Send poison pill.
        struct DbrBody body = { .req_id = 0, .type = DBR_SESS_CLOSE };
        if (dbr_likely(elm_send_body(impl->sock, &body, DBR_FALSE)))
            break;
        if (dbr_unlikely(dbr_err_num() != DBR_EINTR)) {
            dbr_err_perror("elm_send_body() failed");
            break;
        }
        // DBR_EINTR
    }
    zmq_close(impl->sock);

    void* ret;
    const int err = pthread_join(impl->child, &ret);
    if (dbr_unlikely(err)) {
        dbr_err_printf(DBR_ESYSTEM, "pthread_join() failed: %s", strerror(err));
    }
    free(impl);
}

static DbrBool
insert_exec_list(DbrJourn journ, struct DbrSlNode* first, DbrBool enriched)
{
    struct ZmqJourn* impl = journ_implof(journ);
    struct DbrBody body = { .req_id = 0, .type = DBR_INSERT_EXEC_LIST_REQ,
                            .insert_exec_list_req = { .first = first, .count_ = 0 } };
    return send_body(impl->sock, &body, enriched);
}

static DbrBool
insert_exec(DbrJourn journ, struct DbrExec* exec, DbrBool enriched)
{
    struct ZmqJourn* impl = journ_implof(journ);
    struct DbrBody body = { .req_id = 0, .type = DBR_INSERT_EXEC_REQ,
                            .insert_exec_req = { .exec = exec } };
    return send_body(impl->sock, &body, enriched);
}

static DbrBool
update_exec(DbrJourn journ, DbrIden id, DbrMillis modified)
{
    struct ZmqJourn* impl = journ_implof(journ);
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

enum { INIT = -1, SUCCESS = 0, FAILURE = 1 };
enum { INPROC_LEN = sizeof("inproc://") - 1 };

struct Init {

    size_t capacity;
    DbrJourn (*factory)(void*);
    void* arg;

    int level;
    DbrLogger logger;

    struct ZmqJourn* impl;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char addr[INPROC_LEN + DBR_UUID_MAX + 1];
    int state;
    int err_num;
    char err_msg[DBR_ERRMSG_MAX];
};

static void*
start_routine(void* arg)
{
    enum { TMOUT = 5000 };

    struct Init* init = arg;
    struct ZmqJourn* impl = init->impl;

    // Inherit parent's level and logger.
    dbr_log_setlevel(init->level);
    dbr_log_setlogger(init->logger);

    struct ElmPool pool;
    if (!elm_pool_init(&pool, 8 * 1024 * 1024))
        goto fail1;

    DbrJourn journ = init->factory(init->arg);
    if (!journ)
        goto fail2;

    // The socket cannot be created in the parent. The zguide says:
    // Remember: Do not use or close sockets except in the thread that created them.

    void* sock = zmq_socket(impl->zctx, ZMQ_PAIR);
    if (dbr_unlikely(!sock)) {
        dbr_err_printf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }
    const int opt = 0;
    zmq_setsockopt(sock, ZMQ_LINGER, &opt, sizeof(opt));

    const int hwm = HWM;
    if (dbr_unlikely(zmq_setsockopt(sock, ZMQ_SNDHWM, &hwm, sizeof(int))) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_setsockopt() failed: %s", zmq_strerror(zmq_errno()));
        goto fail4;
    }

    if (dbr_unlikely(zmq_connect(sock, init->addr) < 0)) {
        dbr_err_printf(DBR_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail4;
    }

    pthread_mutex_lock(&init->mutex);
    init->state = SUCCESS;
    pthread_cond_signal(&init->cond);
    pthread_mutex_unlock(&init->mutex);
    // The init pointer is left dangling beyond this point.
    // Fail-fast if accessed due to logic-error.
    init = NULL;

    for (;;) {
        struct DbrBody body;
        if (dbr_unlikely(!elm_recv_body(sock, &pool, &body))) {
            if (dbr_err_num() == DBR_EINTR) {
                dbr_log_warn("thread interrupted");
                continue;
            }
            dbr_err_perror("elm_recv_body() failed");
            goto done;
        }
        switch (body.type) {
        case DBR_SESS_CLOSE:
            // Poison pill.
            goto done;
        case DBR_INSERT_EXEC_LIST_REQ:
            if (!dbr_journ_insert_exec_list(journ, body.insert_exec_list_req.first, DBR_FALSE))
                dbr_err_perror("dbr_journ_insert_exec_list() failed");
            elm_pool_free_entity_list(&pool, DBR_ENTITY_EXEC, body.insert_exec_list_req.first);
            break;
        case DBR_INSERT_EXEC_REQ:
            if (!dbr_journ_insert_exec(journ, body.insert_exec_req.exec, DBR_FALSE))
                dbr_err_perror("dbr_journ_insert_exec() failed");
            dbr_exec_decref(body.insert_exec_req.exec, &pool);
            break;
        case DBR_UPDATE_EXEC_REQ:
            if (!dbr_journ_update_exec(journ, body.update_exec_req.id,
                                       body.update_exec_req.modified))
                dbr_err_perror("dbr_journ_update_exec() failed");
            break;
        }
    }
 done:
    dbr_log_info("exiting thread");
    zmq_close(sock);
    dbr_journ_destroy(journ);
    elm_pool_term(&pool);
    return NULL;
 fail4:
    zmq_close(sock);
 fail3:
    dbr_journ_destroy(journ);
 fail2:
    elm_pool_term(&pool);
 fail1:
    init->err_num = dbr_err_num();
    strncpy(init->err_msg, dbr_err_msg(), DBR_ERRMSG_MAX);

    pthread_mutex_lock(&init->mutex);
    init->state = FAILURE;
    pthread_cond_signal(&init->cond);
    pthread_mutex_unlock(&init->mutex);
    return NULL;
}

DBR_API DbrJourn
dbr_zmqjourn_create(void* zctx, size_t capacity, DbrJourn (*factory)(void*), void* arg)
{
    struct ZmqJourn* impl = malloc(sizeof(struct ZmqJourn));
    if (dbr_unlikely(!impl)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    impl->zctx = zctx;
    impl->i_journ.vtbl = &JOURN_VTBL;

    struct Init init = {

        .capacity = capacity,
        .factory = factory,
        .arg = arg,

        .level = dbr_log_level(),
        .logger = dbr_log_logger(),

        .impl = impl,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .cond = PTHREAD_COND_INITIALIZER,
        .addr = { 0 },
        .state = INIT,
        .err_num = 0,
        .err_msg = ""
    };

    DbrUuid uuid;
    uuid_generate(uuid);
    __builtin_memcpy(init.addr, "inproc://", INPROC_LEN);
    uuid_unparse_lower(uuid, init.addr + INPROC_LEN);

    impl->sock = zmq_socket(zctx, ZMQ_PAIR);
    if (dbr_unlikely(!impl->sock)) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    const int opt = 0;
    zmq_setsockopt(impl->sock, ZMQ_LINGER, &opt, sizeof(opt));

    const int hwm = HWM;
    if (dbr_unlikely(zmq_setsockopt(impl->sock, ZMQ_RCVHWM, &hwm, sizeof(int))) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_setsockopt() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }

    if (dbr_unlikely(zmq_bind(impl->sock, init.addr) < 0)) {
        dbr_err_setf(DBR_EIO, "zmq_bind() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }

    const int err = pthread_create(&impl->child, NULL, start_routine, &init);
    if (err) {
        dbr_err_setf(DBR_ESYSTEM, "pthread_create() failed: %s", strerror(err));
        goto fail3;
    }

    pthread_mutex_lock(&init.mutex);
    while (init.state == INIT)
        pthread_cond_wait(&init.cond, &init.mutex);
    pthread_mutex_unlock(&init.mutex);

    if (init.state == FAILURE) {
        dbr_err_set(init.err_num, init.err_msg);
        goto fail4;
    }

    pthread_cond_destroy(&init.cond);
    pthread_mutex_destroy(&init.mutex);
    return &impl->i_journ;
 fail4:
    {
        void* ret;
        const int err = pthread_join(impl->child, &ret);
        if (err)
            dbr_err_printf(DBR_ESYSTEM, "pthread_create() failed: %s", strerror(err));
    }
 fail3:
    zmq_close(impl->sock);
 fail2:
    pthread_cond_destroy(&init.cond);
    pthread_mutex_destroy(&init.mutex);
    free(impl);
 fail1:
    return NULL;
}
