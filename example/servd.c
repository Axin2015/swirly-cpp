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
static DbrServ serv = NULL;
static void* ctx = NULL;
static void* sock = NULL;

static volatile sig_atomic_t quit = false;

static inline struct DbrRec*
find_rec_mnem(int type, const char* mnem)
{
    struct DbrSlNode* node = dbr_serv_find_rec_mnem(serv, type, mnem);
    return node != DBR_SERV_END_REC ? dbr_serv_rec_entry(node) : NULL;
}

static void
status_err(struct DbrBody* body, DbrIden req_id)
{
    body->req_id = req_id;
    body->type = DBR_STATUS_REP;
    body->status_rep.num = dbr_err_num();
    strncpy(body->status_rep.msg, dbr_err_msg(), DBR_ERRMSG_MAX);
    dbr_log_error("error: %.128s", body->status_rep.msg);
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
    dbr_log_error("error: %.128s", body->status_rep.msg);
}

static DbrBool
send_exec(DbrIden req_id, struct DbrExec* exec)
{
    struct DbrBody rep = { .req_id = req_id, .type = DBR_EXEC_REP, .exec_rep.exec = exec };
    const DbrBool ok = dbr_send_msg(sock, exec->c.trader.rec->mnem, &rep, true);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
}

static DbrBool
send_posn(DbrIden req_id, struct DbrPosn* posn)
{
    struct DbrBody rep = { .req_id = req_id, .type = DBR_POSN_REP, .posn_rep.posn = posn };

    DbrAccnt accnt = dbr_serv_accnt(serv, posn->accnt.rec);
    for (struct DbrRbNode* node = dbr_accnt_first_memb(accnt);
         node != DBR_ACCNT_END_MEMB; node = dbr_rbnode_next(node)) {

        struct DbrMemb* memb = dbr_accnt_memb_entry(node);
        const char* trader = memb->trader.rec->mnem;
        if (zmq_send(sock, trader, strnlen(trader, DBR_MNEM_MAX), ZMQ_SNDMORE) < 0) {
            dbr_err_setf(DBR_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
            dbr_err_print();
            goto fail1;
        }

        // FIXME: pack message once for all traders.
        if (!dbr_send_body(sock, &rep, true)) {
            dbr_err_prints("dbr_send_msg() failed");
            goto fail1;
        }
    }
    return true;
 fail1:
    return false;
}

static DbrBool
flush(const struct DbrBody* req)
{
    {
        struct DbrSlNode* node = dbr_serv_first_exec(serv);
        assert(node != DBR_SERV_END_EXEC);

        struct DbrExec* exec = dbr_serv_exec_entry(node);
        if (!send_exec(req->req_id, exec))
            goto fail1;

        for (node = dbr_slnode_next(node);
             node != DBR_SERV_END_EXEC; node = dbr_slnode_next(node)) {
            struct DbrExec* exec = dbr_serv_exec_entry(node);
            if (!send_exec(0, exec))
                goto fail1;
        }
    }

    for (struct DbrRbNode* node = dbr_serv_first_posnup(serv);
         node != DBR_SERV_END_POSNUP; node = dbr_rbnode_next(node)) {
        struct DbrPosn* posn = dbr_serv_posnup_entry(node);
        if (!send_posn(0, posn))
            goto fail1;
    }
    dbr_serv_clear(serv);
    return true;
 fail1:
    dbr_serv_clear(serv);
    return false;
}

static DbrBool
sess_trader(DbrIden req_id, DbrTrader trader)
{
    struct DbrBody rep;

    struct DbrSlNode* first = dbr_serv_first_rec(serv, DBR_TRADER, NULL);

    rep.req_id = req_id;
    rep.type = DBR_TRADER_LIST_REP;
    rep.entity_list_rep.first = first;
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_accnt(DbrIden req_id, DbrTrader trader)
{
    struct DbrBody rep;

    struct DbrSlNode* first = dbr_serv_first_rec(serv, DBR_ACCNT, NULL);

    rep.req_id = req_id;
    rep.type = DBR_ACCNT_LIST_REP;
    rep.entity_list_rep.first = first;
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_contr(DbrIden req_id, DbrTrader trader)
{
    struct DbrBody rep;

    struct DbrSlNode* first = dbr_serv_first_rec(serv, DBR_CONTR, NULL);

    rep.req_id = req_id;
    rep.type = DBR_CONTR_LIST_REP;
    rep.entity_list_rep.first = first;
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_order(DbrIden req_id, DbrTrader trader)
{
    struct DbrBody rep;

    // Copy to entity node.
    struct DbrQueue q = DBR_QUEUE_INIT(q);
    for (struct DbrRbNode* node = dbr_trader_first_order(trader);
         node != DBR_TRADER_END_ORDER; node = dbr_rbnode_next(node)) {
        struct DbrOrder* order = dbr_trader_order_entry(node);
        dbr_queue_insert_back(&q, &order->shared_node_);
    }
    rep.req_id = req_id;
    rep.type = DBR_ORDER_LIST_REP;
    rep.entity_list_rep.first = dbr_queue_first(&q);
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
 }

static DbrBool
sess_trade(DbrIden req_id, DbrTrader trader)
{
    struct DbrBody rep;

    // Copy to entity node.
    struct DbrQueue q = DBR_QUEUE_INIT(q);
    for (struct DbrRbNode* node = dbr_trader_first_trade(trader);
         node != DBR_TRADER_END_TRADE; node = dbr_rbnode_next(node)) {
        struct DbrExec* exec = dbr_trader_trade_entry(node);
        dbr_queue_insert_back(&q, &exec->shared_node_);
    }
    rep.req_id = req_id;
    rep.type = DBR_EXEC_LIST_REP;
    rep.entity_list_rep.first = dbr_queue_first(&q);
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_memb(DbrIden req_id, DbrTrader trader)
{
    struct DbrBody rep;

    // Copy to entity node.
    struct DbrQueue q = DBR_QUEUE_INIT(q);
    for (struct DbrRbNode* node = dbr_trader_first_memb(trader);
         node != DBR_TRADER_END_MEMB; node = dbr_rbnode_next(node)) {
        struct DbrMemb* memb = dbr_trader_memb_entry(node);
        dbr_queue_insert_back(&q, &memb->shared_node_);
    }
    rep.req_id = req_id;
    rep.type = DBR_MEMB_LIST_REP;
    rep.entity_list_rep.first = dbr_queue_first(&q);
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_posn(DbrIden req_id, DbrTrader trader)
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
            status_err(&rep, req_id);
            goto fail1;
        }
        // Copy each posn to entity node.
        for (struct DbrRbNode* pnode = dbr_accnt_first_posn(accnt);
             pnode != DBR_ACCNT_END_POSN; pnode = dbr_rbnode_next(pnode)) {
            struct DbrPosn* posn = dbr_accnt_posn_entry(pnode);
            dbr_queue_insert_back(&q, &posn->shared_node_);
        }
    }
    rep.req_id = req_id;
    rep.type = DBR_POSN_LIST_REP;
    rep.entity_list_rep.first = dbr_queue_first(&q);
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, true);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_prints("dbr_send_msg() failed");
    return false;
}

static DbrBool
sess_view(DbrIden req_id, DbrTrader trader)
{
    return true;
}

static DbrBool
sess_logon(DbrIden req_id, DbrTrader trader)
{
    struct DbrBody rep = { .req_id = req_id, .type = DBR_STATUS_REP,
                           .status_rep = { .num = 0, .msg = "" } };
    return dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false)
        && sess_trader(0, trader)
        && sess_accnt(0, trader)
        && sess_contr(0, trader)
        && sess_order(0, trader)
        && sess_trade(0, trader)
        && sess_memb(0, trader)
        && sess_posn(0, trader)
        && sess_view(0, trader);
}

static DbrBool
sess_logoff(DbrIden req_id, DbrTrader trader)
{
    return true;
}

static DbrBool
sess_heartbt(DbrIden req_id, DbrTrader trader)
{
    return true;
}

static DbrBool
place_order(const struct DbrBody* req, DbrTrader trader)
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
    return flush(req);
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_prints("dbr_send_msg() failed");
    return false;
}

