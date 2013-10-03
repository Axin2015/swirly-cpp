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

static DbrBool
read_entity(const struct DbrMsg* req)
{
    struct DbrMsg rep = { .type = DBR_READ_ENTITY_REP,
                          .read_entity_rep = { .type = req->read_entity_req.type } };
    DbrModel model = dbr_sqlstore_model(store);

    if (dbr_model_read_entity(model, rep.read_entity_rep.type, pool,
                              &rep.read_entity_rep.first) < 0) {
        dbr_err_print("dbr_model_read_entity() failed");
        goto fail1;
    }
    DbrBool ok = dbr_send_msg(sock, &rep);
    dbr_pool_free_list(pool, rep.read_entity_rep.type, rep.read_entity_rep.first);
    if (!ok) {
        dbr_err_print("dbr_send_msg() failed");
        goto fail1;
    }
    return true;
 fail1:
    return false;
}

static DbrBool
write_trans(const struct DbrMsg* req)
{
    struct DbrMsg rep = { .type = DBR_STATUS_OK };
    DbrJourn journ = dbr_sqlstore_journ(store);

    if (!dbr_journ_begin_trans(journ)) {
        dbr_err_print("dbr_journ_begin_trans() failed");
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
                                        stmt->insert_order.resd,
                                        stmt->insert_order.exec,
                                        stmt->insert_order.lots,
                                        stmt->insert_order.min,
                                        stmt->insert_order.flags,
                                        stmt->insert_order.now)) {
                dbr_err_print("dbr_journ_insert_order() failed");
                goto fail1;
            }
            break;
        case DBR_UPDATE_ORDER:
            if (!dbr_journ_update_order(journ,
                                        stmt->update_order.id,
                                        stmt->update_order.rev,
                                        stmt->update_order.status,
                                        stmt->update_order.resd,
                                        stmt->update_order.exec,
                                        stmt->update_order.lots,
                                        stmt->update_order.now)) {
                dbr_err_print("dbr_journ_update_order() failed");
                goto fail1;
            }
            break;
        case DBR_ARCHIVE_ORDER:
            if (!dbr_journ_archive_order(journ,
                                         stmt->archive_order.id,
                                         stmt->archive_order.now)) {
                dbr_err_print("dbr_journ_archive_order() failed");
                goto fail1;
            }
            break;
        case DBR_INSERT_TRADE:
            if (!dbr_journ_insert_trade(journ,
                                        stmt->insert_trade.id,
                                        stmt->insert_trade.match,
                                        stmt->insert_trade.order,
                                        stmt->insert_trade.order_rev,
                                        stmt->insert_trade.tid,
                                        stmt->insert_trade.aid,
                                        stmt->insert_trade.cid,
                                        stmt->insert_trade.settl_date,
                                        stmt->insert_trade.ref,
                                        stmt->insert_trade.cpty,
                                        stmt->insert_trade.role,
                                        stmt->insert_trade.action,
                                        stmt->insert_trade.ticks,
                                        stmt->insert_trade.resd,
                                        stmt->insert_trade.exec,
                                        stmt->insert_trade.lots,
                                        stmt->insert_trade.now)) {
                dbr_err_print("dbr_journ_insert_trade() failed");
                goto fail1;
            }
            break;
        case DBR_ARCHIVE_TRADE:
            if (!dbr_journ_archive_trade(journ,
                                         stmt->archive_trade.id,
                                         stmt->archive_trade.now)) {
                dbr_err_print("dbr_journ_archive_trade() failed");
                goto fail1;
            }
            break;
        }
    }
    if (!dbr_journ_commit_trans(journ)) {
        dbr_err_print("dbr_journ_commit_trans() failed");
        goto fail1;
    }
    DbrBool ok = dbr_send_msg(sock, &rep);
    if (!dbr_send_msg(sock, &rep))
        dbr_err_print("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_journ_rollback_trans(journ))
        dbr_err_print("dbr_journ_rollback_trans() failed");
    return false;
}

static DbrBool
run(void)
{
    while (!quit) {
        struct DbrMsg req;
        if (!dbr_recv_msg(sock, pool, &req)) {
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

    if (zmq_bind(sock, "tcp://*:5555") < 0)
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
