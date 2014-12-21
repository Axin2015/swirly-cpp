/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */

#include "config.h"

#include <sc/fir/sqljourn.h>
#include <sc/fir/sqlmodel.h>

#include <sc/fig/accnt.h>
#include <sc/fig/book.h>
#include <sc/fig/serv.h>
#include <sc/fig/sess.h>

#include <sc/elm/msg.h>
#include <sc/elm/pool.h>
#include <sc/elm/zmqjourn.h>

#include <sc/ash/daemon.h>
#include <sc/ash/err.h>
#include <sc/ash/log.h>
#include <sc/ash/prioq.h>
#include <sc/ash/queue.h>
#include <sc/ash/tree.h>
#include <sc/ash/util.h>

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

static void* zctx = NULL;
static ScJourn journ = NULL;
static ScPool pool = NULL;
static ScServ serv = NULL;
static void* mdsock = NULL;
static void* trsock = NULL;
static struct ScPrioq prioq = { 0 };

static inline ScBool
is_hbtmr(ScIden tmr)
{
    // Heartbeat timers are even numbered.
    return (tmr & 0x1) == 0;
}

static inline ScIden
sess_to_hbtmr(struct ScSess* sess)
{
    return (ScIden)sess;
}

static inline struct ScSess*
hbtmr_to_sess(ScIden hbtmr)
{
    return (struct ScSess*)hbtmr;
}

static inline ScIden
sess_to_trtmr(struct ScSess* sess)
{
    return ((ScIden)sess) + 1;
}

static inline struct ScSess*
trtmr_to_sess(ScIden trtmr)
{
    return (struct ScSess*)(trtmr - 1);
}

static void
free_view_list(struct ScSlNode* first)
{
    struct ScSlNode* node = first;
    while (node) {
        struct ScView* view = sc_shared_view_entry(node);
        node = node->next;
        sc_pool_free_view(pool, view);
    }
}

static inline struct ScRec*
find_rec_id(int type, ScIden id)
{
    struct ScSlNode* node = sc_serv_find_rec_id(serv, type, id);
    return node != SC_SERV_END_REC ? sc_serv_rec_entry(node) : NULL;
}

static ScAccnt
get_accnt(ScIden aid)
{
    struct ScRec* arec = find_rec_id(SC_ENTITY_ACCNT, aid);
    if (!arec) {
        sc_err_setf(SC_EINVAL, "no such account '%ld'", aid);
        return NULL;
    }
    return sc_serv_accnt(serv, arec);
}

static void
status_err(struct ScBody* body, ScIden req_id, int sid)
{
    body->req_id = req_id;
    body->sid = sid;
    body->type = SC_STATUS_REP;
    body->status_rep.num = sc_err_num();
    strncpy(body->status_rep.msg, sc_err_msg(), SC_ERRMSG_MAX);
    sc_log_error("error: %.128s", body->status_rep.msg);
}

static void
status_setf(struct ScBody* body, ScIden req_id, int sid, int num, const char* format, ...)
{
    body->req_id = req_id;
    body->sid = sid;
    body->type = SC_STATUS_REP;
    body->status_rep.num = num;

    va_list args;
    va_start(args, format);
    vsnprintf(body->status_rep.msg, SC_ERRMSG_MAX, format, args);
    va_end(args);
    sc_log_error("error: %.128s", body->status_rep.msg);
}

static ScBool
send_exec(struct ScSess* sess, ScIden req_id, struct ScExec* exec)
{
    const int sid = sess->sid;
    struct ScBody rep = { .req_id = req_id, .sid = sid, .type = SC_EXEC_REP,
                           .exec_rep.exec = exec };
    const ScBool ok = sc_send_msg(trsock, sess->uuid, &rep, SC_TRUE);
    if (!ok)
        sc_err_perror("sc_send_msg() failed");
    return ok;
}

