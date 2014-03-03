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

#include <dbr/err.h>
#include <dbr/log.h>
#include <dbr/msg.h>
#include <dbr/refcount.h>
#include <dbr/sqljourn.h>
#include <dbr/sqlmodel.h>
#include <dbr/util.h>

#include <zmq.h>

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static DbrPool pool = NULL;
static DbrJourn journ = NULL;
static DbrModel model = NULL;
static void* ctx = NULL;
static void* sock = NULL;

static volatile sig_atomic_t quit = DBR_FALSE;

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
    struct DbrBody rep = { .req_id = req->req_id };
    switch (req->type) {
    case DBR_ENTITY_TRADER:
        rep.type = DBR_TRADER_LIST_REP;
        break;
    case DBR_ENTITY_ACCNT:
        rep.type = DBR_ACCNT_LIST_REP;
        break;
    case DBR_ENTITY_CONTR:
        rep.type = DBR_CONTR_LIST_REP;
        break;
    case DBR_ENTITY_ORDER:
        rep.type = DBR_ORDER_LIST_REP;
        break;
    case DBR_ENTITY_EXEC:
        rep.type = DBR_EXEC_LIST_REP;
        break;
    case DBR_ENTITY_MEMB:
        rep.type = DBR_MEMB_LIST_REP;
        break;
    case DBR_ENTITY_POSN:
        rep.type = DBR_POSN_LIST_REP;
        break;
    }
    const int type = req->read_entity_req.type;
    const ssize_t size = dbr_model_read_entity(model, type, pool, &rep.entity_list_rep.first);
    if (size < 0) {
        dbr_err_prints("dbr_model_read_entity() failed");
        status_err(&rep, req->req_id);
        goto fail1;
    }
    const DbrBool ok = dbr_send_body(sock, &rep, DBR_FALSE);
    assert(rep.entity_list_rep.count_ == size);
    dbr_pool_free_entity_list(pool, type, rep.entity_list_rep.first);
    if (!ok)
        dbr_err_prints("dbr_send_body() failed");
    return ok;
 fail1:
    if (!dbr_send_body(sock, &rep, DBR_FALSE))
        dbr_err_prints("dbr_send_body() failed");
    return DBR_FALSE;
}

static DbrBool
insert_exec_list(struct DbrBody* req)
{
    struct DbrBody rep = { .req_id = req->req_id, .type = DBR_STATUS_REP,
                           .status_rep = { .num = 0, .msg = "" } };

    if (!dbr_journ_insert_exec_list(journ, req->insert_exec_list_req.first, DBR_FALSE)) {
        dbr_err_prints("dbr_journ_insert_exec_list() failed");
        status_err(&rep, req->req_id);
        goto fail1;
    }
    const DbrBool ok = dbr_send_body(sock, &rep, DBR_FALSE);
    if (!ok)
        dbr_err_prints("dbr_send_body() failed");
    return ok;
 fail1:
    if (!dbr_send_body(sock, &rep, DBR_FALSE))
        dbr_err_prints("dbr_send_body() failed");
    return DBR_FALSE;
}

static DbrBool
insert_exec(struct DbrBody* req)
{
    struct DbrBody rep = { .req_id = req->req_id, .type = DBR_STATUS_REP,
                           .status_rep = { .num = 0, .msg = "" } };

    if (!dbr_journ_insert_exec(journ, req->insert_exec_req.exec, DBR_FALSE)) {
        dbr_err_prints("dbr_journ_insert_exec() failed");
        status_err(&rep, req->req_id);
        goto fail1;
    }
    const DbrBool ok = dbr_send_body(sock, &rep, DBR_FALSE);
    if (!ok)
        dbr_err_prints("dbr_send_body() failed");
    return ok;
 fail1:
    if (!dbr_send_body(sock, &rep, DBR_FALSE))
        dbr_err_prints("dbr_send_body() failed");
    return DBR_FALSE;
}

static DbrBool
update_exec(struct DbrBody* req)
{
    struct DbrBody rep = { .req_id = req->req_id, .type = DBR_STATUS_REP,
                           .status_rep = { .num = 0, .msg = "" } };

    if (!dbr_journ_update_exec(journ, req->update_exec_req.id, req->update_exec_req.modified)) {
        dbr_err_prints("dbr_journ_update_exec() failed");
        status_err(&rep, req->req_id);
        goto fail1;
    }
    const DbrBool ok = dbr_send_body(sock, &rep, DBR_FALSE);
    if (!ok)
        dbr_err_prints("dbr_send_body() failed");
    return ok;
 fail1:
    if (!dbr_send_body(sock, &rep, DBR_FALSE))
        dbr_err_prints("dbr_send_body() failed");
    return DBR_FALSE;
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
            dbr_pool_free_entity_list(pool, DBR_ENTITY_EXEC, req.insert_exec_list_req.first);
            break;
        case DBR_INSERT_EXEC_REQ:
            insert_exec(&req);
            dbr_exec_decref(req.insert_exec_req.exec, pool);
            break;
        case DBR_UPDATE_EXEC_REQ:
            update_exec(&req);
            break;
        }
    }
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

static void
sighandler(int signum)
{
    quit = DBR_TRUE;
}

int
main(int argc, char* argv[])
{
    int status = 1;

    pool = dbr_pool_create(dbr_millis(), 8 * 1024 * 1024);
    if (!pool) {
        dbr_err_prints("dbr_pool_create() failed");
        goto exit1;
    }

    journ = dbr_sqljourn_create("doobry.db");
    if (!journ) {
        dbr_err_prints("dbr_sqljourn_create() failed");
        goto exit2;
    }

    model = dbr_sqlmodel_create("doobry.db");
    if (!model) {
        dbr_err_prints("dbr_sqlmodel_create() failed");
        goto exit3;
    }

    ctx = zmq_ctx_new();
    if (!ctx)
        goto exit4;

    sock = zmq_socket(ctx, ZMQ_REP);
    if (!sock)
        goto exit5;

    if (zmq_bind(sock, "tcp://*:3272") < 0)
        goto exit6;

    struct sigaction action;
    action.sa_handler = sighandler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    if (!run())
        goto exit6;

    dbr_log_info("exiting...");
    status = 0;
 exit6:
    zmq_close(sock);
 exit5:
    zmq_ctx_destroy(ctx);
 exit4:
    dbr_model_destroy(model);
 exit3:
    dbr_journ_destroy(journ);
 exit2:
    dbr_pool_destroy(pool);
 exit1:
    return status;
}
