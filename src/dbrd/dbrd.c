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

#include "config.h"

#include <dbr/accnt.h>
#include <dbr/book.h>
#include <dbr/daemon.h>
#include <dbr/err.h>
#include <dbr/serv.h>
#include <dbr/log.h>
#include <dbr/msg.h>
#include <dbr/pool.h>
#include <dbr/prioq.h>
#include <dbr/queue.h>
#include <dbr/sess.h>
#include <dbr/sqljourn.h>
#include <dbr/sqlmodel.h>
#include <dbr/trader.h>
#include <dbr/tree.h>
#include <dbr/util.h>
#include <dbr/zmqjourn.h>

#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h> // getopt()

#include <uuid/uuid.h>
#include <zmq.h>

enum {
    TRSOCK
};

// Other constants.

enum {
    // Non-negative timer ids are reserved for internal use.

    MDTMR = -1,
    MDINT = 2000,
    MDTMOUT = (MDINT * 3) / 2,

    TRINT = 5000,
    TRTMOUT = (TRINT * 3) / 2
};

static void* ctx = NULL;
static DbrJourn journ = NULL;
static DbrPool pool = NULL;
static DbrServ serv = NULL;
static void* mdsock = NULL;
static void* trsock = NULL;
static struct DbrPrioq prioq = { 0 };

static inline DbrBool
is_hbtmr(DbrKey tmr)
{
    // Heartbeat timers are even numbered.
    return (tmr & 0x1) == 0;
}

static inline DbrKey
sess_to_hbtmr(struct DbrSess* sess)
{
    return (DbrKey)sess;
}

static inline struct DbrSess*
hbtmr_to_sess(DbrKey hbtmr)
{
    return (struct DbrSess*)hbtmr;
}

static inline DbrKey
sess_to_trtmr(struct DbrSess* sess)
{
    return ((DbrKey)sess) + 1;
}

static inline struct DbrSess*
trtmr_to_sess(DbrKey trtmr)
{
    return (struct DbrSess*)(trtmr - 1);
}

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