static ScBool
flush_posnup(struct ScPosn* posn)
{
    static long marker = 0;
    ++marker;

    struct ScBody rep = { .req_id = 0, .sid = 0, .type = SC_POSN_REP, .posn_rep.posn = posn };

    // Send position update to each trader that is a member of this account.

    ScAccnt accnt = sc_serv_accnt(serv, posn->accnt.rec);
    for (struct ScRbNode* node = sc_accnt_first_trader(accnt);
         node != SC_ACCNT_END_TRADER; node = sc_rbnode_next(node)) {

        struct ScPerm* perm = sc_accnt_trader_entry(node);
        ScAccnt trader = perm->trader.rec->accnt.state;
        struct ScSess* other = sc_accnt_sess(trader);
        // Next if trader not logged-on or the position has already been sent.
        if (!other || other->marker_ == marker)
            continue;

        // Set marker to record that position update has been sent on this session.
        other->marker_ = marker;

        if (zmq_send(trsock, other->uuid, 16, ZMQ_SNDMORE) < 0) {
            sc_err_printf(SC_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
            goto fail1;
        }

        rep.sid = other->sid;
        if (!sc_send_body(trsock, &rep, SC_TRUE)) {
            sc_err_perror("sc_send_body() failed");
            goto fail1;
        }
    }
    return SC_TRUE;
 fail1:
    return SC_FALSE;
}

static ScBool
mdflush(ScMillis now)
{
    struct ScBody rep;

    struct ScQueue q = SC_QUEUE_INIT(q);
    for (struct ScRbNode* node = sc_serv_first_bookup(serv);
         node != SC_SERV_END_BOOKUP; node = sc_rbnode_next(node)) {
        struct ScBook* book = sc_serv_bookup_entry(node);

        struct ScView* view = sc_pool_alloc_view(pool);
        if (!view)
            goto fail1;
        sc_view_init(view);

        sc_book_view(book, view, now);
        sc_queue_insert_back(&q, &view->shared_node_);
    }

    rep.req_id = 0;
    rep.sid = 0;
    rep.type = SC_VIEW_LIST_REP;
    rep.view_list_rep.first = sc_queue_first(&q);

    const ScBool ok = sc_send_body(mdsock, &rep, SC_TRUE);
    free_view_list(q.first);
    if (!ok)
        sc_err_perror("sc_send_body() failed");
    sc_serv_clear_md(serv);
    return ok;
 fail1:
    free_view_list(q.first);
    sc_serv_clear_md(serv);
    return SC_FALSE;
}

static ScBool
trflush(struct ScSess* sess, const struct ScBody* req)
{
    const ScIden req_id = req->req_id;
    {
        struct ScSlNode* node = sc_serv_first_exec(serv);
        assert(node != SC_SERV_END_EXEC);

        struct ScExec* exec = sc_serv_exec_entry(node);
        if (!send_exec(sess, req_id, exec))
            goto fail1;

        for (node = sc_slnode_next(node);
             node != SC_SERV_END_EXEC; node = sc_slnode_next(node)) {
            struct ScExec* exec = sc_serv_exec_entry(node);
            // Send execution on associated trader's session.
            ScAccnt trader = exec->i.trader.rec->accnt.state;
            // Null if trader is not logged-on.
            struct ScSess* other = sc_accnt_sess(trader);
            if (other && !send_exec(other, 0, exec))
                goto fail1;
        }
    }

    for (struct ScRbNode* node = sc_serv_first_posnup(serv);
         node != SC_SERV_END_POSNUP; node = sc_rbnode_next(node)) {
        struct ScPosn* posn = sc_serv_posnup_entry(node);
        if (!flush_posnup(posn))
            goto fail1;
    }

    sc_serv_clear_tr(serv);
    return SC_TRUE;
 fail1:
    sc_serv_clear_tr(serv);
    return SC_FALSE;
}

static ScBool
sess_accnt(struct ScSess* sess, ScIden req_id)
{
    struct ScBody rep;

    struct ScSlNode* first = sc_serv_first_rec(serv, SC_ENTITY_ACCNT, NULL);

    rep.req_id = req_id;
    rep.sid = sess->sid;
    rep.type = SC_ACCNT_LIST_REP;
    rep.entity_list_rep.first = first;
    const ScBool ok = sc_send_msg(trsock, sess->uuid, &rep, SC_TRUE);
    if (!ok)
        sc_err_perror("sc_send_msg() failed");
    return ok;
}

static ScBool
sess_contr(struct ScSess* sess, ScIden req_id)
{
    struct ScBody rep;

    struct ScSlNode* first = sc_serv_first_rec(serv, SC_ENTITY_CONTR, NULL);

    rep.req_id = req_id;
    rep.sid = sess->sid;
    rep.type = SC_CONTR_LIST_REP;
    rep.entity_list_rep.first = first;
    const ScBool ok = sc_send_msg(trsock, sess->uuid, &rep, SC_TRUE);
    if (!ok)
        sc_err_perror("sc_send_msg() failed");
    return ok;
}

static ScBool
sess_trader(struct ScSess* sess, ScIden req_id, ScAccnt trader)
{
    struct ScBody rep;

    // Copy to entity node.
    struct ScQueue q = SC_QUEUE_INIT(q);
    for (struct ScRbNode* node = sc_accnt_first_trader(trader);
         node != SC_ACCNT_END_TRADER; node = sc_rbnode_next(node)) {
        struct ScPerm* perm = sc_accnt_trader_entry(node);
        sc_queue_insert_back(&q, &perm->shared_node_);
    }
    rep.req_id = req_id;
    rep.sid = sess->sid;
    rep.type = SC_TRADER_LIST_REP;
    rep.entity_list_rep.first = sc_queue_first(&q);
    const ScBool ok = sc_send_msg(trsock, sess->uuid, &rep, SC_TRUE);
    if (!ok)
        sc_err_perror("sc_send_msg() failed");
    return ok;
}

static ScBool
sess_giveup(struct ScSess* sess, ScIden req_id, ScAccnt trader)
{
    struct ScBody rep;

    // Copy to entity node.
    struct ScQueue q = SC_QUEUE_INIT(q);
    for (struct ScRbNode* node = sc_accnt_first_giveup(trader);
         node != SC_ACCNT_END_GIVEUP; node = sc_rbnode_next(node)) {
        struct ScPerm* perm = sc_accnt_giveup_entry(node);
        sc_queue_insert_back(&q, &perm->shared_node_);
    }
    rep.req_id = req_id;
    rep.sid = sess->sid;
    rep.type = SC_GIVEUP_LIST_REP;
    rep.entity_list_rep.first = sc_queue_first(&q);
    const ScBool ok = sc_send_msg(trsock, sess->uuid, &rep, SC_TRUE);
    if (!ok)
        sc_err_perror("sc_send_msg() failed");
    return ok;
}

static ScBool
sess_order(struct ScSess* sess, ScIden req_id, ScAccnt trader)
{
    struct ScBody rep;

    // Copy to entity node.
    struct ScQueue q = SC_QUEUE_INIT(q);
    for (struct ScRbNode* node = sc_accnt_first_order(trader);
         node != SC_ACCNT_END_ORDER; node = sc_rbnode_next(node)) {
        struct ScOrder* order = sc_accnt_order_entry(node);
        sc_queue_insert_back(&q, &order->shared_node_);
    }
    rep.req_id = req_id;
    rep.sid = sess->sid;
    rep.type = SC_ORDER_LIST_REP;
    rep.entity_list_rep.first = sc_queue_first(&q);
    const ScBool ok = sc_send_msg(trsock, sess->uuid, &rep, SC_TRUE);
    if (!ok)
        sc_err_perror("sc_send_msg() failed");
    return ok;
}

static ScBool
sess_exec(struct ScSess* sess, ScIden req_id, ScAccnt trader)
{
    struct ScBody rep;

    // Copy to entity node.
    struct ScQueue q = SC_QUEUE_INIT(q);
    for (struct ScRbNode* node = sc_accnt_first_trade(trader);
         node != SC_ACCNT_END_TRADE; node = sc_rbnode_next(node)) {
        struct ScExec* exec = sc_accnt_trade_entry(node);
        sc_queue_insert_back(&q, &exec->shared_node_);
    }
    rep.req_id = req_id;
    rep.sid = sess->sid;
    rep.type = SC_EXEC_LIST_REP;
    rep.entity_list_rep.first = sc_queue_first(&q);
    const ScBool ok = sc_send_msg(trsock, sess->uuid, &rep, SC_TRUE);
    if (!ok)
        sc_err_perror("sc_send_msg() failed");
    return ok;
}

static ScBool
sess_posn(struct ScSess* sess, ScIden req_id, ScAccnt trader)
{
    struct ScBody rep;

    struct ScQueue q = SC_QUEUE_INIT(q);

    // Send positions for each giveup that the account (trader) belongs to.

    // The initial positions must be sent if this is the this first subscription to this trader.
    if (sc_sess_subs(sess, trader) == 1) {
        // Copy each posn to entity node.
        for (struct ScRbNode* pnode = sc_accnt_first_posn(trader);
             pnode != SC_ACCNT_END_POSN; pnode = sc_rbnode_next(pnode)) {
            struct ScPosn* posn = sc_accnt_posn_entry(pnode);
            sc_queue_insert_back(&q, &posn->shared_node_);
        }
    }
    // For each giveup.
    for (struct ScRbNode* gnode = sc_accnt_first_giveup(trader);
         gnode != SC_ACCNT_END_GIVEUP; gnode = sc_rbnode_next(gnode)) {
        struct ScPerm* perm = sc_accnt_giveup_entry(gnode);
        struct ScRec* grec = perm->giveup.rec;
        ScAccnt giveup = sc_serv_accnt(serv, grec);
        if (!giveup) {
            status_err(&rep, req_id, sess->sid);
            goto fail1;
        }
        // Is this session already subscribed to this giveup?
        const int subs = sc_sess_subs(sess, giveup);
        if (subs > 1)
            continue;
        // This logon represents the first subscription to this giveup, so the initial positions for
        // this giveup must be sent.
        assert(subs == 1);
        // Copy each posn to entity node.
        for (struct ScRbNode* pnode = sc_accnt_first_posn(giveup);
             pnode != SC_ACCNT_END_POSN; pnode = sc_rbnode_next(pnode)) {
            struct ScPosn* posn = sc_accnt_posn_entry(pnode);
            sc_queue_insert_back(&q, &posn->shared_node_);
        }
    }
    rep.req_id = req_id;
    rep.sid = sess->sid;
    rep.type = SC_POSN_LIST_REP;
    rep.entity_list_rep.first = sc_queue_first(&q);
    const ScBool ok = sc_send_msg(trsock, sess->uuid, &rep, SC_TRUE);
    if (!ok)
        sc_err_perror("sc_send_msg() failed");
    return ok;
 fail1:
    if (!sc_send_msg(trsock, sess->uuid, &rep, SC_FALSE))
        sc_err_perror("sc_send_msg() failed");
    return SC_FALSE;
}

static ScBool
sess_book(struct ScSess* sess, ScIden req_id, ScMillis now)
{
    struct ScBody rep;

    struct ScQueue q = SC_QUEUE_INIT(q);
    for (struct ScRbNode* node = sc_serv_first_book(serv);
         node != SC_SERV_END_BOOK; node = sc_rbnode_next(node)) {
        struct ScBook* book = sc_serv_book_entry(node);

        struct ScView* view = sc_pool_alloc_view(pool);
        if (!view)
            goto fail1;
        sc_view_init(view);

        sc_book_view(book, view, now);
        sc_queue_insert_back(&q, &view->shared_node_);
    }
    rep.req_id = req_id;
    rep.sid = sess->sid;
    rep.type = SC_VIEW_LIST_REP;
    rep.view_list_rep.first = sc_queue_first(&q);
    const ScBool ok = sc_send_msg(trsock, sess->uuid, &rep, SC_TRUE);
    free_view_list(q.first);
    if (!ok)
        sc_err_perror("sc_send_msg() failed");
    return ok;
 fail1:
    free_view_list(q.first);
    return SC_FALSE;
}

static ScBool
sess_open(struct ScSess* sess, const struct ScBody* req, ScMillis now)
{
    // Reserve so that push cannot fail after send.
    if (!sc_prioq_reserve(&prioq, sc_prioq_size(&prioq) + 2))
        goto fail1;

    const ScIden req_id = req->req_id;
    const int sid = req->sid;

    // Open represents the start of a new session.
    sess->sid = sid;
    sess->hbint = req->sess_open.hbint;

    struct ScBody rep = { .req_id = req_id, .sid = sid, .type = SC_SESS_OPEN,
                           .sess_open = { .hbint = TRINT } };
    return sc_send_msg(trsock, sess->uuid, &rep, SC_FALSE)
        && sess_accnt(sess, 0)
        && sess_contr(sess, 0)
        && sess_book(sess, 0, now)
        && sc_prioq_push(&prioq, sess_to_hbtmr(sess), now + sess->hbint)
        && sc_prioq_push(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
 fail1:
    return SC_FALSE;
}

static ScBool
sess_close(struct ScSess* sess, ScIden req_id)
{
    sc_prioq_remove(&prioq, sess_to_hbtmr(sess));
    sc_prioq_remove(&prioq, sess_to_trtmr(sess));

    for (struct ScRbNode* node = sc_sess_first_accnt(sess);
         node != SC_SESS_END_ACCNT; node = sc_rbnode_next(node)) {
        ScAccnt trader = sc_sess_accnt_entry(node);
        sc_sess_logoff(sess, trader);
        // TODO: implicit logoff?
    }
    const int sid = sess->sid;
    struct ScBody rep = { .req_id = req_id, .sid = sid, .type = SC_SESS_CLOSE };
    return sc_send_msg(trsock, sess->uuid, &rep, SC_FALSE);
}

static ScBool
sess_logon(struct ScSess* sess, const struct ScBody* req)
{
    struct ScBody rep;

    const ScIden req_id = req->req_id;
    const int sid = sess->sid;
    const ScIden aid = req->sess_logon.aid;
    ScAccnt accnt = get_accnt(aid);
    if (!accnt || !sc_sess_logon(sess, accnt)) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }

    rep.req_id = req_id;
    rep.sid = sid;
    rep.type = SC_SESS_LOGON;
    rep.sess_logon.aid = aid;
    return sc_send_msg(trsock, sess->uuid, &rep, SC_FALSE)
        && sess_trader(sess, 0, accnt)
        && sess_giveup(sess, 0, accnt)
        && sess_order(sess, 0, accnt)
        && sess_exec(sess, 0, accnt)
        && sess_posn(sess, 0, accnt);
 fail1:
    if (!sc_send_msg(trsock, sess->uuid, &rep, SC_FALSE))
        sc_err_perror("sc_send_msg() failed");
    return SC_FALSE;
}

static ScBool
sess_logoff(struct ScSess* sess, const struct ScBody* req)
{
    struct ScBody rep;

    const ScIden req_id = req->req_id;
    const int sid = sess->sid;
    const ScIden aid = req->sess_logoff.aid;
    ScAccnt accnt = get_accnt(aid);
    if (!accnt) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }
    sc_sess_logoff(sess, accnt);

    rep.req_id = req_id;
    rep.sid = sid;
    rep.type = SC_SESS_LOGOFF;
    rep.sess_logoff.aid = aid;
    return sc_send_msg(trsock, sess->uuid, &rep, SC_FALSE);
 fail1:
    if (!sc_send_msg(trsock, sess->uuid, &rep, SC_FALSE))
        sc_err_perror("sc_send_msg() failed");
    return SC_FALSE;
}

