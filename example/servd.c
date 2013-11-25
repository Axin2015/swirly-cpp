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
#include <dbr/accnt.h>
#include <dbr/err.h>
#include <dbr/serv.h>
#include <dbr/journ.h>
#include <dbr/log.h>
#include <dbr/msg.h>
#include <dbr/queue.h>
#include <dbr/sqlstore.h>
#include <dbr/trader.h>
#include <dbr/tree.h>

#include <zmq.h>

#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static DbrPool pool = NULL;
static DbrSqlStore store = NULL;
static DbrServ serv = NULL;
static void* ctx = NULL;
static void* sock = NULL;

static volatile sig_atomic_t quit = false;

static void
status_err(struct DbrBody* body, DbrIden req_id)
{
    body->req_id = req_id;
    body->type = DBR_STATUS_REP;
    body->status_rep.num = dbr_err_num();
    strncpy(body->status_rep.msg, dbr_err_msg(), DBR_ERRMSG_MAX);
}

static void
status_setf(struct DbrBody* body, DbrIden req_id, int num, const char* format, ...)
{
    body->req_id = req_id;
    body->type = DBR_STATUS_REP;
    body->status_rep.num = num;

    va_list args;
    va_start(args, format);
    vsnprintf(body->status_rep.msg, DBR_ERRMSG_MAX, format, args);
    va_end(args);
}

DBR_API struct DbrRec*
find_rec_mnem(int type, const char* mnem)
{
    struct DbrSlNode* node = dbr_serv_find_rec_mnem(serv, type, mnem);
    return node != DBR_SERV_END_REC ? dbr_entity_rec_entry(node) : NULL;
}

