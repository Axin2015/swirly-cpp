/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/elm/zmqjourn.h>

#include "msg.h"
#include "pool.h"

#include <sc/elm/pool.h>
#include <sc/elm/refcount.h>

#include <sc/ash/err.h>
#include <sc/ash/log.h>

#include <uuid/uuid.h>

#include <zmq.h>

#include <stdlib.h> // malloc()
#include <string.h> // strerror()

#include <pthread.h>

enum { HWM = 0 };

struct ZmqJourn {
    // Shared state.
    void* zctx;
    struct ScIJourn i_journ;

    // Thread state confined to parent.
    void* sock;
    pthread_t child;
};

static ScBool
send_body(void* sock, struct ScBody* body, ScBool enriched)
{
    const size_t len = sc_body_len(body, enriched);
    char buf[len];

    if (!elm_write_body(buf, body, enriched)) {
        sc_err_set(SC_EIO, "elm_write_body() failed");
        goto fail1;
    }
    if (zmq_send(sock, buf, len, ZMQ_DONTWAIT) != len) {
        sc_err_setf(SC_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        goto fail1;
    }
    return SC_TRUE;
 fail1:
    return SC_FALSE;
}

static inline struct ZmqJourn*
journ_implof(ScJourn journ)
{
    return sc_implof(struct ZmqJourn, i_journ, journ);
}

static void
destroy(ScJourn journ)
{
    struct ZmqJourn* impl = journ_implof(journ);

    for (;;) {
        // Send poison pill.
        struct ScBody body = { .req_id = 0, .sid = 0, .type = SC_SESS_CLOSE };
        if (sc_likely(elm_send_body(impl->sock, &body, SC_FALSE)))
            break;
        if (sc_unlikely(sc_err_num() != SC_EINTR)) {
            sc_err_perror("elm_send_body() failed");
            break;
        }
        // SC_EINTR
    }
    zmq_close(impl->sock);

    void* ret;
    const int err = pthread_join(impl->child, &ret);
    if (sc_unlikely(err)) {
        sc_err_printf(SC_ESYSTEM, "pthread_join() failed: %s", strerror(err));
    }
    free(impl);
}

static ScBool
insert_exec_list(ScJourn journ, struct ScSlNode* first, ScBool enriched)
{
    struct ZmqJourn* impl = journ_implof(journ);
    struct ScBody body = { .req_id = 0, .sid = 0, .type = SC_INSERT_EXEC_LIST_REQ,
                            .insert_exec_list_req = { .first = first, .count_ = 0 } };
    return send_body(impl->sock, &body, enriched);
}

static ScBool
insert_exec(ScJourn journ, struct ScExec* exec, ScBool enriched)
{
    struct ZmqJourn* impl = journ_implof(journ);
    struct ScBody body = { .req_id = 0, .sid = 0, .type = SC_INSERT_EXEC_REQ,
                            .insert_exec_req = { .exec = exec } };
    return send_body(impl->sock, &body, enriched);
}

static ScBool
update_exec(ScJourn journ, ScIden id, ScMillis modified)
{
    struct ZmqJourn* impl = journ_implof(journ);
    struct ScBody body = { .req_id = 0, .sid = 0, .type = SC_UPDATE_EXEC_REQ,
                            .update_exec_req = { .id = id, .modified = modified } };
    return send_body(impl->sock, &body, SC_FALSE);
}

static const struct ScJournVtbl JOURN_VTBL = {
    .destroy = destroy,
    .insert_exec_list = insert_exec_list,
    .insert_exec = insert_exec,
    .update_exec = update_exec
};

enum { INIT = -1, SUCCESS = 0, FAILURE = 1 };
enum { INPROC_LEN = sizeof("inproc://") - 1 };

struct Init {

    size_t capacity;
    ScJourn (*factory)(void*);
    void* arg;

    int level;
    ScLogger logger;

    struct ZmqJourn* impl;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char addr[INPROC_LEN + SC_UUID_MAX + 1];
    int state;
    int err_num;
    char err_msg[SC_ERRMSG_MAX];
};

static void*
start_routine(void* arg)
{
    enum { TMOUT = 5000 };

    struct Init* init = arg;
    struct ZmqJourn* impl = init->impl;

    // Inherit parent's level and logger.
    sc_log_setlevel(init->level);
    sc_log_setlogger(init->logger);

    struct ElmPool pool;
    if (!elm_pool_init(&pool, 8 * 1024 * 1024))
        goto fail1;

    ScJourn journ = init->factory(init->arg);
    if (!journ)
        goto fail2;

    // The socket cannot be created in the parent. The zguide says:
    // Remember: Do not use or close sockets except in the thread that created them.

    void* sock = zmq_socket(impl->zctx, ZMQ_PAIR);
    if (sc_unlikely(!sock)) {
        sc_err_printf(SC_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }
    const int opt = 0;
    zmq_setsockopt(sock, ZMQ_LINGER, &opt, sizeof(opt));

    const int hwm = HWM;
    if (sc_unlikely(zmq_setsockopt(sock, ZMQ_SNDHWM, &hwm, sizeof(int))) < 0) {
        sc_err_setf(SC_EIO, "zmq_setsockopt() failed: %s", zmq_strerror(zmq_errno()));
        goto fail4;
    }

    if (sc_unlikely(zmq_connect(sock, init->addr) < 0)) {
        sc_err_printf(SC_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
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
        struct ScBody body;
        if (sc_unlikely(!elm_recv_body(sock, &pool, &body))) {
            if (sc_err_num() == SC_EINTR) {
                sc_log_warn("thread interrupted");
                continue;
            }
            sc_err_perror("elm_recv_body() failed");
            goto done;
        }
        switch (body.type) {
        case SC_SESS_CLOSE:
            // Poison pill.
            goto done;
        case SC_INSERT_EXEC_LIST_REQ:
            if (!sc_journ_insert_exec_list(journ, body.insert_exec_list_req.first, SC_FALSE))
                sc_err_perror("sc_journ_insert_exec_list() failed");
            elm_pool_free_entity_list(&pool, SC_ENTITY_EXEC, body.insert_exec_list_req.first);
            break;
        case SC_INSERT_EXEC_REQ:
            if (!sc_journ_insert_exec(journ, body.insert_exec_req.exec, SC_FALSE))
                sc_err_perror("sc_journ_insert_exec() failed");
            sc_exec_decref(body.insert_exec_req.exec, &pool);
            break;
        case SC_UPDATE_EXEC_REQ:
            if (!sc_journ_update_exec(journ, body.update_exec_req.id,
                                       body.update_exec_req.modified))
                sc_err_perror("sc_journ_update_exec() failed");
            break;
        }
    }
 done:
    sc_log_info("exiting thread");
    zmq_close(sock);
    sc_journ_destroy(journ);
    elm_pool_term(&pool);
    return NULL;
 fail4:
    zmq_close(sock);
 fail3:
    sc_journ_destroy(journ);
 fail2:
    elm_pool_term(&pool);
 fail1:
    init->err_num = sc_err_num();
    strncpy(init->err_msg, sc_err_msg(), SC_ERRMSG_MAX);

    pthread_mutex_lock(&init->mutex);
    init->state = FAILURE;
    pthread_cond_signal(&init->cond);
    pthread_mutex_unlock(&init->mutex);
    return NULL;
}

SC_API ScJourn
sc_zmqjourn_create(void* zctx, size_t capacity, ScJourn (*factory)(void*), void* arg)
{
    struct ZmqJourn* impl = malloc(sizeof(struct ZmqJourn));
    if (sc_unlikely(!impl)) {
        sc_err_set(SC_ENOMEM, "out of memory");
        goto fail1;
    }

    impl->zctx = zctx;
    impl->i_journ.vtbl = &JOURN_VTBL;

    struct Init init = {

        .capacity = capacity,
        .factory = factory,
        .arg = arg,

        .level = sc_log_level(),
        .logger = sc_log_logger(),

        .impl = impl,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .cond = PTHREAD_COND_INITIALIZER,
        .addr = { 0 },
        .state = INIT,
        .err_num = 0,
        .err_msg = ""
    };

    ScUuid uuid;
    uuid_generate(uuid);
    __builtin_memcpy(init.addr, "inproc://", INPROC_LEN);
    uuid_unparse_lower(uuid, init.addr + INPROC_LEN);

    impl->sock = zmq_socket(zctx, ZMQ_PAIR);
    if (sc_unlikely(!impl->sock)) {
        sc_err_setf(SC_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    const int opt = 0;
    zmq_setsockopt(impl->sock, ZMQ_LINGER, &opt, sizeof(opt));

    const int hwm = HWM;
    if (sc_unlikely(zmq_setsockopt(impl->sock, ZMQ_RCVHWM, &hwm, sizeof(int))) < 0) {
        sc_err_setf(SC_EIO, "zmq_setsockopt() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }

    if (sc_unlikely(zmq_bind(impl->sock, init.addr) < 0)) {
        sc_err_setf(SC_EIO, "zmq_bind() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }

    const int err = pthread_create(&impl->child, NULL, start_routine, &init);
    if (err) {
        sc_err_setf(SC_ESYSTEM, "pthread_create() failed: %s", strerror(err));
        goto fail3;
    }

    pthread_mutex_lock(&init.mutex);
    while (init.state == INIT)
        pthread_cond_wait(&init.cond, &init.mutex);
    pthread_mutex_unlock(&init.mutex);

    if (init.state == FAILURE) {
        sc_err_set(init.err_num, init.err_msg);
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
            sc_err_printf(SC_ESYSTEM, "pthread_create() failed: %s", strerror(err));
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