static ScBool
place_order(struct ScSess* sess, const struct ScBody* req)
{
    struct ScBody rep;

    const ScIden req_id = req->req_id;
    const int sid = sess->sid;
    const ScIden tid = req->place_order_req.tid;
    ScAccnt trader = get_accnt(tid);
    if (!trader) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }
    struct ScRec* grec = find_rec_id(SC_ENTITY_ACCNT, req->place_order_req.gid);
    if (!grec) {
        status_setf(&rep, req_id, sid, SC_EINVAL, "no such accnt '%ld'",
                    req->place_order_req.gid);
        goto fail1;
    }
    struct ScRec* crec = find_rec_id(SC_ENTITY_CONTR, req->place_order_req.cid);
    if (!crec) {
        status_setf(&rep, req_id, sid, SC_EINVAL, "no such contr '%ld'",
                    req->place_order_req.cid);
        goto fail1;
    }
    ScAccnt giveup = sc_serv_accnt(serv, grec);
    if (!giveup) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }
    struct ScBook* book = sc_serv_book(serv, crec, req->place_order_req.settl_day);
    if (!book) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }

    const char* ref = req->place_order_req.ref;
    const int action = req->place_order_req.action;
    const ScTicks ticks = req->place_order_req.ticks;
    const ScLots lots = req->place_order_req.lots;
    const ScLots min_lots = req->place_order_req.min_lots;

    struct ScOrder* order = sc_serv_place(serv, trader, giveup, book, ref, action, ticks,
                                            lots, min_lots);
    if (!order) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }
    return trflush(sess, req);
 fail1:
    if (!sc_send_msg(trsock, sess->uuid, &rep, SC_FALSE))
        sc_err_perror("sc_send_msg() failed");
    return SC_FALSE;
}

