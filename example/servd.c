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
#define _XOPEN_SOURCE 700 // strnlen()

#include <dbr/accnt.h>
#include <dbr/book.h>
#include <dbr/err.h>
#include <dbr/serv.h>
#include <dbr/log.h>
#include <dbr/msg.h>
#include <dbr/pool.h>
#include <dbr/queue.h>
#include <dbr/sqlstore.h>
#include <dbr/trader.h>
#include <dbr/tree.h>
#include <dbr/util.h>

#include <zmq.h>

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { HBINT_IN = 2000 };

static DbrPool pool = NULL;
static DbrSqlStore store = NULL;
static DbrServ serv = NULL;
static void* ctx = NULL;
static void* router = NULL;
static void* pub = NULL;

static volatile sig_atomic_t quit = DBR_FALSE;

static void
free_view_list(struct DbrSlNode* first)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrView* view = dbr_shared_view_entry(node);
        node = node->next;
        dbr_pool_free_view(pool, view);
    }
}

static inline struct DbrRec*
find_rec_id(int type, DbrIden id)
{
    struct DbrSlNode* node = dbr_serv_find_rec_id(serv, type, id);
    return node != DBR_SERV_END_REC ? dbr_serv_rec_entry(node) : NULL;
}

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
    const DbrBool ok = dbr_send_msg(router, exec->c.trader.rec->mnem, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
}

