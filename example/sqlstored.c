/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
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
#include <dbr/err.h>
#include <dbr/log.h>
#include <dbr/model.h>
#include <dbr/msg.h>
#include <dbr/sqlstore.h>

#include <zmq.h>

#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static volatile sig_atomic_t quit = false;

static void
sighandler(int signum)
{
    quit = true;
}

static DbrBool
run(void* sock, DbrSqlStore store, DbrPool pool)
{
    DbrModel model = dbr_sqlstore_model(store);
    while (!quit) {
        struct DbrMsg msg;
        if (!dbr_recv_msg(sock, pool, &msg)) {
            if (dbr_err_num() == DBR_EINTR)
                continue;
            dbr_err_print("dbr_recv_msg() failed");
            goto fail1;
        }

        msg.type = DBR_READ_ENTITY_REP;
        msg.read_entity_rep.type = msg.read_entity_req.type;
        if (dbr_model_read_entity(model, msg.read_entity_rep.type, pool,
                                  &msg.read_entity_rep.first) < 0) {
            dbr_err_print("dbr_model_read_entity() failed");
            goto fail1;
        }

        DbrBool ret = dbr_send_msg(sock, &msg);
        dbr_pool_free_list(pool, msg.read_entity_rep.type, msg.read_entity_rep.first);
        if (!ret) {
            dbr_err_print("dbr_send_msg() failed");
            goto fail1;
        }
    }
    return true;
 fail1:
    return false;
}

int
main(int argc, char* argv[])
{
    int status = 1;

    DbrPool pool = dbr_pool_create();
    if (!pool) {
        dbr_err_print("dbr_pool_create() failed");
        goto exit1;
    }

    DbrSqlStore store = dbr_sqlstore_create(1, "doobry.db");
    if (!store) {
        dbr_err_print("dbr_sqlstore_create() failed");
        goto exit2;
    }

    void* ctx = zmq_ctx_new();
    if (!ctx)
        goto exit3;

    void* sock = zmq_socket(ctx, ZMQ_REP);
    if (!sock)
        goto exit4;

    if (zmq_bind(sock, "tcp://*:5555") < 0)
        goto exit5;

    struct sigaction action;
    action.sa_handler = sighandler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    if (!run(sock, store, pool))
        goto exit5;

    dbr_log_info("exiting...");
    status = 0;
 exit5:
    zmq_close(sock);
 exit4:
    zmq_ctx_destroy(ctx);
 exit3:
    dbr_sqlstore_destroy(store);
 exit2:
    dbr_pool_destroy(pool);
 exit1:
    return status;
}