static ScBool
revise_order_id(struct ScSess* sess, const struct ScBody* req)
{
    struct ScBody rep;

    const ScIden req_id = req->req_id;
    const int sid = sess->sid;
    const ScIden tid = req->revise_order_id_req.tid;
    ScAccnt trader = get_accnt(tid);
    if (!trader) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }
    const ScIden id = req->revise_order_id_req.id;
    const ScLots lots = req->revise_order_id_req.lots;

    struct ScOrder* order = sc_serv_revise_id(serv, trader, id, lots);
    if (!order) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }
    return trflush(sess, req);
 fail1:
    if (!sc_send_msg(trsock, sess->uuid, &rep, SC_FALSE))
        sc_err_perror("sc_send_msg() failed");
    return SC_FALSE;
}

static ScBool
revise_order_ref(struct ScSess* sess, const struct ScBody* req)
{
    struct ScBody rep;

    const ScIden req_id = req->req_id;
    const int sid = sess->sid;
    const ScIden tid = req->revise_order_ref_req.tid;
    ScAccnt trader = get_accnt(tid);
    if (!trader) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }
    const char* ref = req->revise_order_ref_req.ref;
    const ScLots lots = req->revise_order_ref_req.lots;

    struct ScOrder* order = sc_serv_revise_ref(serv, trader, ref, lots);
    if (!order) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }
    return trflush(sess, req);
 fail1:
    if (!sc_send_msg(trsock, sess->uuid, &rep, SC_FALSE))
        sc_err_perror("sc_send_msg() failed");
    return SC_FALSE;
}

