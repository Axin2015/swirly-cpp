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
#include <dbr/ctx.h>

#include "async.h"

#include <dbr/clnt.h>
#include <dbr/dispatch.h>
#include <dbr/err.h>
#include <dbr/log.h>
#include <dbr/pool.h>
#include <dbr/util.h>

#include <uuid/uuid.h>

#include <zmq.h>

#include <stdlib.h> // malloc()
#include <string.h> // strerror()

#include <pthread.h>

struct FigCtx {
    // Shared state.
    void* zctx;
    DbrUuid uuid;
    // Thread state confined to parent.
    pthread_t child;
};

enum { ADDR_MAX = 128 };
enum { INIT = -1, SUCCESS = 0, FAILURE = 1 };

struct Init {

    char mdaddr[ADDR_MAX + 1];
    char traddr[ADDR_MAX + 1];
    DbrIden seed;
    DbrMillis tmout;
    size_t capacity;
    DbrHandler handler;

    int level;
    DbrLogger logger;

    DbrCtx ctx;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int state;
    int err_num;
    char err_msg[DBR_ERRMSG_MAX];
};

static void*
start_routine(void* arg)
{
    enum { TMOUT = 5000 };

    struct Init* init = arg;
    DbrCtx ctx = init->ctx;

    // Inherit parent's level and logger.
    dbr_log_setlevel(init->level);
    dbr_log_setlogger(init->logger);

    DbrPool pool = dbr_pool_create(init->capacity);
    if (!pool)
        goto fail1;

    DbrClnt clnt = dbr_clnt_create(ctx->zctx, ctx->uuid, init->mdaddr, init->traddr,
                                   init->seed, init->tmout, pool);
    if (!clnt)
        goto fail2;

    DbrHandler handler = init->handler;

    pthread_mutex_lock(&init->mutex);
    init->state = SUCCESS;
    pthread_cond_signal(&init->cond);
    pthread_mutex_unlock(&init->mutex);
    // The init pointer is left dangling beyond this point.
    // Fail-fast if accessed due to logic-error.
    init = NULL;

    // Dispatch loop.
    while (!dbr_clnt_is_closed(clnt)) {
        if (!dbr_clnt_dispatch(clnt, TMOUT, handler))
            dbr_err_perror("dbr_clnt_dispatch() failed");
    }
    dbr_log_info("exiting thread");

    dbr_clnt_destroy(clnt);
    dbr_pool_destroy(pool);
    return NULL;
 fail2:
    dbr_pool_destroy(pool);
 fail1:
    init->err_num = dbr_err_num();
    strncpy(init->err_msg, dbr_err_msg(), DBR_ERRMSG_MAX);

    pthread_mutex_lock(&init->mutex);
    init->state = FAILURE;
    pthread_cond_signal(&init->cond);
    pthread_mutex_unlock(&init->mutex);
    return NULL;
}

DBR_API DbrCtx
dbr_ctx_create(const char* mdaddr, const char* traddr, DbrIden seed, DbrMillis tmout,
               size_t capacity, DbrHandler handler)
{
    DbrCtx ctx = malloc(sizeof(struct FigCtx));
    if (dbr_unlikely(!ctx)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    if (!(ctx->zctx = zmq_ctx_new())) {
        dbr_err_setf(DBR_EIO, "zmq_ctx_new() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    uuid_generate(ctx->uuid);

    struct Init init = {

        .mdaddr = { 0 },
        .traddr = { 0 },
        .seed = seed,
        .tmout = tmout,
        .capacity = capacity,
        .handler = handler,

        .level = dbr_log_level(),
        .logger = dbr_log_logger(),

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
    return ctx;
 fail4:
    {
        void* ret;
        const int err = pthread_join(ctx->child, &ret);
        if (err)
            dbr_err_printf(DBR_ESYSTEM, "pthread_join() failed: %s", strerror(err));
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

DBR_API void
dbr_ctx_destroy(DbrCtx ctx)
{
    // Best effort to close clnt owned by child thread.
    void* sock = fig_async_create(ctx->zctx, ctx->uuid);
    if (sock) {
        if (!fig_async_close(sock))
            dbr_err_perror("fig_async_close() failed");
        fig_async_destroy(sock);
    } else
        dbr_err_perror("fig_async_create() failed");

    void* ret;
    const int err = pthread_join(ctx->child, &ret);
    if (err)
        dbr_err_printf(DBR_ESYSTEM, "pthread_join() failed: %s", strerror(err));

    zmq_ctx_destroy(ctx->zctx);
    free(ctx);
}

DBR_API DbrAsync
dbr_ctx_async(DbrCtx ctx)
{
    return dbr_async_create(ctx->zctx, ctx->uuid);
}

DBR_API const unsigned char*
dbr_ctx_uuid(DbrCtx ctx)
{
    return ctx->uuid;
}
