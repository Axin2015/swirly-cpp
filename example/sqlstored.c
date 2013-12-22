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
#define _XOPEN_SOURCE 700 // strnlen()

#include <dbr/err.h>
#include <dbr/journ.h>
#include <dbr/log.h>
#include <dbr/model.h>
#include <dbr/msg.h>
#include <dbr/sqlstore.h>
#include <dbr/util.h>

#include <zmq.h>

#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static DbrPool pool = NULL;
static DbrSqlStore store = NULL;
static void* ctx = NULL;
static void* sock = NULL;

static volatile sig_atomic_t quit = false;

static void
status_err(struct DbrBody* rep, DbrIden req_id)
{
    rep->req_id = req_id;
    rep->type = DBR_STATUS_REP;
    rep->status_rep.num = dbr_err_num();
    strncpy(rep->status_rep.msg, dbr_err_msg(), DBR_ERRMSG_MAX);
}

static DbrBool
read_entity(const struct DbrBody* req)
{
    struct DbrBody rep = { .req_id = req->req_id, .type = DBR_ENTITY_LIST_REP,
                           .entity_list_rep = { .type = req->read_entity_req.type } };
    DbrModel model = dbr_sqlstore_model(store);

    if (dbr_model_read_entity(model, rep.entity_list_rep.type, pool, &rep.entity_list_rep.first) < 0) {
        dbr_err_prints("dbr_model_read_entity() failed");
        status_err(&rep, req->req_id);
        goto fail1;
    }
    const DbrBool ok = dbr_send_body(sock, &rep, false);
    dbr_pool_free_entity_list(pool, rep.entity_list_rep.type, rep.entity_list_rep.first);
    if (!ok)
        dbr_err_prints("dbr_send_body() failed");
    return ok;
 fail1:
    if (!dbr_send_body(sock, &rep, false))
        dbr_err_prints("dbr_send_body() failed");
    return false;
}

static DbrBool
insert_exec_list(struct DbrBody* req)
{
    struct DbrBody rep = { .req_id = req->req_id, .type = DBR_STATUS_REP,
                           .status_rep = { .num = 0, .msg = "" } };
    DbrJourn journ = dbr_sqlstore_journ(store);

    if (!dbr_journ_insert_exec_list(journ, req->insert_exec_list_req.first, false)) {
        dbr_err_prints("dbr_journ_insert_exec_list() failed");
        status_err(&rep, req->req_id);
        goto fail1;
    }
    const DbrBool ok = dbr_send_body(sock, &rep, false);
    if (!ok)
        dbr_err_prints("dbr_send_body() failed");
    return ok;
 fail1:
    if (!dbr_send_body(sock, &rep, false))
        dbr_err_prints("dbr_send_body() failed");
    return false;
}

static DbrBool
insert_exec(struct DbrBody* req)
{
    struct DbrBody rep = { .req_id = req->req_id, .type = DBR_STATUS_REP,
                           .status_rep = { .num = 0, .msg = "" } };
    DbrJourn journ = dbr_sqlstore_journ(store);

    if (!dbr_journ_insert_exec(journ, req->insert_exec_req.exec, false)) {
        dbr_err_prints("dbr_journ_insert_exec() failed");
        status_err(&rep, req->req_id);
        goto fail1;
    }
    const DbrBool ok = dbr_send_body(sock, &rep, false);
    if (!ok)
        dbr_err_prints("dbr_send_body() failed");
    return ok;
 fail1:
    if (!dbr_send_body(sock, &rep, false))
        dbr_err_prints("dbr_send_body() failed");
    return false;
}

static DbrBool
update_exec(struct DbrBody* req)
{
    struct DbrBody rep = { .req_id = req->req_id, .type = DBR_STATUS_REP,
                           .status_rep = { .num = 0, .msg = "" } };
    DbrJourn journ = dbr_sqlstore_journ(store);

    if (!dbr_journ_update_exec(journ, req->update_exec_req.id, req->update_exec_req.modified)) {
        dbr_err_prints("dbr_journ_update_exec() failed");
        status_err(&rep, req->req_id);
        goto fail1;
    }
    const DbrBool ok = dbr_send_body(sock, &rep, false);
    if (!ok)
        dbr_err_prints("dbr_send_body() failed");
    return ok;
 fail1:
    if (!dbr_send_body(sock, &rep, false))
        dbr_err_prints("dbr_send_body() failed");
    return false;
}

static DbrBool
run(void)
{
    while (!quit) {
        struct DbrBody req;
        if (!dbr_recv_body(sock, pool, &req)) {
            if (dbr_err_num() == DBR_EINTR)
                continue;
            dbr_err_prints("dbr_recv_msg() failed");
            goto fail1;
        }
        switch (req.type) {
        case DBR_READ_ENTITY_REQ:
            read_entity(&req);
            break;
        case DBR_INSERT_EXEC_LIST_REQ:
            insert_exec_list(&req);
            dbr_pool_free_entity_list(pool, DBR_EXEC, req.insert_exec_list_req.first);
            break;
        case DBR_INSERT_EXEC_REQ:
            insert_exec(&req);
            dbr_pool_free_exec(pool, req.insert_exec_req.exec);
            break;
        case DBR_UPDATE_EXEC_REQ:
            update_exec(&req);
            break;
        }
    }
    return true;
 fail1:
    return false;
}

static void
sighandler(int signum)
{
    quit = true;
}

int
main(int argc, char* argv[])
{
    int status = 1;

    pool = dbr_pool_create();
    if (!pool) {
        dbr_err_prints("dbr_pool_create() failed");
        goto exit1;
    }

    store = dbr_sqlstore_create("doobry.db", dbr_millis());
    if (!store) {
        dbr_err_prints("dbr_sqlstore_create() failed");
        goto exit2;
    }

    ctx = zmq_ctx_new();
    if (!ctx)
        goto exit3;

    sock = zmq_socket(ctx, ZMQ_REP);
    if (!sock)
        goto exit4;

    if (zmq_bind(sock, "tcp://*:3271") < 0)
        goto exit5;

    struct sigaction action;
    action.sa_handler = sighandler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    if (!run())
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