static ScBool
cancel_order_id(struct ScSess* sess, const struct ScBody* req)
{
    struct ScBody rep;

    const ScIden req_id = req->req_id;
    const int sid = sess->sid;
    const ScIden tid = req->cancel_order_id_req.tid;
    ScAccnt trader = get_accnt(tid);
    if (!trader) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }
    const ScIden id = req->cancel_order_id_req.id;

    struct ScOrder* order = sc_serv_cancel_id(serv, trader, id);
    if (!order) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }
    return trflush(sess, req);
 fail1:
    if (!sc_send_msg(trsock, sess->uuid, &rep, SC_FALSE))
        sc_err_perror("sc_send_msg() failed");
    return SC_FALSE;
}

static ScBool
cancel_order_ref(struct ScSess* sess, const struct ScBody* req)
{
    struct ScBody rep;

    const ScIden req_id = req->req_id;
    const int sid = sess->sid;
    const ScIden tid = req->cancel_order_ref_req.tid;
    ScAccnt trader = get_accnt(tid);
    if (!trader) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }
    const char* ref = req->cancel_order_ref_req.ref;

    struct ScOrder* order = sc_serv_cancel_ref(serv, trader, ref);
    if (!order) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }
    return trflush(sess, req);
 fail1:
    if (!sc_send_msg(trsock, sess->uuid, &rep, SC_FALSE))
        sc_err_perror("sc_send_msg() failed");
    return SC_FALSE;
}