static DbrBool
sess_rec(DbrTrader trader, const struct DbrBody* req)
{
    struct DbrBody rep;

    struct DbrSlNode* first = dbr_serv_first_rec(serv, req->sess_entity_req.type, NULL);

    rep.req_id = req->req_id;
    rep.type = DBR_ENTITY_REP;
    rep.entity_rep.type = req->sess_entity_req.type;
    rep.entity_rep.first = first;
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_print("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_order(DbrTrader trader, const struct DbrBody* req)
{
    struct DbrBody rep;

    // Copy to entity node.
    struct DbrQueue q = DBR_QUEUE_INIT(q);
    for (struct DbrRbNode* node = dbr_trader_first_order(trader);
         node != DBR_TRADER_END_ORDER; node = dbr_rbnode_next(node)) {
        struct DbrOrder* order = dbr_trader_order_entry(node);
        dbr_queue_insert_back(&q, &order->entity_node_);
    }
    rep.req_id = req->req_id;
    rep.type = DBR_ENTITY_REP;
    rep.entity_rep.type = DBR_ORDER;
    rep.entity_rep.first = dbr_queue_first(&q);
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_print("dbr_send_msg() failed");
    return ok;
 }

static DbrBool
sess_trade(DbrTrader trader, const struct DbrBody* req)
{
    struct DbrBody rep;

    // Copy to entity node.
    struct DbrQueue q = DBR_QUEUE_INIT(q);
    for (struct DbrRbNode* node = dbr_trader_first_trade(trader);
         node != DBR_TRADER_END_TRADE; node = dbr_rbnode_next(node)) {
        struct DbrExec* exec = dbr_trader_trade_entry(node);
        dbr_queue_insert_back(&q, &exec->entity_node_);
    }
    rep.req_id = req->req_id;
    rep.type = DBR_ENTITY_REP;
    rep.entity_rep.type = DBR_EXEC;
    rep.entity_rep.first = dbr_queue_first(&q);
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_print("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_memb(DbrTrader trader, const struct DbrBody* req)
{
    struct DbrBody rep;

    // Copy to entity node.
    struct DbrQueue q = DBR_QUEUE_INIT(q);
    for (struct DbrRbNode* node = dbr_trader_first_memb(trader);
         node != DBR_TRADER_END_MEMB; node = dbr_rbnode_next(node)) {
        struct DbrMemb* memb = dbr_trader_memb_entry(node);
        dbr_queue_insert_back(&q, &memb->entity_node_);
    }
    rep.req_id = req->req_id;
    rep.type = DBR_ENTITY_REP;
    rep.entity_rep.type = DBR_MEMB;
    rep.entity_rep.first = dbr_queue_first(&q);
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_print("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_posn(DbrTrader trader, const struct DbrBody* req)
{
    struct DbrBody rep;

    struct DbrQueue q = DBR_QUEUE_INIT(q);
    // For each account.
    for (struct DbrRbNode* mnode = dbr_trader_first_memb(trader);
         mnode != DBR_TRADER_END_MEMB; mnode = dbr_rbnode_next(mnode)) {
        struct DbrMemb* memb = dbr_trader_memb_entry(mnode);
        struct DbrRec* arec = memb->accnt.rec;
        DbrAccnt accnt = dbr_serv_accnt(serv, arec);
        if (!accnt) {
            status_err(&rep, req->req_id);
            goto fail1;
        }
        // Copy each posn to entity node.
        for (struct DbrRbNode* pnode = dbr_accnt_first_posn(accnt);
             pnode != DBR_ACCNT_END_POSN; pnode = dbr_rbnode_next(pnode)) {
            struct DbrPosn* posn = dbr_accnt_posn_entry(pnode);
            dbr_queue_insert_back(&q, &posn->entity_node_);
        }
    }
    rep.req_id = req->req_id;
    rep.type = DBR_ENTITY_REP;
    rep.entity_rep.type = DBR_POSN;
    rep.entity_rep.first = dbr_queue_first(&q);
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_print("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_print("dbr_send_msg() failed");
    return false;
}

static DbrBool
place_order(DbrTrader trader, const struct DbrBody* req)
{
    struct DbrBody rep;

    struct DbrRec* arec = find_rec_mnem(DBR_ACCNT, req->place_order_req.accnt);
    if (!arec) {
        status_setf(&rep, req->req_id, DBR_EINVAL, "no such accnt '%.16s'",
                    req->place_order_req.accnt);
        goto fail1;
    }
    struct DbrRec* crec = find_rec_mnem(DBR_CONTR, req->place_order_req.contr);
    if (!crec) {
        status_setf(&rep, req->req_id, DBR_EINVAL, "no such contr '%.16s'",
                    req->place_order_req.contr);
        goto fail1;
    }
    DbrAccnt accnt = dbr_serv_accnt(serv, arec);
    if (!accnt) {
        status_err(&rep, req->req_id);
        goto fail1;
    }
    struct DbrBook* book = dbr_serv_book(serv, crec, req->place_order_req.settl_date);
    if (!book) {
        status_err(&rep, req->req_id);
        goto fail1;
    }

    const char* ref = req->place_order_req.ref;
    const int action = req->place_order_req.action;
    const DbrTicks ticks = req->place_order_req.ticks;
    const DbrLots lots = req->place_order_req.lots;
    const DbrLots min_lots = req->place_order_req.min_lots;

    struct DbrOrder* order = dbr_serv_place(serv, trader, accnt, book, ref, action, ticks,
                                            lots, min_lots);
    if (!order) {
        status_err(&rep, req->req_id);
        goto fail1;
    }

    struct DbrQueue posns;
    dbr_queue_init(&posns);
    for (struct DbrRbNode* node = dbr_serv_first_posn(serv);
         node != DBR_TREE_END; node = dbr_rbnode_next(node)) {
        struct DbrPosn* posn = dbr_serv_posn_entry(node);
        dbr_queue_insert_back(&posns, &posn->entity_node_);
    }

    rep.req_id = req->req_id;
    rep.type = DBR_ORDER_REP;
    rep.order_rep.order = order;
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_print("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_print("dbr_send_msg() failed");
    return false;
}

static DbrBool
revise_order_id(DbrTrader trader, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden id = req->revise_order_id_req.id;
    const DbrLots lots = req->revise_order_id_req.lots;

    struct DbrOrder* order = dbr_serv_revise_id(serv, trader, id, lots);
    if (!order) {
        status_err(&rep, req->req_id);
        goto fail1;
    }

    rep.req_id = req->req_id;
    rep.type = DBR_ORDER_REP;
    rep.order_rep.order = order;
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_print("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_print("dbr_send_msg() failed");
    return false;
}

static DbrBool
revise_order_ref(DbrTrader trader, const struct DbrBody* req)
{
    struct DbrBody rep;

    const char* ref = req->revise_order_ref_req.ref;
    const DbrLots lots = req->revise_order_ref_req.lots;

    struct DbrOrder* order = dbr_serv_revise_ref(serv, trader, ref, lots);
    if (!order) {
        status_err(&rep, req->req_id);
        goto fail1;
    }

    rep.req_id = req->req_id;
    rep.type = DBR_ORDER_REP;
    rep.order_rep.order = order;
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_print("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_print("dbr_send_msg() failed");
    return false;
}

static DbrBool
cancel_order_id(DbrTrader trader, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden id = req->cancel_order_id_req.id;

    struct DbrOrder* order = dbr_serv_cancel_id(serv, trader, id);
    if (!order) {
        status_err(&rep, req->req_id);
        goto fail1;
    }

    rep.req_id = req->req_id;
    rep.type = DBR_ORDER_REP;
    rep.order_rep.order = order;
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_print("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_print("dbr_send_msg() failed");
    return false;
}

static DbrBool
cancel_order_ref(DbrTrader trader, const struct DbrBody* req)
{
    struct DbrBody rep;

    const char* ref = req->cancel_order_ref_req.ref;

    struct DbrOrder* order = dbr_serv_cancel_ref(serv, trader, ref);
    if (!order) {
        status_err(&rep, req->req_id);
        goto fail1;
    }

    rep.req_id = req->req_id;
    rep.type = DBR_ORDER_REP;
    rep.order_rep.order = order;
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_print("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_print("dbr_send_msg() failed");
    return false;
}

static DbrBool
archive_order(DbrTrader trader, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden id = req->archive_order_req.id;

    if (!dbr_serv_archive_order(serv, trader, id)) {
        status_err(&rep, req->req_id);
        goto fail1;
    }

    rep.req_id = req->req_id;
    rep.type = DBR_STATUS_REP;
    rep.status_rep.num = 0;
    rep.status_rep.msg[0] = '\0';
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_print("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_print("dbr_send_msg() failed");
    return false;
}

static DbrBool
archive_trade(DbrTrader trader, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden id = req->archive_trade_req.id;

    if (!dbr_serv_archive_trade(serv, trader, id)) {
        status_err(&rep, req->req_id);
        goto fail1;
    }

    rep.req_id = req->req_id;
    rep.type = DBR_STATUS_REP;
    rep.status_rep.num = 0;
    rep.status_rep.msg[0] = '\0';
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_print("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_print("dbr_send_msg() failed");
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
        struct DbrBody rep;
        struct DbrRec* trec = find_rec_mnem(DBR_TRADER, req.head.trader);
        if (!trec) {
            status_setf(&rep, req.body.req_id, DBR_EINVAL, "no such trader '%.16s'",
                        req.head.trader);
            if (!dbr_send_msg(sock, req.head.trader, &rep, false))
                dbr_err_print("dbr_send_msg() failed");
            continue;
        }
        DbrTrader trader = dbr_serv_trader(serv, trec);
        if (!trader) {
            status_err(&rep, req.body.req_id);
            if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
                dbr_err_print("dbr_send_msg() failed");
            continue;
        }
        switch (req.body.type) {
        case DBR_SESS_ENTITY_REQ:
            switch (req.body.sess_entity_req.type) {
            case DBR_TRADER:
            case DBR_ACCNT:
            case DBR_CONTR:
                sess_rec(trader, &req.body);
                break;
            case DBR_ORDER:
                sess_order(trader, &req.body);
                break;
            case DBR_EXEC:
                sess_trade(trader, &req.body);
                break;
            case DBR_MEMB:
                sess_memb(trader, &req.body);
                break;
            case DBR_POSN:
                sess_posn(trader, &req.body);
                break;
            };
            break;
        case DBR_PLACE_ORDER_REQ:
            place_order(trader, &req.body);
            break;
        case DBR_REVISE_ORDER_ID_REQ:
            revise_order_id(trader, &req.body);
            break;
        case DBR_REVISE_ORDER_REF_REQ:
            revise_order_ref(trader, &req.body);
            break;
        case DBR_CANCEL_ORDER_ID_REQ:
            cancel_order_id(trader, &req.body);
            break;
        case DBR_CANCEL_ORDER_REF_REQ:
            cancel_order_ref(trader, &req.body);
            break;
        case DBR_ARCHIVE_ORDER_REQ:
            archive_order(trader, &req.body);
            break;
        case DBR_ARCHIVE_TRADE_REQ:
            archive_trade(trader, &req.body);
            break;
        default:
            // TODO: unsupported type.
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

    DbrJourn journ = dbr_sqlstore_journ(store);
    DbrModel model = dbr_sqlstore_model(store);

    serv = dbr_serv_create(journ, model, pool);
    if (!serv) {
        dbr_err_print("dbr_serv_create() failed");
        goto exit3;
    }

    ctx = zmq_ctx_new();
    if (!ctx)
        goto exit4;

    sock = zmq_socket(ctx, ZMQ_ROUTER);
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
    dbr_serv_destroy(serv);
 exit3:
    dbr_sqlstore_destroy(store);
 exit2:
    dbr_pool_destroy(pool);
 exit1:
    return status;
}
