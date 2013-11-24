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
#include <dbr/journ.h>
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
#include <string.h>

static DbrPool pool = NULL;
static DbrSqlStore store = NULL;
static void* ctx = NULL;
static void* sock = NULL;

static volatile sig_atomic_t quit = false;

static void
free_stmts(struct DbrSlNode* first, DbrPool pool)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrStmt* stmt = dbr_trans_stmt_entry(node);
        node = node->next;
        dbr_pool_free_stmt(pool, stmt);
    }
}

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
    struct DbrBody rep = { .req_id = req->req_id, .type = DBR_ENTITY_REP,
                           .entity_rep = { .type = req->read_entity_req.type } };
    DbrModel model = dbr_sqlstore_model(store);

    if (dbr_model_read_entity(model, rep.entity_rep.type, pool, &rep.entity_rep.first) < 0) {
        dbr_err_print("dbr_model_read_entity() failed");
        status_err(&rep, req->req_id);
        goto fail1;
    }
    const DbrBool ok = dbr_send_body(sock, &rep, false);
    dbr_pool_free_entities(pool, rep.entity_rep.type, rep.entity_rep.first);
    if (!ok)
        dbr_err_print("dbr_send_body() failed");
    return ok;
 fail1:
    if (!dbr_send_body(sock, &rep, false))
        dbr_err_print("dbr_send_body() failed");
    return false;
}

static DbrBool
write_trans(const struct DbrBody* req)
{
    struct DbrBody rep = { .req_id = req->req_id, .type = DBR_STATUS_REP,
                           .status_rep = { .num = 0, .msg = "" } };
    DbrJourn journ = dbr_sqlstore_journ(store);

    if (!dbr_journ_begin_trans(journ)) {
        dbr_err_print("dbr_journ_begin_trans() failed");
        status_err(&rep, req->req_id);
        goto fail1;
    }
    for (struct DbrSlNode* node = req->write_trans_req.first; node; node = node->next) {
        struct DbrStmt* stmt = dbr_trans_stmt_entry(node);
        switch (stmt->type) {
        case DBR_INSERT_ORDER:
            if (!dbr_journ_insert_order(journ,
                                        stmt->insert_order.id,
                                        stmt->insert_order.rev,
                                        stmt->insert_order.status,
                                        stmt->insert_order.tid,
                                        stmt->insert_order.aid,
                                        stmt->insert_order.cid,
                                        stmt->insert_order.settl_date,
                                        stmt->insert_order.ref,
                                        stmt->insert_order.action,
                                        stmt->insert_order.ticks,
                                        stmt->insert_order.lots,
                                        stmt->insert_order.resd,
                                        stmt->insert_order.exec,
                                        stmt->insert_order.last_ticks,
                                        stmt->insert_order.last_lots,
                                        stmt->insert_order.min_lots,
                                        stmt->insert_order.now)) {
                dbr_err_print("dbr_journ_insert_order() failed");
                status_err(&rep, req->req_id);
                goto fail2;
            }
            break;
        case DBR_UPDATE_ORDER:
            if (!dbr_journ_update_order(journ,
                                        stmt->update_order.id,
                                        stmt->update_order.rev,
                                        stmt->update_order.status,
                                        stmt->update_order.lots,
                                        stmt->update_order.resd,
                                        stmt->update_order.exec,
                                        stmt->update_order.last_ticks,
                                        stmt->update_order.last_lots,
                                        stmt->update_order.now)) {
                dbr_err_print("dbr_journ_update_order() failed");
                status_err(&rep, req->req_id);
                goto fail2;
            }
            break;
        case DBR_ARCHIVE_ORDER:
            if (!dbr_journ_archive_order(journ,
                                         stmt->archive_order.id,
                                         stmt->archive_order.now)) {
                dbr_err_print("dbr_journ_archive_order() failed");
                status_err(&rep, req->req_id);
                goto fail2;
            }
            break;
        case DBR_INSERT_TRADE:
            if (!dbr_journ_insert_trade(journ,
                                        stmt->insert_trade.id,
                                        stmt->insert_trade.order,
                                        stmt->insert_trade.rev,
                                        stmt->insert_trade.match,
                                        stmt->insert_trade.role,
                                        stmt->insert_trade.cpty,
                                        stmt->insert_trade.now)) {
                dbr_err_print("dbr_journ_insert_trade() failed");
                status_err(&rep, req->req_id);
                goto fail2;
            }
            break;
        case DBR_ARCHIVE_TRADE:
            if (!dbr_journ_archive_trade(journ,
                                         stmt->archive_trade.id,
                                         stmt->archive_trade.now)) {
                dbr_err_print("dbr_journ_archive_trade() failed");
                status_err(&rep, req->req_id);
                goto fail2;
            }
            break;
        }
    }
    if (!dbr_journ_commit_trans(journ)) {
        dbr_err_print("dbr_journ_commit_trans() failed");
        status_err(&rep, req->req_id);
        goto fail2;
    }
    const DbrBool ok = dbr_send_body(sock, &rep, false);
    if (!ok)
        dbr_err_print("dbr_send_body() failed");
    return ok;
 fail2:
    if (!dbr_journ_rollback_trans(journ))
        dbr_err_print("dbr_journ_rollback_trans() failed");
 fail1:
    if (!dbr_send_body(sock, &rep, false))
        dbr_err_print("dbr_send_body() failed");
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
            dbr_err_print("dbr_recv_msg() failed");
            goto fail1;
        }
        switch (req.type) {
        case DBR_READ_ENTITY_REQ:
            read_entity(&req);
            break;
        case DBR_WRITE_TRANS_REQ:
            write_trans(&req);
            free_stmts(req.write_trans_req.first, pool);
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
        dbr_err_print("dbr_pool_create() failed");
        goto exit1;
    }

    store = dbr_sqlstore_create(1, "doobry.db");
    if (!store) {
        dbr_err_print("dbr_sqlstore_create() failed");
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