static DbrBool
revise_order_id(const struct DbrBody* req, DbrTrader trader)
{
    struct DbrBody rep;

    const DbrIden id = req->revise_order_id_req.id;
    const DbrLots lots = req->revise_order_id_req.lots;

    struct DbrOrder* order = dbr_serv_revise_id(serv, trader, id, lots);
    if (!order) {
        status_err(&rep, req->req_id);
        goto fail1;
    }
    return flush(req);
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_prints("dbr_send_msg() failed");
    return false;
}

static DbrBool
revise_order_ref(const struct DbrBody* req, DbrTrader trader)
{
    struct DbrBody rep;

    const char* ref = req->revise_order_ref_req.ref;
    const DbrLots lots = req->revise_order_ref_req.lots;

    struct DbrOrder* order = dbr_serv_revise_ref(serv, trader, ref, lots);
    if (!order) {
        status_err(&rep, req->req_id);
        goto fail1;
    }
    return flush(req);
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_prints("dbr_send_msg() failed");
    return false;
}

static DbrBool
cancel_order_id(const struct DbrBody* req, DbrTrader trader)
{
    struct DbrBody rep;

    const DbrIden id = req->cancel_order_id_req.id;

    struct DbrOrder* order = dbr_serv_cancel_id(serv, trader, id);
    if (!order) {
        status_err(&rep, req->req_id);
        goto fail1;
    }
    return flush(req);
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_prints("dbr_send_msg() failed");
    return false;
}

