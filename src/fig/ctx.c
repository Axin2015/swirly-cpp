/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/fig/ctx.h>

#include "async.h"

#include <sc/fig/clnt.h>
#include <sc/fig/dispatch.h>

#include <sc/ash/err.h>
#include <sc/ash/log.h>
#include <sc/ash/util.h>

#include <uuid/uuid.h>

#include <zmq.h>

#include <stdlib.h> // malloc()
#include <string.h> // strerror()

#include <pthread.h>

struct FigCtx {
    // Shared state.
    void* zctx;
    ScUuid uuid;
    // Thread state confined to parent.
    pthread_t child;
};

enum { ADDR_MAX = 128 };
enum { INIT = -1, SUCCESS = 0, FAILURE = 1 };

struct Init {

    char mdaddr[ADDR_MAX + 1];
    char traddr[ADDR_MAX + 1];
    ScMillis tmout;
    size_t capacity;
    ScHandler handler;

    int level;
    ScLogger logger;

    ScCtx ctx;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int state;
    int err_num;
    char err_msg[SC_ERRMSG_MAX];
};

static void*
start_routine(void* arg)
{
    enum { TMOUT = 5000 };

    struct Init* init = arg;
    ScCtx ctx = init->ctx;

    // Inherit parent's level and logger.
    sc_log_setlevel(init->level);
    sc_log_setlogger(init->logger);

    ScPool pool = sc_pool_create(init->capacity);
    if (!pool)
        goto fail1;

    ScClnt clnt = sc_clnt_create(ctx->zctx, ctx->uuid, init->mdaddr, init->traddr,
                                   init->tmout, pool);
    if (!clnt)
        goto fail2;

    ScHandler handler = init->handler;

    pthread_mutex_lock(&init->mutex);
    init->state = SUCCESS;
    pthread_cond_signal(&init->cond);
    pthread_mutex_unlock(&init->mutex);
    // The init pointer is left dangling beyond this point.
    // Fail-fast if accessed due to logic-error.
    init = NULL;

    // Dispatch loop.
    while (!sc_clnt_is_closed(clnt)) {
        if (!sc_clnt_dispatch(clnt, TMOUT, handler))
            sc_err_perror("sc_clnt_dispatch() failed");
    }
    sc_log_info("exiting thread");

    sc_clnt_destroy(clnt);
    sc_pool_destroy(pool);
    return NULL;
 fail2:
    sc_pool_destroy(pool);
 fail1:
    init->err_num = sc_err_num();
    strncpy(init->err_msg, sc_err_msg(), SC_ERRMSG_MAX);

    pthread_mutex_lock(&init->mutex);
    init->state = FAILURE;
    pthread_cond_signal(&init->cond);
    pthread_mutex_unlock(&init->mutex);
    return NULL;
}

SC_API ScCtx
sc_ctx_create(const char* mdaddr, const char* traddr, ScMillis tmout, size_t capacity,
               ScHandler handler)
{
    ScCtx ctx = malloc(sizeof(struct FigCtx));
    if (sc_unlikely(!ctx)) {
        sc_err_set(SC_ENOMEM, "out of memory");
        goto fail1;
    }

    if (!(ctx->zctx = zmq_ctx_new())) {
        sc_err_setf(SC_EIO, "zmq_ctx_new() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    uuid_generate(ctx->uuid);

    struct Init init = {

        .mdaddr = { 0 },
        .traddr = { 0 },
        .tmout = tmout,
        .capacity = capacity,
        .handler = handler,

        .level = sc_log_level(),
        .logger = sc_log_logger(),

        .ctx = ctx,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .cond = PTHREAD_COND_INITIALIZER,
        .state = INIT,
        .err_num = 0,
        .err_msg = ""
    };

    // Null terminated by initialiser.
    strncpy(init.mdaddr, mdaddr, ADDR_MAX);
    strncpy(init.traddr, traddr, ADDR_MAX);

    const int err = pthread_create(&ctx->child, NULL, start_routine, &init);
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
    return ctx;
 fail4:
    {
        void* ret;
        const int err = pthread_join(ctx->child, &ret);
        if (err)
            sc_err_printf(SC_ESYSTEM, "pthread_join() failed: %s", strerror(err));
    }
 fail3:
    zmq_ctx_destroy(ctx->zctx);
    pthread_cond_destroy(&init.cond);
    pthread_mutex_destroy(&init.mutex);
 fail2:
    free(ctx);
 fail1:
    return NULL;
}

SC_API void
sc_ctx_destroy(ScCtx ctx)
{
    // Best effort to close clnt owned by child thread.
    void* sock = fig_async_create(ctx->zctx, ctx->uuid);
    if (sock) {
        if (!fig_async_close(sock))
            sc_err_perror("fig_async_close() failed");
        fig_async_destroy(sock);
    } else
        sc_err_perror("fig_async_create() failed");

    void* ret;
    const int err = pthread_join(ctx->child, &ret);
    if (err)
        sc_err_printf(SC_ESYSTEM, "pthread_join() failed: %s", strerror(err));

    zmq_ctx_destroy(ctx->zctx);
    free(ctx);
}

SC_API ScAsync
sc_ctx_async(ScCtx ctx)
{
    return sc_async_create(ctx->zctx, ctx->uuid);
}

SC_API const unsigned char*
sc_ctx_uuid(ScCtx ctx)
{
    return ctx->uuid;
}