static DbrTrader
get_trader(DbrIden tid)
{
    struct DbrRec* trec = find_rec_id(DBR_ENTITY_TRADER, tid);
    if (!trec) {
        dbr_err_setf(DBR_EINVAL, "no such trader '%ld'", tid);
        return NULL;
    }
    return dbr_serv_trader(serv, trec);
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
send_exec(struct DbrSess* sess, DbrIden req_id, struct DbrExec* exec)
{
    struct DbrBody rep = { .req_id = req_id, .type = DBR_EXEC_REP, .exec_rep.exec = exec };
    const DbrBool ok = dbr_send_msg(trsock, sess->uuid, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_perror("dbr_send_msg() failed");
    return ok;
}

static DbrBool
flush_posnup(struct DbrPosn* posn)
{
    static long marker = 0;
    ++marker;

    struct DbrBody rep = { .req_id = 0, .type = DBR_POSN_REP, .posn_rep.posn = posn };

    // Send position update to each trader that is a member of this account.

    DbrAccnt accnt = dbr_serv_accnt(serv, posn->accnt.rec);
    for (struct DbrRbNode* node = dbr_accnt_first_memb(accnt);
         node != DBR_ACCNT_END_MEMB; node = dbr_rbnode_next(node)) {

        struct DbrMemb* memb = dbr_accnt_memb_entry(node);
        DbrTrader trader = memb->trader.rec->trader.state;
        struct DbrSess* other = dbr_trader_sess(trader);
        // Next if trader not logged-on or the position has already been sent.
        if (!other || other->marker_ == marker)
            continue;

        // Set marker to record that position update has been sent on this session.
        other->marker_ = marker;

        if (zmq_send(trsock, other->uuid, 16, ZMQ_SNDMORE) < 0) {
            dbr_err_printf(DBR_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
            goto fail1;
        }

        // FIXME: pack message once for all traders.
        if (!dbr_send_body(trsock, &rep, DBR_TRUE)) {
            dbr_err_perror("dbr_send_body() failed");
            goto fail1;
        }
    }
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

static DbrBool
mdflush(DbrMillis now)
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

        dbr_book_view(book, view, now);
        dbr_queue_insert_back(&q, &view->shared_node_);
    }

    rep.req_id = 0;
    rep.type = DBR_VIEW_LIST_REP;
    rep.view_list_rep.first = dbr_queue_first(&q);

    const DbrBool ok = dbr_send_body(mdsock, &rep, DBR_TRUE);
    free_view_list(q.first);
    if (!ok)
        dbr_err_perror("dbr_send_body() failed");
    dbr_serv_mdclear(serv);
    return ok;
 fail1:
    free_view_list(q.first);
    dbr_serv_mdclear(serv);
    return DBR_FALSE;
}

static DbrBool
trflush(struct DbrSess* sess, const struct DbrBody* req)
{
    const DbrIden req_id = req->req_id;
    {
        struct DbrSlNode* node = dbr_serv_first_exec(serv);
        assert(node != DBR_SERV_END_EXEC);

        struct DbrExec* exec = dbr_serv_exec_entry(node);
        if (!send_exec(sess, req_id, exec))
            goto fail1;

        for (node = dbr_slnode_next(node);
             node != DBR_SERV_END_EXEC; node = dbr_slnode_next(node)) {
            struct DbrExec* exec = dbr_serv_exec_entry(node);
            // Send execution on associated trader's session.
            DbrTrader trader = exec->c.trader.rec->trader.state;
            // Null if trader is not logged-on.
            struct DbrSess* other = dbr_trader_sess(trader);
            if (other && !send_exec(other, 0, exec))
                goto fail1;
        }
    }

    for (struct DbrRbNode* node = dbr_serv_first_posnup(serv);
         node != DBR_SERV_END_POSNUP; node = dbr_rbnode_next(node)) {
        struct DbrPosn* posn = dbr_serv_posnup_entry(node);
        if (!flush_posnup(posn))
            goto fail1;
    }

    dbr_serv_trclear(serv);
    return DBR_TRUE;
 fail1:
    dbr_serv_trclear(serv);
    return DBR_FALSE;
}

static DbrBool
sess_trader(struct DbrSess* sess, DbrIden req_id)
{
    struct DbrBody rep;

    struct DbrSlNode* first = dbr_serv_first_rec(serv, DBR_ENTITY_TRADER, NULL);

    rep.req_id = req_id;
    rep.type = DBR_TRADER_LIST_REP;
    rep.entity_list_rep.first = first;
    const DbrBool ok = dbr_send_msg(trsock, sess->uuid, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_perror("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_accnt(struct DbrSess* sess, DbrIden req_id)
{
    struct DbrBody rep;

    struct DbrSlNode* first = dbr_serv_first_rec(serv, DBR_ENTITY_ACCNT, NULL);

    rep.req_id = req_id;
    rep.type = DBR_ACCNT_LIST_REP;
    rep.entity_list_rep.first = first;
    const DbrBool ok = dbr_send_msg(trsock, sess->uuid, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_perror("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_contr(struct DbrSess* sess, DbrIden req_id)
{
    struct DbrBody rep;

    struct DbrSlNode* first = dbr_serv_first_rec(serv, DBR_ENTITY_CONTR, NULL);

    rep.req_id = req_id;
    rep.type = DBR_CONTR_LIST_REP;
    rep.entity_list_rep.first = first;
    const DbrBool ok = dbr_send_msg(trsock, sess->uuid, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_perror("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_book(struct DbrSess* sess, DbrIden req_id, DbrMillis now)
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

        dbr_book_view(book, view, now);
        dbr_queue_insert_back(&q, &view->shared_node_);
    }
    rep.req_id = req_id;
    rep.type = DBR_VIEW_LIST_REP;
    rep.view_list_rep.first = dbr_queue_first(&q);
    const DbrBool ok = dbr_send_msg(trsock, sess->uuid, &rep, DBR_TRUE);
    free_view_list(q.first);
    if (!ok)
        dbr_err_perror("dbr_send_msg() failed");
    return ok;
 fail1:
    free_view_list(q.first);
    return DBR_FALSE;
}

static DbrBool
sess_order(struct DbrSess* sess, DbrIden req_id, DbrTrader trader)
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
    const DbrBool ok = dbr_send_msg(trsock, sess->uuid, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_perror("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_exec(struct DbrSess* sess, DbrIden req_id, DbrTrader trader)
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
    const DbrBool ok = dbr_send_msg(trsock, sess->uuid, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_perror("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_memb(struct DbrSess* sess, DbrIden req_id, DbrTrader trader)
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
    const DbrBool ok = dbr_send_msg(trsock, sess->uuid, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_perror("dbr_send_msg() failed");
    return ok;
}

static DbrBool
sess_posn(struct DbrSess* sess, DbrIden req_id, DbrTrader trader)
{
    struct DbrBody rep;

    struct DbrQueue q = DBR_QUEUE_INIT(q);

    // Send positions for each account that the trader is a member of.

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
        const int subs = dbr_sess_subs(sess, accnt);
        if (subs > 1)
            continue;
        // This logon represents the first subscription to this account, so the initial positions
        // for this account must be sent.
        assert(subs == 1);
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
    const DbrBool ok = dbr_send_msg(trsock, sess->uuid, &rep, DBR_TRUE);
    if (!ok)
        dbr_err_perror("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_send_msg(trsock, sess->uuid, &rep, DBR_FALSE))
        dbr_err_perror("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
sess_open(struct DbrSess* sess, const struct DbrBody* req, DbrMillis now)
{
    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&prioq, dbr_prioq_size(&prioq) + 2))
        goto fail1;

    sess->hbint = req->sess_open.hbint;

    const DbrIden req_id = req->req_id;
    struct DbrBody rep = { .req_id = req_id, .type = DBR_SESS_OPEN,
                           .sess_open = { .hbint = TRINT } };
    return dbr_send_msg(trsock, sess->uuid, &rep, DBR_FALSE)
        && sess_trader(sess, 0)
        && sess_accnt(sess, 0)
        && sess_contr(sess, 0)
        && sess_book(sess, 0, now)
        && dbr_prioq_push(&prioq, sess_to_hbtmr(sess), now + sess->hbint)
        && dbr_prioq_push(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
 fail1:
    return DBR_FALSE;
}

static DbrBool
sess_close(struct DbrSess* sess, DbrIden req_id)
{
    dbr_prioq_remove(&prioq, sess_to_hbtmr(sess));
    dbr_prioq_remove(&prioq, sess_to_trtmr(sess));

    for (struct DbrRbNode* node = dbr_sess_first_trader(sess);
         node != DBR_SESS_END_TRADER; node = dbr_rbnode_next(node)) {
        DbrTrader trader = dbr_sess_trader_entry(node);
        dbr_sess_logoff(sess, trader, DBR_FALSE);
        // TODO: implicit logoff?
    }
    struct DbrBody rep = { .req_id = req_id, .type = DBR_SESS_CLOSE };
    return dbr_send_msg(trsock, sess->uuid, &rep, DBR_FALSE);
}

static DbrBool
sess_logon(struct DbrSess* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    const DbrIden tid = req->sess_logon.tid;
    DbrTrader trader = get_trader(tid);
    if (!trader || !dbr_sess_logon(sess, trader)) {
        status_err(&rep, req_id);
        goto fail1;
    }

    rep.req_id = req_id;
    rep.type = DBR_SESS_LOGON;
    rep.sess_logon.tid = tid;
    return dbr_send_msg(trsock, sess->uuid, &rep, DBR_FALSE)
        && sess_order(sess, 0, trader)
        && sess_exec(sess, 0, trader)
        && sess_memb(sess, 0, trader)
        && sess_posn(sess, 0, trader);
 fail1:
    if (!dbr_send_msg(trsock, sess->uuid, &rep, DBR_FALSE))
        dbr_err_perror("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
sess_logoff(struct DbrSess* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    const DbrIden tid = req->sess_logoff.tid;
    DbrTrader trader = get_trader(tid);
    if (!trader) {
        status_err(&rep, req_id);
        goto fail1;
    }
    dbr_sess_logoff(sess, trader, DBR_FALSE);

    rep.req_id = req_id;
    rep.type = DBR_SESS_LOGOFF;
    rep.sess_logoff.tid = tid;
    return dbr_send_msg(trsock, sess->uuid, &rep, DBR_FALSE);
 fail1:
    if (!dbr_send_msg(trsock, sess->uuid, &rep, DBR_FALSE))
        dbr_err_perror("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
place_order(struct DbrSess* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    const DbrIden tid = req->place_order_req.tid;
    DbrTrader trader = get_trader(tid);
    if (!trader) {
        status_err(&rep, req_id);
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

    struct DbrOrder* order = dbr_serv_place(serv, trader, accnt, book, ref, action, ticks,
                                            lots, min_lots);
    if (!order) {
        status_err(&rep, req_id);
        goto fail1;
    }
    return trflush(sess, req);
 fail1:
    if (!dbr_send_msg(trsock, sess->uuid, &rep, DBR_FALSE))
        dbr_err_perror("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
revise_order_id(struct DbrSess* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    const DbrIden tid = req->revise_order_id_req.tid;
    DbrTrader trader = get_trader(tid);
    if (!trader) {
        status_err(&rep, req_id);
        goto fail1;
    }
    const DbrIden id = req->revise_order_id_req.id;
    const DbrLots lots = req->revise_order_id_req.lots;

    struct DbrOrder* order = dbr_serv_revise_id(serv, trader, id, lots);
    if (!order) {
        status_err(&rep, req_id);
        goto fail1;
    }
    return trflush(sess, req);
 fail1:
    if (!dbr_send_msg(trsock, sess->uuid, &rep, DBR_FALSE))
        dbr_err_perror("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
revise_order_ref(struct DbrSess* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    const DbrIden tid = req->revise_order_ref_req.tid;
    DbrTrader trader = get_trader(tid);
    if (!trader) {
        status_err(&rep, req_id);
        goto fail1;
    }
    const char* ref = req->revise_order_ref_req.ref;
    const DbrLots lots = req->revise_order_ref_req.lots;

    struct DbrOrder* order = dbr_serv_revise_ref(serv, trader, ref, lots);
    if (!order) {
        status_err(&rep, req_id);
        goto fail1;
    }
    return trflush(sess, req);
 fail1:
    if (!dbr_send_msg(trsock, sess->uuid, &rep, DBR_FALSE))
        dbr_err_perror("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
cancel_order_id(struct DbrSess* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    const DbrIden tid = req->cancel_order_id_req.tid;
    DbrTrader trader = get_trader(tid);
    if (!trader) {
        status_err(&rep, req_id);
        goto fail1;
    }
    const DbrIden id = req->cancel_order_id_req.id;

    struct DbrOrder* order = dbr_serv_cancel_id(serv, trader, id);
    if (!order) {
        status_err(&rep, req_id);
        goto fail1;
    }
    return trflush(sess, req);
 fail1:
    if (!dbr_send_msg(trsock, sess->uuid, &rep, DBR_FALSE))
        dbr_err_perror("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
cancel_order_ref(struct DbrSess* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    const DbrIden tid = req->cancel_order_ref_req.tid;
    DbrTrader trader = get_trader(tid);
    if (!trader) {
        status_err(&rep, req_id);
        goto fail1;
    }
    const char* ref = req->cancel_order_ref_req.ref;

    struct DbrOrder* order = dbr_serv_cancel_ref(serv, trader, ref);
    if (!order) {
        status_err(&rep, req_id);
        goto fail1;
    }
    return trflush(sess, req);
 fail1:
    if (!dbr_send_msg(trsock, sess->uuid, &rep, DBR_FALSE))
        dbr_err_perror("dbr_send_msg() failed");
    return DBR_FALSE;
}

static DbrBool
ack_trade(struct DbrSess* sess, const struct DbrBody* req)
{
    struct DbrBody rep;

    const DbrIden req_id = req->req_id;
    const DbrIden tid = req->ack_trade_req.tid;
    DbrTrader trader = get_trader(tid);
    if (!trader) {
        status_err(&rep, req_id);
        goto fail1;
    }
    const DbrIden id = req->ack_trade_req.id;

    if (!dbr_serv_ack_trade(serv, trader, id)) {
        status_err(&rep, req_id);
        goto fail1;
    }

    rep.req_id = req_id;
    rep.type = DBR_STATUS_REP;
    rep.status_rep.num = 0;
    rep.status_rep.msg[0] = '\0';
    const DbrBool ok = dbr_send_msg(trsock, sess->uuid, &rep, DBR_FALSE);
    if (!ok)
        dbr_err_perror("dbr_send_msg() failed");
    return ok;
 fail1:
    if (!dbr_send_msg(trsock, sess->uuid, &rep, DBR_FALSE))
        dbr_err_perror("dbr_send_msg() failed");
    return DBR_FALSE;
}

enum {
    NONE,
    HANGUP,
    QUIT,
};

static volatile sig_atomic_t action = NONE;

static DbrJourn
factory(void* arg)
{
    return dbr_sqljourn_create((const char*)arg);
}

static DbrBool
load(DbrServ serv, const char* path)
{
    DbrBool ret;

    DbrModel model = dbr_sqlmodel_create(path);
    if (dbr_likely(model)) {
        ret = dbr_serv_load(serv, model);
        dbr_model_destroy(model);
    } else
        ret = DBR_FALSE;

    return ret;
}

static DbrBool
open_logfile(const char* path)
{
    int fd = open(path, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP);
    if (fd < 0) {
        dbr_err_setf(DBR_EIO, "open() failed: %s", strerror(errno));
        return DBR_FALSE;
    }
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    close(fd);
    return DBR_TRUE;
}

static void
sighandler(int signum)
{
    switch (signum) {
    case SIGHUP:
        action = HANGUP;
        break;
    case SIGINT:
    case SIGTERM:
        action = QUIT;
        break;
    };
}

static DbrBool
run(struct Config* config)
{
    zmq_pollitem_t items[] = {
        { .socket = trsock, .events = ZMQ_POLLIN }
    };

    DbrMillis now = dbr_millis();
    dbr_prioq_push(&prioq, MDTMR, MDINT * ((now / MDINT) + 1));

    while (action != QUIT) {

        if (action == HANGUP) {
            action = NONE;
            // Exit when hang-up is received on non-daemon process.
            if (!config->daemon)
                break;
            if (config->logfile[0] != '\0') {
                dbr_log_info("reopening logfile on hangup");
                fflush(stdout);
                if (!open_logfile(config->logfile)) {
                    dbr_err_perror("open_logfile() failed");
                    goto fail1;
                }
            }
        }

        char buf[DBR_UUID_MAX + 1];
        DbrMillis ms = -1;
        const struct DbrElem* elem;
        while ((elem = dbr_prioq_top(&prioq))) {
            if (elem->key > now) {
                // Millis until next timeout.
                ms = elem->key - now;
                break;
            }
            const DbrKey key = elem->key;
            const DbrIden id = elem->id;
            dbr_prioq_pop(&prioq);

            if (id == MDTMR) {
                // Cannot fail due to pop.
                dbr_prioq_push(&prioq, id, key + MDINT);
                if (!mdflush(key))
                    goto fail1;
                // Next heartbeat may have already expired.
            } else if (is_hbtmr(id)) {
                struct DbrSess* sess = hbtmr_to_sess(id);
                uuid_unparse_lower(sess->uuid, buf);
                dbr_log_info("session heartbeat to '%.36s'", buf);
                dbr_prioq_push(&prioq, id, key + sess->hbint);
                struct DbrBody body = { .req_id = 0, .type = DBR_SESS_HEARTBT };
                if (!dbr_send_msg(trsock, sess->uuid, &body, DBR_FALSE))
                    goto fail1;
            } else {
                assert(id & 0x1);
                struct DbrSess* sess = trtmr_to_sess(id);
                uuid_unparse_lower(sess->uuid, buf);
                dbr_log_info("session timeout on '%.36s'", buf);
                sess_close(sess, 0);
            }
        }

        const int nevents = zmq_poll(items, 1, ms);
        if (nevents < 0) {
            if (zmq_errno() == EINTR)
                continue;
            dbr_err_setf(DBR_EIO, "zmq_poll() failed: %s", zmq_strerror(zmq_errno()));
            goto fail1;
        }
        // Current time after slow operation.
        now = dbr_millis();

        if ((items[TRSOCK].revents & ZMQ_POLLIN)) {
            struct DbrMsg req;
            if (!dbr_recv_msg(trsock, pool, &req)) {
                if (dbr_err_num() == DBR_EINTR)
                    continue;
                dbr_err_perror("dbr_recv_msg() failed");
                goto fail1;
            }
            struct DbrBody rep;
            struct DbrSess* sess = dbr_serv_sess(serv, req.uuid);
            if (!sess) {
                status_err(&rep, req.body.req_id);
                if (!dbr_send_msg(trsock, req.uuid, &rep, DBR_FALSE))
                    dbr_err_perror("dbr_send_msg() failed");
                continue;
            }
            uuid_unparse_lower(req.uuid, buf);
            switch (req.body.type) {
            case DBR_SESS_OPEN:
                dbr_log_info("session open from '%.36s'", buf);
                sess_open(sess, &req.body, now);
                break;
            case DBR_SESS_CLOSE:
                uuid_unparse_lower(sess->uuid, buf);
                dbr_log_info("session close from '%.36s'", buf);
                sess_close(sess, req.body.req_id);
                break;
            case DBR_SESS_LOGON:
                dbr_log_info("session logon from '%.36s'", buf);
                sess_logon(sess, &req.body);
                dbr_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case DBR_SESS_LOGOFF:
                dbr_log_info("session logoff '%.36s'", buf);
                sess_logoff(sess, &req.body);
                dbr_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case DBR_SESS_HEARTBT:
                dbr_log_info("session heartbeat from '%.36s'", buf);
                dbr_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case DBR_PLACE_ORDER_REQ:
                dbr_log_info("place order request from '%.36s'", buf);
                place_order(sess, &req.body);
                dbr_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case DBR_REVISE_ORDER_ID_REQ:
                dbr_log_info("revise order by id request from '%.36s'", buf);
                revise_order_id(sess, &req.body);
                dbr_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case DBR_REVISE_ORDER_REF_REQ:
                dbr_log_info("revise order by ref request from '%.36s'", buf);
                revise_order_ref(sess, &req.body);
                dbr_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case DBR_CANCEL_ORDER_ID_REQ:
                dbr_log_info("cancel order by id request from '%.36s'", buf);
                cancel_order_id(sess, &req.body);
                dbr_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case DBR_CANCEL_ORDER_REF_REQ:
                dbr_log_info("cancel order by ref request from '%.36s'", buf);
                cancel_order_ref(sess, &req.body);
                dbr_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case DBR_ACK_TRADE_REQ:
                dbr_log_info("acknowledge trade request from '%.36s'", buf);
                ack_trade(sess, &req.body);
                dbr_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            default:
                dbr_log_error("invalid body-type '%d'", req.body.type);
                break;
            }
        }
    }
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

int
main(int argc, char* argv[])
{
    // Reasonable config defaults.
    struct Config config = {
        .daemon = DBR_FALSE,
        .prefix = "",
        .bankfile = "doobry.bin",
        .dbfile = "doobry.db",
        .logfile = "",
        .journsize = 1 * 1024 * 1024,
        .poolsize = 1 * 1024 * 1024
    };

    int status = EXIT_FAILURE;

    char c;
    while ((c = getopt(argc, argv, "f:")) != -1) {
        switch (c) {
        case 'f':
            if (!parse_file(optarg, &config)) {
                dbr_err_perror("parse_file() failed");
                goto exit1;
            }
            break;
        }
    }
    argc -= optind;
    argv += optind;

    dbr_log_setlogger(dbr_log_serv);

    if (config.daemon) {
        if (dbr_daemon("/", 0027) < 0) {
            dbr_err_perror("dbr_daemon() failed");
            goto exit1;
        }
        if (config.logfile[0] != '\0') {
            if (!open_logfile(config.logfile)) {
                dbr_err_perror("open_logfile() failed");
                goto exit1;
            }
        } else {
            // Daemon uses syslog by default.
            openlog("dbrd", LOG_PID | LOG_NDELAY, LOG_LOCAL0);
#if DBR_DEBUG_LEVEL >= 1
            setlogmask(LOG_UPTO(LOG_DEBUG));
#else  // DBR_DEBUG_LEVEL < 1
            setlogmask(LOG_UPTO(LOG_INFO));
#endif // DBR_DEBUG_LEVEL < 1
            dbr_log_setlogger(dbr_log_syslog);
        }
    }

    dbr_log_notice("server started");
    dbr_log_info("daemon:    %s", config.daemon ? "yes" : "no");
    dbr_log_info("bankfile:  %s", config.bankfile);
    dbr_log_info("dbfile:    %s", config.dbfile);
    dbr_log_info("logfile:   %s", config.logfile);
    dbr_log_info("journsize: %d", config.journsize);
    dbr_log_info("poolsize:  %d", config.poolsize);

    ctx = zmq_ctx_new();
    if (!ctx) {
        dbr_err_printf(DBR_EIO, "zmq_ctx_new() failed: %s", zmq_strerror(zmq_errno()));
        goto exit1;
    }

    journ = dbr_zmqjourn_create(ctx, config.journsize, factory, config.dbfile);
    if (!journ) {
        dbr_err_perror("dbr_sqljourn_create() failed");
        goto exit2;
    }

    pool = dbr_pool_create(config.poolsize);
    if (!pool) {
        dbr_err_perror("dbr_pool_create() failed");
        goto exit3;
    }

    serv = dbr_serv_create(config.bankfile, journ, pool);
    if (!serv) {
        dbr_err_perror("dbr_serv_create() failed");
        goto exit4;
    }

    if (!load(serv, config.dbfile)) {
        dbr_err_perror("load() failed");
        goto exit5;
    }

    mdsock = zmq_socket(ctx, ZMQ_PUB);
    if (!mdsock) {
        dbr_err_printf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto exit5;
    }
    const int opt = 0;
    zmq_setsockopt(mdsock, ZMQ_LINGER, &opt, sizeof(opt));

    if (zmq_bind(mdsock, "tcp://*:3270") < 0) {
        dbr_err_printf(DBR_EIO, "zmq_bind() failed: %s", zmq_strerror(zmq_errno()));
        goto exit6;
    }

    trsock = zmq_socket(ctx, ZMQ_ROUTER);
    if (!trsock) {
        dbr_err_printf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto exit6;
    }
    zmq_setsockopt(trsock, ZMQ_LINGER, &opt, sizeof(opt));

    if (zmq_bind(trsock, "tcp://*:3271") < 0) {
        dbr_err_printf(DBR_EIO, "zmq_bind() failed: %s", zmq_strerror(zmq_errno()));
        goto exit7;
    }

    if (!dbr_prioq_init(&prioq))
        goto exit7;

    struct sigaction action;
    action.sa_handler = sighandler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGHUP, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    if (!run(&config)) {
        dbr_err_perror("run() failed");
        goto exit8;
    }

    status = EXIT_SUCCESS;
 exit8:
    dbr_prioq_term(&prioq);
 exit7:
    zmq_close(trsock);
 exit6:
    zmq_close(mdsock);
 exit5:
    dbr_serv_destroy(serv);
 exit4:
    dbr_pool_destroy(pool);
 exit3:
    dbr_journ_destroy(journ);
 exit2:
    zmq_ctx_destroy(ctx);
 exit1:
    return status;
}