static ScBool
ack_trade(struct ScSess* sess, const struct ScBody* req)
{
    struct ScBody rep;

    const ScIden req_id = req->req_id;
    const int sid = sess->sid;
    const ScIden tid = req->ack_trade_req.tid;
    ScAccnt trader = get_accnt(tid);
    if (!trader) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }
    const ScIden id = req->ack_trade_req.id;

    if (!sc_serv_ack_trade(serv, trader, id)) {
        status_err(&rep, req_id, sid);
        goto fail1;
    }

    rep.req_id = req_id;
    rep.sid = sid;
    rep.type = SC_STATUS_REP;
    rep.status_rep.num = 0;
    rep.status_rep.msg[0] = '\0';
    const ScBool ok = sc_send_msg(trsock, sess->uuid, &rep, SC_FALSE);
    if (!ok)
        sc_err_perror("sc_send_msg() failed");
    return ok;
 fail1:
    if (!sc_send_msg(trsock, sess->uuid, &rep, SC_FALSE))
        sc_err_perror("sc_send_msg() failed");
    return SC_FALSE;
}

enum {
    NONE,
    HANGUP,
    QUIT,
};

static volatile sig_atomic_t action = NONE;

static ScJourn
factory(void* arg)
{
    return sc_sqljourn_create((const char*)arg);
}

static ScBool
load(ScServ serv, const char* path)
{
    ScBool ret;

    ScModel model = sc_sqlmodel_create(path);
    if (sc_likely(model)) {
        ret = sc_serv_load(serv, model);
        sc_model_destroy(model);
    } else
        ret = SC_FALSE;

    return ret;
}