static DbrBool
send_posnup(DbrIden req_id, struct DbrPosn* posn)
{
    struct DbrBody rep = { .req_id = req_id, .type = DBR_POSN_REP, .posn_rep.posn = posn };

    DbrAccnt accnt = dbr_serv_accnt(serv, posn->accnt.rec);
    for (struct DbrRbNode* node = dbr_accnt_first_memb(accnt);
         node != DBR_ACCNT_END_MEMB; node = dbr_rbnode_next(node)) {

        struct DbrMemb* memb = dbr_accnt_memb_entry(node);
        const char* trader = memb->trader.rec->mnem;
        if (zmq_send(router, trader, strnlen(trader, DBR_MNEM_MAX), ZMQ_SNDMORE) < 0) {
            dbr_err_setf(DBR_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
            dbr_err_print();
            goto fail1;
        }

        // FIXME: pack message once for all traders.
        if (!dbr_send_body(router, &rep, DBR_TRUE)) {
            dbr_err_prints("dbr_send_msg() failed");
            goto fail1;
        }
    }
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

static DbrBool
send_bookup(DbrIden req_id)
{
    struct DbrBody rep;

    struct DbrQueue q = DBR_QUEUE_INIT(q);
    for (struct DbrRbNode* node = dbr_serv_first_bookup(serv);
         node != DBR_SERV_END_BOOKUP; node = dbr_rbnode_next(node)) {
        struct DbrBook* book = dbr_serv_bookup_entry(node);

        struct DbrView* view = dbr_pool_alloc_view(pool);
        if (!view)
            goto fail1;
        dbr_view_init(view);

        dbr_book_view(book, view);
        dbr_queue_insert_back(&q, &view->shared_node_);
    }
    rep.req_id = req_id;
    rep.type = DBR_VIEW_LIST_REP;
    rep.view_list_rep.first = dbr_queue_first(&q);
    const DbrBool ok = dbr_send_body(pub, &rep, DBR_TRUE);
    free_view_list(q.first);
    if (!ok)
        dbr_err_prints("dbr_send_body() failed");
    return ok;
 fail1:
    free_view_list(q.first);
    return DBR_FALSE;
}

static DbrBool
flush(const struct DbrBody* req)
{
    const DbrIden req_id = req->req_id;
    {
        struct DbrSlNode* node = dbr_serv_first_exec(serv);
        assert(node != DBR_SERV_END_EXEC);

        struct DbrExec* exec = dbr_serv_exec_entry(node);
        if (!send_exec(req_id, exec))
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
        if (!send_posnup(0, posn))
            goto fail1;
    }

    if (!send_bookup(0))
        goto fail1;

    dbr_serv_clear(serv);
    return DBR_TRUE;
 fail1:
    dbr_serv_clear(serv);
    return DBR_FALSE;
}

static DbrBool
sess_trader(const char* sess, DbrIden req_id)
{
    struct DbrBody rep;

    struct DbrSlNode* first = dbr_serv_first_rec(serv, DBR_ENTITY_TRADER, NULL);

    rep.req_id = req_id;
    rep.type = DBR_TRADER_LIST_REP;
    rep.entity_list_rep.first = first;
    const DbrBool ok = dbr_send_msg(router, sess, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_accnt(const char* sess, DbrIden req_id)
{
    struct DbrBody rep;

    struct DbrSlNode* first = dbr_serv_first_rec(serv, DBR_ENTITY_ACCNT, NULL);

    rep.req_id = req_id;
    rep.type = DBR_ACCNT_LIST_REP;
    rep.entity_list_rep.first = first;
    const DbrBool ok = dbr_send_msg(router, sess, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_contr(const char* sess, DbrIden req_id)
{
    struct DbrBody rep;

    struct DbrSlNode* first = dbr_serv_first_rec(serv, DBR_ENTITY_CONTR, NULL);

    rep.req_id = req_id;
    rep.type = DBR_CONTR_LIST_REP;
    rep.entity_list_rep.first = first;
    const DbrBool ok = dbr_send_msg(router, sess, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_book(const char* sess, DbrIden req_id)
{
    struct DbrBody rep;

    struct DbrQueue q = DBR_QUEUE_INIT(q);
    for (struct DbrRbNode* node = dbr_serv_first_book(serv);
         node != DBR_SERV_END_BOOK; node = dbr_rbnode_next(node)) {
        struct DbrBook* book = dbr_serv_book_entry(node);

        struct DbrView* view = dbr_pool_alloc_view(pool);
        if (!view)
            goto fail1;
        dbr_view_init(view);

        dbr_book_view(book, view);
        dbr_queue_insert_back(&q, &view->shared_node_);
    }
    rep.req_id = req_id;
    rep.type = DBR_VIEW_LIST_REP;
    rep.view_list_rep.first = dbr_queue_first(&q);
    const DbrBool ok = dbr_send_msg(router, sess, &rep, DBR_TRUE);
    free_view_list(q.first);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
 fail1:
    free_view_list(q.first);
    return DBR_FALSE;
}

static DbrBool
sess_order(const char* sess, DbrIden req_id, DbrTrader trader)
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
    const DbrBool ok = dbr_send_msg(router, sess, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_exec(const char* sess, DbrIden req_id, DbrTrader trader)
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
    const DbrBool ok = dbr_send_msg(router, sess, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_memb(const char* sess, DbrIden req_id, DbrTrader trader)
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
    const DbrBool ok = dbr_send_msg(router, dbr_trader_rec(trader)->mnem, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_posn(const char* sess, DbrIden req_id, DbrTrader trader)
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
    const DbrBool ok = dbr_send_msg(router, dbr_trader_rec(trader)->mnem, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_send_msg(router, dbr_trader_rec(trader)->mnem, &rep, DBR_FALSE))
        dbr_err_prints("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
sess_open(const char* sess, const struct DbrBody* req)
{
    const DbrIden req_id = req->req_id;
    struct DbrBody rep = { .req_id = req_id, .type = DBR_SESS_OPEN,
                           .sess_open = { .hbint = HBINT_IN } };
    return dbr_send_msg(router, sess, &rep, DBR_FALSE)
        && sess_trader(sess, 0)
        && sess_accnt(sess, 0)
        && sess_contr(sess, 0)
        && sess_book(sess, 0);
}

static DbrBool
sess_close(const char* sess, const struct DbrBody* req)
{
    return DBR_TRUE;
}

static DbrBool
sess_logon(const char* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    const DbrIden tid = req->sess_logon.tid;
    struct DbrRec* trec = find_rec_id(DBR_ENTITY_TRADER, tid);
    if (!trec) {
        status_setf(&rep, req_id, DBR_EINVAL, "no such trader '%ld'", tid);
        goto fail1;
    }
    DbrTrader trader = dbr_serv_trader(serv, trec);
    if (!trader) {
        status_err(&rep, req_id);
        goto fail1;
    }
    rep.req_id = req_id;
    rep.type = DBR_SESS_LOGON;
    rep.sess_logon.tid = tid;

    return dbr_send_msg(router, sess, &rep, DBR_FALSE)
        && sess_order(sess, 0, trader)
        && sess_exec(sess, 0, trader)
        && sess_memb(sess, 0, trader)
        && sess_posn(sess, 0, trader);
 fail1:
    if (!dbr_send_msg(router, sess, &rep, DBR_FALSE))
        dbr_err_prints("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
sess_logoff(const char* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    const DbrIden tid = req->sess_logoff.tid;
    struct DbrRec* trec = find_rec_id(DBR_ENTITY_TRADER, tid);
    if (!trec) {
        status_setf(&rep, req_id, DBR_EINVAL, "no such trader '%ld'", tid);
        goto fail1;
    }
    rep.req_id = req_id;
    rep.type = DBR_SESS_LOGOFF;
    rep.sess_logoff.tid = tid;

    return dbr_send_msg(router, sess, &rep, DBR_FALSE);
 fail1:
    if (!dbr_send_msg(router, sess, &rep, DBR_FALSE))
        dbr_err_prints("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
sess_heartbt(const char* sess, const struct DbrBody* req)
{
    const DbrIden req_id = req->req_id;
    struct DbrBody rep = { .req_id = req_id, .type = DBR_SESS_HEARTBT };
    dbr_send_body(pub, &rep, DBR_FALSE);
    return dbr_send_msg(router, sess, &rep, DBR_FALSE);
}

static DbrBool
place_order(const char* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    const DbrIden tid = req->place_order_req.tid;
    struct DbrRec* trec = find_rec_id(DBR_ENTITY_TRADER, tid);
    if (!trec) {
        status_setf(&rep, req_id, DBR_EINVAL, "no such trader '%ld'", tid);
        goto fail1;
    }
    struct DbrRec* arec = find_rec_id(DBR_ENTITY_ACCNT, req->place_order_req.aid);
    if (!arec) {
        status_setf(&rep, req_id, DBR_EINVAL, "no such accnt '%ld'",
                    req->place_order_req.aid);
        goto fail1;
    }
    struct DbrRec* crec = find_rec_id(DBR_ENTITY_CONTR, req->place_order_req.cid);
    if (!crec) {
        status_setf(&rep, req_id, DBR_EINVAL, "no such contr '%ld'",
                    req->place_order_req.cid);
        goto fail1;
    }
    DbrAccnt accnt = dbr_serv_accnt(serv, arec);
    if (!accnt) {
        status_err(&rep, req_id);
        goto fail1;
    }
    struct DbrBook* book = dbr_serv_book(serv, crec, req->place_order_req.settl_date);
    if (!book) {
        status_err(&rep, req_id);
        goto fail1;
    }

    const char* ref = req->place_order_req.ref;
    const int action = req->place_order_req.action;
    const DbrTicks ticks = req->place_order_req.ticks;
    const DbrLots lots = req->place_order_req.lots;
    const DbrLots min_lots = req->place_order_req.min_lots;

    DbrTrader trader = NULL; // TODO
    struct DbrOrder* order = dbr_serv_place(serv, trader, accnt, book, ref, action, ticks,
                                            lots, min_lots);
    if (!order) {
        status_err(&rep, req_id);
        goto fail1;
    }
    return flush(req);
 fail1:
    if (!dbr_send_msg(router, sess, &rep, DBR_FALSE))
        dbr_err_prints("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
revise_order_id(const char* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    struct DbrRec* trec = find_rec_id(DBR_ENTITY_TRADER, req->revise_order_id_req.tid);
    if (!trec) {
        status_setf(&rep, req_id, DBR_EINVAL, "no such trader '%ld'",
                    req->revise_order_id_req.tid);
        goto fail1;
    }
    const DbrIden id = req->revise_order_id_req.id;
    const DbrLots lots = req->revise_order_id_req.lots;

    DbrTrader trader = NULL; // TODO
    struct DbrOrder* order = dbr_serv_revise_id(serv, trader, id, lots);
    if (!order) {
        status_err(&rep, req_id);
        goto fail1;
    }
    return flush(req);
 fail1:
    if (!dbr_send_msg(router, sess, &rep, DBR_FALSE))
        dbr_err_prints("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
revise_order_ref(const char* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    struct DbrRec* trec = find_rec_id(DBR_ENTITY_TRADER, req->revise_order_ref_req.tid);
    if (!trec) {
        status_setf(&rep, req_id, DBR_EINVAL, "no such trader '%ld'",
                    req->revise_order_ref_req.tid);
        goto fail1;
    }
    const char* ref = req->revise_order_ref_req.ref;
    const DbrLots lots = req->revise_order_ref_req.lots;

    DbrTrader trader = NULL; // TODO
    struct DbrOrder* order = dbr_serv_revise_ref(serv, trader, ref, lots);
    if (!order) {
        status_err(&rep, req_id);
        goto fail1;
    }
    return flush(req);
 fail1:
    if (!dbr_send_msg(router, sess, &rep, DBR_FALSE))
        dbr_err_prints("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
cancel_order_id(const char* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    struct DbrRec* trec = find_rec_id(DBR_ENTITY_TRADER, req->cancel_order_id_req.tid);
    if (!trec) {
        status_setf(&rep, req_id, DBR_EINVAL, "no such trader '%ld'",
                    req->cancel_order_id_req.tid);
        goto fail1;
    }
    const DbrIden id = req->cancel_order_id_req.id;

    DbrTrader trader = NULL; // TODO
    struct DbrOrder* order = dbr_serv_cancel_id(serv, trader, id);
    if (!order) {
        status_err(&rep, req_id);
        goto fail1;
    }
    return flush(req);
 fail1:
    if (!dbr_send_msg(router, sess, &rep, DBR_FALSE))
        dbr_err_prints("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
cancel_order_ref(const char* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    struct DbrRec* trec = find_rec_id(DBR_ENTITY_TRADER, req->cancel_order_ref_req.tid);
    if (!trec) {
        status_setf(&rep, req_id, DBR_EINVAL, "no such trader '%ld'",
                    req->cancel_order_ref_req.tid);
        goto fail1;
    }
    const char* ref = req->cancel_order_ref_req.ref;

    DbrTrader trader = NULL; // TODO
    struct DbrOrder* order = dbr_serv_cancel_ref(serv, trader, ref);
    if (!order) {
        status_err(&rep, req_id);
        goto fail1;
    }
    return flush(req);
 fail1:
    if (!dbr_send_msg(router, sess, &rep, DBR_FALSE))
        dbr_err_prints("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
ack_trade(const char* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    struct DbrRec* trec = find_rec_id(DBR_ENTITY_TRADER, req->ack_trade_req.id);
    if (!trec) {
        status_setf(&rep, req_id, DBR_EINVAL, "no such trader '%ld'",
                    req->ack_trade_req.id);
        goto fail1;
    }
    const DbrIden id = req->ack_trade_req.id;

    DbrTrader trader = NULL; // TODO
    if (!dbr_serv_ack_trade(serv, trader, id)) {
        status_err(&rep, req_id);
        goto fail1;
    }

    rep.req_id = req_id;
    rep.type = DBR_STATUS_REP;
    rep.status_rep.num = 0;
    rep.status_rep.msg[0] = '\0';
    const DbrBool ok = dbr_send_msg(router, sess, &rep, DBR_FALSE);
    if (!ok)
        dbr_err_prints("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_send_msg(router, sess, &rep, DBR_FALSE))
        dbr_err_prints("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
run(void)
{
    while (!quit) {
        struct DbrMsg req;
        dbr_log_info("receiving...");
        if (!dbr_recv_msg(router, pool, &req)) {
            if (dbr_err_num() == DBR_EINTR)
                continue;
            dbr_err_prints("dbr_recv_msg() failed");
            goto fail1;
        }
        dbr_log_info("received '%d' from '%.16s'", req.body.type, req.head.sess);
#if 0
        struct DbrBody rep;
        struct DbrRec* trec = find_rec_mnem(DBR_ENTITY_TRADER, req.head.trader);
        if (!trec) {
            status_setf(&rep, req.body.req_id, DBR_EINVAL, "no such trader '%.16s'",
                        req.head.trader);
            if (!dbr_send_msg(router, req.head.trader, &rep, DBR_FALSE))
                dbr_err_prints("dbr_send_msg() failed");
            continue;
        }
        DbrTrader trader = dbr_serv_trader(serv, trec);
        if (!trader) {
            status_err(&rep, req.body.req_id);
            if (!dbr_send_msg(router, dbr_trader_rec(trader)->mnem, &rep, DBR_FALSE))
                dbr_err_prints("dbr_send_msg() failed");
            continue;
        }
#endif
        switch (req.body.type) {
        case DBR_SESS_OPEN:
            sess_open(req.head.sess, &req.body);
            break;
        case DBR_SESS_CLOSE:
            sess_close(req.head.sess, &req.body);
            break;
        case DBR_SESS_LOGON:
            sess_logon(req.head.sess, &req.body);
            break;
        case DBR_SESS_LOGOFF:
            sess_logoff(req.head.sess, &req.body);
            break;
        case DBR_SESS_HEARTBT:
            sess_heartbt(req.head.sess, &req.body);
            break;
        case DBR_PLACE_ORDER_REQ:
            place_order(req.head.sess, &req.body);
            break;
        case DBR_REVISE_ORDER_ID_REQ:
            revise_order_id(req.head.sess, &req.body);
            break;
        case DBR_REVISE_ORDER_REF_REQ:
            revise_order_ref(req.head.sess, &req.body);
            break;
        case DBR_CANCEL_ORDER_ID_REQ:
            cancel_order_id(req.head.sess, &req.body);
            break;
        case DBR_CANCEL_ORDER_REF_REQ:
            cancel_order_ref(req.head.sess, &req.body);
            break;
        case DBR_ACK_TRADE_REQ:
            ack_trade(req.head.sess, &req.body);
            break;
        default:
            dbr_log_error("invalid body-type '%d'", req.body.type);
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
    if (!ctx) {
        dbr_err_setf(DBR_EIO, "zmq_ctx_new() failed: %s", zmq_strerror(zmq_errno()));
        dbr_err_print();
        goto exit4;
    }

    router = zmq_socket(ctx, ZMQ_ROUTER);
    if (!router) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        dbr_err_print();
        goto exit5;
    }

    if (zmq_bind(router, "tcp://*:3270") < 0) {
        dbr_err_setf(DBR_EIO, "zmq_bind() failed: %s", zmq_strerror(zmq_errno()));
        dbr_err_print();
        goto exit6;
    }

    pub = zmq_socket(ctx, ZMQ_PUB);
    if (!pub) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        dbr_err_print();
        goto exit6;
    }

    if (zmq_bind(pub, "tcp://*:3271") < 0) {
        dbr_err_setf(DBR_EIO, "zmq_bind() failed: %s", zmq_strerror(zmq_errno()));
        dbr_err_print();
        goto exit7;
    }

    struct sigaction action;
    action.sa_handler = sighandler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    if (!run())
        goto exit7;

    dbr_log_info("exiting...");
    status = 0;
 exit7:
    zmq_close(pub);
 exit6:
    zmq_close(router);
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