static DbrBool
cancel_order_ref(const struct DbrBody* req, DbrTrader trader)
{
    struct DbrBody rep;

    const char* ref = req->cancel_order_ref_req.ref;

    struct DbrOrder* order = dbr_serv_cancel_ref(serv, trader, ref);
    if (!order) {
        status_err(&rep, req->req_id);
        goto fail1;
    }
    return flush(req);
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_prints("dbr_send_msg() failed");
    return false;
}

static DbrBool
ack_trade(const struct DbrBody* req, DbrTrader trader)
{
    struct DbrBody rep;

    const DbrIden id = req->ack_trade_req.id;

    if (!dbr_serv_ack_trade(serv, trader, id)) {
        status_err(&rep, req->req_id);
        goto fail1;
    }

    rep.req_id = req->req_id;
    rep.type = DBR_STATUS_REP;
    rep.status_rep.num = 0;
    rep.status_rep.msg[0] = '\0';
    const DbrBool ok = dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
        dbr_err_prints("dbr_send_msg() failed");
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
            dbr_err_prints("dbr_recv_msg() failed");
            goto fail1;
        }
        dbr_log_info("received '%d' from '%.16s'", req.body.type, req.head.trader);
        struct DbrBody rep;
        struct DbrRec* trec = find_rec_mnem(DBR_TRADER, req.head.trader);
        if (!trec) {
            status_setf(&rep, req.body.req_id, DBR_EINVAL, "no such trader '%.16s'",
                        req.head.trader);
            if (!dbr_send_msg(sock, req.head.trader, &rep, false))
                dbr_err_prints("dbr_send_msg() failed");
            continue;
        }
        DbrTrader trader = dbr_serv_trader(serv, trec);
        if (!trader) {
            status_err(&rep, req.body.req_id);
            if (!dbr_send_msg(sock, dbr_trader_rec(trader)->mnem, &rep, false))
                dbr_err_prints("dbr_send_msg() failed");
            continue;
        }
        switch (req.body.type) {
        case DBR_SESS_LOGON:
            sess_logon(req.body.req_id, trader);
            break;
        case DBR_SESS_LOGOFF:
            sess_logoff(req.body.req_id, trader);
            break;
        case DBR_SESS_HEARTBT:
            sess_heartbt(req.body.req_id, trader);
            break;
        case DBR_PLACE_ORDER_REQ:
            place_order(&req.body, trader);
            break;
        case DBR_REVISE_ORDER_ID_REQ:
            revise_order_id(&req.body, trader);
            break;
        case DBR_REVISE_ORDER_REF_REQ:
            revise_order_ref(&req.body, trader);
            break;
        case DBR_CANCEL_ORDER_ID_REQ:
            cancel_order_id(&req.body, trader);
            break;
        case DBR_CANCEL_ORDER_REF_REQ:
            cancel_order_ref(&req.body, trader);
            break;
        case DBR_ACK_TRADE_REQ:
            ack_trade(&req.body, trader);
            break;
        default:
            dbr_log_error("invalid body-type '%d'", req.body.type);
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

    DbrJourn journ = dbr_sqlstore_journ(store);
    DbrModel model = dbr_sqlstore_model(store);

    serv = dbr_serv_create(journ, model, pool);
    if (!serv) {
        dbr_err_prints("dbr_serv_create() failed");
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