static ScBool
open_logfile(const char* path)
{
    int fd = open(path, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP);
    if (fd < 0) {
        sc_err_setf(SC_EIO, "open() failed: %s", strerror(errno));
        return SC_FALSE;
    }
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    close(fd);
    return SC_TRUE;
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

static ScBool
run(struct Config* config)
{
    zmq_pollitem_t items[] = {
        { .socket = trsock, .events = ZMQ_POLLIN }
    };

    ScMillis now = sc_millis();
    sc_prioq_push(&prioq, MDTMR, MDINT * ((now / MDINT) + 1));

    while (action != QUIT) {

        if (action == HANGUP) {
            action = NONE;
            // Exit when hang-up is received on non-daemon process.
            if (!config->daemon)
                break;
            if (config->logfile[0] != '\0') {
                sc_log_info("reopening logfile on hangup");
                fflush(stdout);
                if (!open_logfile(config->logfile)) {
                    sc_err_perror("open_logfile() failed");
                    goto fail1;
                }
            }
        }

        char buf[SC_UUID_MAX + 1];
        ScMillis ms = -1;
        const struct ScElem* elem;
        while ((elem = sc_prioq_top(&prioq))) {
            if (elem->key > now) {
                // Millis until next timeout.
                ms = elem->key - now;
                break;
            }
            const ScKey key = elem->key;
            const ScIden id = elem->id;
            sc_prioq_pop(&prioq);

            if (id == MDTMR) {
                // Cannot fail due to pop.
                sc_prioq_push(&prioq, id, key + MDINT);
                if (!mdflush(key))
                    goto fail1;
                // Next heartbeat may have already expired.
            } else if (is_hbtmr(id)) {
                struct ScSess* sess = hbtmr_to_sess(id);
                uuid_unparse_lower(sess->uuid, buf);
                sc_prioq_push(&prioq, id, key + sess->hbint);
                struct ScBody body = { .req_id = 0, .sid = sess->sid, .type = SC_SESS_HEARTBT };
                if (!sc_send_msg(trsock, sess->uuid, &body, SC_FALSE))
                    goto fail1;
            } else {
                assert(id & 0x1);
                struct ScSess* sess = trtmr_to_sess(id);
                uuid_unparse_lower(sess->uuid, buf);
                sc_log_info("session timeout on '%.36s'", buf);
                sess_close(sess, 0);
            }
        }

        const int nevents = zmq_poll(items, 1, ms);
        if (nevents < 0) {
            if (zmq_errno() == EINTR)
                continue;
            sc_err_setf(SC_EIO, "zmq_poll() failed: %s", zmq_strerror(zmq_errno()));
            goto fail1;
        }
        // Current time after slow operation.
        now = sc_millis();

        if ((items[TRSOCK].revents & ZMQ_POLLIN)) {
            struct ScMsg req;
            if (!sc_recv_msg(trsock, pool, &req)) {
                if (sc_err_num() == SC_EINTR)
                    continue;
                sc_err_perror("sc_recv_msg() failed");
                goto fail1;
            }
            struct ScBody rep;
            struct ScSess* sess = sc_serv_sess(serv, req.uuid);
            if (!sess) {
                status_err(&rep, req.body.req_id, sess->sid);
                if (!sc_send_msg(trsock, req.uuid, &rep, SC_FALSE))
                    sc_err_perror("sc_send_msg() failed");
                continue;
            }
            uuid_unparse_lower(req.uuid, buf);
            switch (req.body.type) {
            case SC_SESS_OPEN:
                sc_log_info("session open from '%.36s'", buf);
                sess_open(sess, &req.body, now);
                break;
            case SC_SESS_CLOSE:
                uuid_unparse_lower(sess->uuid, buf);
                sc_log_info("session close from '%.36s'", buf);
                sess_close(sess, req.body.req_id);
                break;
            case SC_SESS_LOGON:
                sc_log_info("session logon from '%.36s'", buf);
                sess_logon(sess, &req.body);
                sc_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case SC_SESS_LOGOFF:
                sc_log_info("session logoff '%.36s'", buf);
                sess_logoff(sess, &req.body);
                sc_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case SC_SESS_HEARTBT:
                sc_log_debug3("session heartbeat from '%.36s'", buf);
                sc_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case SC_PLACE_ORDER_REQ:
                sc_log_info("place order request from '%.36s'", buf);
                place_order(sess, &req.body);
                sc_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case SC_REVISE_ORDER_ID_REQ:
                sc_log_info("revise order by id request from '%.36s'", buf);
                revise_order_id(sess, &req.body);
                sc_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case SC_REVISE_ORDER_REF_REQ:
                sc_log_info("revise order by ref request from '%.36s'", buf);
                revise_order_ref(sess, &req.body);
                sc_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case SC_CANCEL_ORDER_ID_REQ:
                sc_log_info("cancel order by id request from '%.36s'", buf);
                cancel_order_id(sess, &req.body);
                sc_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case SC_CANCEL_ORDER_REF_REQ:
                sc_log_info("cancel order by ref request from '%.36s'", buf);
                cancel_order_ref(sess, &req.body);
                sc_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            case SC_ACK_TRADE_REQ:
                sc_log_info("acknowledge trade request from '%.36s'", buf);
                ack_trade(sess, &req.body);
                sc_prioq_replace(&prioq, sess_to_trtmr(sess), now + TRTMOUT);
                break;
            default:
                sc_log_error("invalid body-type '%d'", req.body.type);
                break;
            }
        }
    }
    return SC_TRUE;
 fail1:
    return SC_FALSE;
}

int
main(int argc, char* argv[])
{
    // Reasonable config defaults.
    struct Config config = {
        .daemon = SC_FALSE,
        .prefix = "",
        .bankfile = "twirly.bin",
        .dbfile = "twirly.db",
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
                sc_err_perror("parse_file() failed");
                goto exit1;
            }
            break;
        }
    }
    argc -= optind;
    argv += optind;

    sc_log_setlogger(sc_log_serv);

    if (config.daemon) {
        if (sc_daemon("/", 0027) < 0) {
            sc_err_perror("sc_daemon() failed");
            goto exit1;
        }
        if (config.logfile[0] != '\0') {
            if (!open_logfile(config.logfile)) {
                sc_err_perror("open_logfile() failed");
                goto exit1;
            }
        } else {
            // Daemon uses syslog by default.
            openlog("scd", LOG_PID | LOG_NDELAY, LOG_LOCAL0);
#if SC_DEBUG_LEVEL >= 1
            setlogmask(LOG_UPTO(LOG_DEBUG));
#else  // SC_DEBUG_LEVEL < 1
            setlogmask(LOG_UPTO(LOG_INFO));
#endif // SC_DEBUG_LEVEL < 1
            sc_log_setlogger(sc_log_syslog);
        }
    }

    sc_log_notice("server started");
    sc_log_info("daemon:    %s", config.daemon ? "yes" : "no");
    sc_log_info("bankfile:  %s", config.bankfile);
    sc_log_info("dbfile:    %s", config.dbfile);
    sc_log_info("logfile:   %s", config.logfile);
    sc_log_info("journsize: %d", config.journsize);
    sc_log_info("poolsize:  %d", config.poolsize);

    zctx = zmq_ctx_new();
    if (!zctx) {
        sc_err_printf(SC_EIO, "zmq_ctx_new() failed: %s", zmq_strerror(zmq_errno()));
        goto exit1;
    }

    journ = sc_zmqjourn_create(zctx, config.journsize, factory, config.dbfile);
    if (!journ) {
        sc_err_perror("sc_sqljourn_create() failed");
        goto exit2;
    }

    pool = sc_pool_create(config.poolsize);
    if (!pool) {
        sc_err_perror("sc_pool_create() failed");
        goto exit3;
    }

    serv = sc_serv_create(config.bankfile, journ, pool);
    if (!serv) {
        sc_err_perror("sc_serv_create() failed");
        goto exit4;
    }

    if (!load(serv, config.dbfile)) {
        sc_err_perror("load() failed");
        goto exit5;
    }

    mdsock = zmq_socket(zctx, ZMQ_PUB);
    if (!mdsock) {
        sc_err_printf(SC_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto exit5;
    }
    const int opt = 0;
    zmq_setsockopt(mdsock, ZMQ_LINGER, &opt, sizeof(opt));

    if (zmq_bind(mdsock, "tcp://*:3270") < 0) {
        sc_err_printf(SC_EIO, "zmq_bind() failed: %s", zmq_strerror(zmq_errno()));
        goto exit6;
    }

    trsock = zmq_socket(zctx, ZMQ_ROUTER);
    if (!trsock) {
        sc_err_printf(SC_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto exit6;
    }
    zmq_setsockopt(trsock, ZMQ_LINGER, &opt, sizeof(opt));

    if (zmq_bind(trsock, "tcp://*:3271") < 0) {
        sc_err_printf(SC_EIO, "zmq_bind() failed: %s", zmq_strerror(zmq_errno()));
        goto exit7;
    }

    if (!sc_prioq_init(&prioq))
        goto exit7;

    struct sigaction action;
    action.sa_handler = sighandler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGHUP, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    if (!run(&config)) {
        sc_err_perror("run() failed");
        goto exit8;
    }

    status = EXIT_SUCCESS;
 exit8:
    sc_prioq_term(&prioq);
 exit7:
    zmq_close(trsock);
 exit6:
    zmq_close(mdsock);
 exit5:
    sc_serv_destroy(serv);
 exit4:
    sc_pool_destroy(pool);
 exit3:
    sc_journ_destroy(journ);
 exit2:
    zmq_ctx_destroy(zctx);
 exit1:
    return status;
}
