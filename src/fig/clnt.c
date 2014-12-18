/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "clnt.h"

#include "accnt.h"

#include <sc/fig/sess.h>

#include <sc/elm/msg.h>

#include <sc/ash/err.h>
#include <sc/ash/log.h>
#include <sc/ash/util.h>

#include <stdlib.h> // malloc()
#include <string.h> // strncpy()

#include <uuid/uuid.h>

static void
term_state(struct ScRec* rec)
{
    switch (rec->type) {
    case SC_ENTITY_ACCNT:
        fig_accnt_term(rec);
        break;
    }
}

static void
free_views(struct ScTree* views, ScPool pool)
{
    assert(views);
    struct ScRbNode* node;
    while ((node = views->root)) {
        struct ScView* view = sc_clnt_view_entry(node);
        sc_tree_remove(views, node);
        sc_pool_free_view(pool, view);
    }
}

#if SC_DEBUG_LEVEL >= 1
SC_EXTERN void
fig_clnt_log_state(unsigned state)
{
    if (state == 0) {
        sc_log_debug1("state: READY");
        return;
    }
    char buf[sizeof("DELTA_WAIT|OPEN_WAIT|ACCNT_WAIT|CONTR_WAIT|SNAP_WAIT|CLOSE_WAIT|CLOSED")] = "";
    if (state & FIG_DELTA_WAIT)
        strcat(buf, "|DELTA_WAIT");
    if (state & FIG_OPEN_WAIT)
        strcat(buf, "|OPEN_WAIT");
    if (state & FIG_ACCNT_WAIT)
        strcat(buf, "|ACCNT_WAIT");
    if (state & FIG_CONTR_WAIT)
        strcat(buf, "|CONTR_WAIT");
    if (state & FIG_SNAP_WAIT)
        strcat(buf, "|SNAP_WAIT");
    if (state & FIG_CLOSE_WAIT)
        strcat(buf, "|CLOSE_WAIT");
    if (state & FIG_CLOSED)
        strcat(buf, "|CLOSED");
    if (buf[0] == '|')
        buf[0] = ' ';
    sc_log_debug1("state:%s", buf);
}
#endif // SC_DEBUG_LEVEL >= 1

SC_EXTERN void
fig_clnt_sess_reset(ScClnt clnt)
{
    // This function does not schedule any new timers.

    sc_sess_reset(&clnt->sess);
    clnt->state = FIG_DELTA_WAIT;
    fig_clnt_log_state(clnt->state);
    fig_cache_reset(&clnt->cache);
    fig_ordidx_init(&clnt->ordidx);
    free_views(&clnt->views, clnt->pool);
    sc_prioq_reset(&clnt->prioq);
    clnt->mdlast = 0;
}

SC_EXTERN ScIden
fig_clnt_sess_close(ScClnt clnt, ScMillis now)
{
    if (clnt->state == FIG_CLOSED) {
        sc_err_set(SC_EINVAL, "client already closed");
        goto fail1;
    }

    if (clnt->state == FIG_DELTA_WAIT) {
        clnt->state |= FIG_CLOSE_WAIT;
        fig_clnt_log_state(clnt->state);
        return 0;
    }

    // Reserve so that push cannot fail after send.
    if (!sc_prioq_reserve(&clnt->prioq, sc_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    const ScIden req_id = clnt->close_id;
    struct ScBody body = { .req_id = req_id, .sid = clnt->sess.sid, .type = SC_SESS_CLOSE };

    if (!sc_send_body(clnt->trsock, &body, SC_FALSE))
        goto fail1;

    clnt->state |= FIG_CLOSE_WAIT;
    fig_clnt_log_state(clnt->state);
    sc_prioq_push(&clnt->prioq, req_id, now + clnt->tmout);
    sc_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return req_id;
 fail1:
    return -1;
}

SC_EXTERN ScIden
fig_clnt_sess_open(ScClnt clnt, ScMillis now)
{
    // Reserve so that push cannot fail after send.
    if (!sc_prioq_reserve(&clnt->prioq, sc_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    const ScIden req_id = clnt->open_id;
    struct ScBody body = { .req_id = req_id, .sid = clnt->sess.sid, .type = SC_SESS_OPEN,
                            .sess_open = { .hbint = FIG_TRINT } };
    if (!sc_send_body(clnt->trsock, &body, SC_FALSE))
        goto fail1;

    clnt->state |= FIG_OPEN_WAIT;
    fig_clnt_log_state(clnt->state);
    sc_prioq_push(&clnt->prioq, req_id, now + clnt->tmout);
    sc_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return req_id;
 fail1:
    return -1;
}

SC_API ScClnt
sc_clnt_create(void* zctx, const ScUuid uuid, const char* mdaddr, const char* traddr,
                ScMillis tmout, ScPool pool)
{
    // 1.
    ScClnt clnt = malloc(sizeof(struct FigClnt));
    if (sc_unlikely(!clnt)) {
        sc_err_set(SC_ENOMEM, "out of memory");
        goto fail1;
    }

    // 2.
    sc_sess_init(&clnt->sess, uuid, pool);

    // 3.
    void* mdsock = zmq_socket(zctx, ZMQ_SUB);
    if (!mdsock) {
        sc_err_setf(SC_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    const int opt = 0;
    zmq_setsockopt(mdsock, ZMQ_LINGER, &opt, sizeof(opt));
    zmq_setsockopt(mdsock, ZMQ_SUBSCRIBE, "", 0);

    if (zmq_connect(mdsock, mdaddr) < 0) {
        sc_err_setf(SC_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }

    // 4.
    void* trsock = zmq_socket(zctx, ZMQ_DEALER);
    if (!trsock) {
        sc_err_setf(SC_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }
    zmq_setsockopt(trsock, ZMQ_LINGER, &opt, sizeof(opt));
    zmq_setsockopt(trsock, ZMQ_IDENTITY, clnt->sess.uuid, 16);

    if (zmq_connect(trsock, traddr) < 0) {
        sc_err_setf(SC_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail4;
    }

    // 5.
    void* asock = zmq_socket(zctx, ZMQ_REP);
    if (!asock) {
        sc_err_setf(SC_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail4;
    }
    zmq_setsockopt(asock, ZMQ_LINGER, &opt, sizeof(opt));

    enum { INPROC_LEN = sizeof("inproc://") - 1 };
    char aaddr[INPROC_LEN + SC_UUID_MAX + 1];
    __builtin_memcpy(aaddr, "inproc://", INPROC_LEN);
    uuid_unparse_lower(clnt->sess.uuid, aaddr + INPROC_LEN);

    if (zmq_bind(asock, aaddr) < 0) {
        sc_err_setf(SC_EIO, "zmq_bind() failed: %s", zmq_strerror(zmq_errno()));
        goto fail5;
    }

    clnt->mdsock = mdsock;
    clnt->trsock = trsock;
    clnt->asock = asock;
    clnt->id = 1;
    clnt->tmout = tmout;
    clnt->pool = pool;
    clnt->state = FIG_DELTA_WAIT;
    fig_clnt_log_state(clnt->state);
    // 6.
    fig_cache_init(&clnt->cache, term_state, pool);
    fig_ordidx_init(&clnt->ordidx);
    // 7.
    sc_tree_init(&clnt->views);
    // 8.
    if (!sc_prioq_init(&clnt->prioq))
        goto fail6;

    clnt->mdlast = 0;
    clnt->close_id = clnt->id++;
    clnt->open_id = clnt->id++;

    clnt->items[FIG_MDSOCK].socket = mdsock;
    clnt->items[FIG_MDSOCK].fd = 0;
    clnt->items[FIG_MDSOCK].events = ZMQ_POLLIN;
    clnt->items[FIG_MDSOCK].revents = 0;

    clnt->items[FIG_TRSOCK].socket = trsock;
    clnt->items[FIG_TRSOCK].fd = 0;
    clnt->items[FIG_TRSOCK].events = ZMQ_POLLIN;
    clnt->items[FIG_TRSOCK].revents = 0;

    clnt->items[FIG_ASOCK].socket = asock;
    clnt->items[FIG_ASOCK].fd = 0;
    clnt->items[FIG_ASOCK].events = ZMQ_POLLIN;
    clnt->items[FIG_ASOCK].revents = 0;

    if (!sc_prioq_push(&clnt->prioq, FIG_MDTMR, sc_millis() + FIG_MDTMOUT))
        goto fail7;

    return clnt;
 fail7:
    // 8.
    sc_prioq_term(&clnt->prioq);
 fail6:
    // 7.
    free_views(&clnt->views, pool);
    // 6.
    fig_cache_term(&clnt->cache);
 fail5:
    // 5.
    zmq_close(asock);
 fail4:
    // 4.
    zmq_close(trsock);
 fail3:
    // 3.
    zmq_close(mdsock);
 fail2:
    // 2.
    sc_sess_term(&clnt->sess);
    // 1.
    free(clnt);
 fail1:
    return NULL;
}

SC_API void
sc_clnt_destroy(ScClnt clnt)
{
    if (clnt) {
        // 8.
        sc_prioq_term(&clnt->prioq);
        // 7.
        free_views(&clnt->views, clnt->pool);
        // 6.
        fig_cache_term(&clnt->cache);
        // 5.
        zmq_close(clnt->asock);
        // 4.
        zmq_close(clnt->trsock);
        // 3.
        zmq_close(clnt->mdsock);
        // 2.
        sc_sess_term(&clnt->sess);
        // 1.
        free(clnt);
    }
}

SC_API void
sc_clnt_reset(ScClnt clnt)
{
    fig_clnt_sess_reset(clnt);
    // Cannot fail due to prioq reset.
    sc_prioq_push(&clnt->prioq, FIG_MDTMR, sc_millis() + FIG_MDTMOUT);
}

SC_API ScIden
sc_clnt_close(ScClnt clnt)
{
    return fig_clnt_sess_close(clnt, sc_millis());
}

SC_API struct ScSlNode*
sc_clnt_find_rec_id(ScClnt clnt, int type, ScIden id)
{
    return fig_cache_find_rec_id(&clnt->cache, type, id);
}

SC_API struct ScSlNode*
sc_clnt_find_rec_mnem(ScClnt clnt, int type, const char* mnem)
{
    return fig_cache_find_rec_mnem(&clnt->cache, type, mnem);
}

SC_API struct ScSlNode*
sc_clnt_first_rec(ScClnt clnt, int type, size_t* size)
{
    return fig_cache_first_rec(&clnt->cache, type, size);
}

SC_API ScBool
sc_clnt_empty_rec(ScClnt clnt, int type)
{
    return fig_cache_empty_rec(&clnt->cache, type);
}

SC_API ScAccnt
sc_clnt_accnt(ScClnt clnt, struct ScRec* arec)
{
    return fig_accnt_lazy(arec, &clnt->ordidx, clnt->pool);
}

SC_API ScIden
sc_clnt_logon(ScClnt clnt, ScAccnt accnt)
{
    if (clnt->state != FIG_READY) {
        sc_err_set(SC_EBUSY, "client not ready");
        goto fail1;
    }

    struct ScBody body;
    body.req_id = clnt->id++;
    body.sid = clnt->sess.sid;
    body.type = SC_SESS_LOGON;
    body.sess_logon.aid = accnt->rec->id;

    // Reserve so that push cannot fail after send.
    if (!sc_prioq_reserve(&clnt->prioq, sc_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!sc_send_body(clnt->trsock, &body, SC_FALSE))
        goto fail1;

    const ScMillis now = sc_millis();
    sc_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    sc_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

SC_API ScIden
sc_clnt_logoff(ScClnt clnt, ScAccnt accnt)
{
    if (clnt->state != FIG_READY) {
        sc_err_set(SC_EBUSY, "client not ready");
        goto fail1;
    }
    struct ScBody body;
    body.req_id = clnt->id++;
    body.sid = clnt->sess.sid;
    body.type = SC_SESS_LOGOFF;
    body.sess_logoff.aid = accnt->rec->id;

    // Reserve so that push cannot fail after send.
    if (!sc_prioq_reserve(&clnt->prioq, sc_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!sc_send_body(clnt->trsock, &body, SC_FALSE))
        goto fail1;

    const ScMillis now = sc_millis();
    sc_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    sc_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

SC_API ScIden
sc_clnt_place(ScClnt clnt, ScAccnt trader, ScAccnt giveup, struct ScRec* crec,
               ScJd settl_day, const char* ref, int action, ScTicks ticks, ScLots lots,
               ScLots min_lots)
{
    if (clnt->state != FIG_READY) {
        sc_err_set(SC_EBUSY, "client not ready");
        goto fail1;
    }
    struct ScBody body;
    body.req_id = clnt->id++;
    body.sid = clnt->sess.sid;
    body.type = SC_PLACE_ORDER_REQ;
    body.place_order_req.tid = trader->rec->id;
    body.place_order_req.gid = giveup->rec->id;
    body.place_order_req.cid = crec->id;
    body.place_order_req.settl_day = settl_day;
    if (ref)
        strncpy(body.place_order_req.ref, ref, SC_REF_MAX);
    else
        body.place_order_req.ref[0] = '\0';
    body.place_order_req.action = action;
    body.place_order_req.ticks = ticks;
    body.place_order_req.lots = lots;
    body.place_order_req.min_lots = min_lots;

    // Reserve so that push cannot fail after send.
    if (!sc_prioq_reserve(&clnt->prioq, sc_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!sc_send_body(clnt->trsock, &body, SC_FALSE))
        goto fail1;

    const ScMillis now = sc_millis();
    sc_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    sc_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

SC_API ScIden
sc_clnt_revise_id(ScClnt clnt, ScAccnt trader, ScIden id, ScLots lots)
{
    if (clnt->state != FIG_READY) {
        sc_err_set(SC_EBUSY, "client not ready");
        goto fail1;
    }
    struct ScBody body;
    body.req_id = clnt->id++;
    body.sid = clnt->sess.sid;
    body.type = SC_REVISE_ORDER_ID_REQ;
    body.revise_order_id_req.tid = trader->rec->id;
    body.revise_order_id_req.id = id;
    body.revise_order_id_req.lots = lots;

    // Reserve so that push cannot fail after send.
    if (!sc_prioq_reserve(&clnt->prioq, sc_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!sc_send_body(clnt->trsock, &body, SC_FALSE))
        goto fail1;

    const ScMillis now = sc_millis();
    sc_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    sc_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

SC_API ScIden
sc_clnt_revise_ref(ScClnt clnt, ScAccnt trader, const char* ref, ScLots lots)
{
    if (clnt->state != FIG_READY) {
        sc_err_set(SC_EBUSY, "client not ready");
        goto fail1;
    }
    struct ScBody body;
    body.req_id = clnt->id++;
    body.sid = clnt->sess.sid;
    body.type = SC_REVISE_ORDER_REF_REQ;
    body.revise_order_ref_req.tid = trader->rec->id;
    strncpy(body.revise_order_ref_req.ref, ref, SC_REF_MAX);
    body.revise_order_ref_req.lots = lots;

    // Reserve so that push cannot fail after send.
    if (!sc_prioq_reserve(&clnt->prioq, sc_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!sc_send_body(clnt->trsock, &body, SC_FALSE))
        goto fail1;

    const ScMillis now = sc_millis();
    sc_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    sc_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

SC_API ScIden
sc_clnt_cancel_id(ScClnt clnt, ScAccnt trader, ScIden id)
{
    if (clnt->state != FIG_READY) {
        sc_err_set(SC_EBUSY, "client not ready");
        goto fail1;
    }
    struct ScBody body;
    body.req_id = clnt->id++;
    body.sid = clnt->sess.sid;
    body.type = SC_CANCEL_ORDER_ID_REQ;
    body.cancel_order_id_req.tid = trader->rec->id;
    body.cancel_order_id_req.id = id;

    // Reserve so that push cannot fail after send.
    if (!sc_prioq_reserve(&clnt->prioq, sc_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!sc_send_body(clnt->trsock, &body, SC_FALSE))
        goto fail1;

    const ScMillis now = sc_millis();
    sc_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    sc_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

SC_API ScIden
sc_clnt_cancel_ref(ScClnt clnt, ScAccnt trader, const char* ref)
{
    if (clnt->state != FIG_READY) {
        sc_err_set(SC_EBUSY, "client not ready");
        goto fail1;
    }
    struct ScBody body;
    body.req_id = clnt->id++;
    body.sid = clnt->sess.sid;
    body.type = SC_CANCEL_ORDER_REF_REQ;
    body.cancel_order_ref_req.tid = trader->rec->id;
    strncpy(body.cancel_order_ref_req.ref, ref, SC_REF_MAX);

    // Reserve so that push cannot fail after send.
    if (!sc_prioq_reserve(&clnt->prioq, sc_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!sc_send_body(clnt->trsock, &body, SC_FALSE))
        goto fail1;

    const ScMillis now = sc_millis();
    sc_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    sc_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

SC_API ScIden
sc_clnt_ack_trade(ScClnt clnt, ScAccnt trader, ScIden id)
{
    if (clnt->state != FIG_READY) {
        sc_err_set(SC_EBUSY, "client not ready");
        goto fail1;
    }
    struct ScBody body;
    body.req_id = clnt->id++;
    body.sid = clnt->sess.sid;
    body.type = SC_ACK_TRADE_REQ;
    body.ack_trade_req.tid = trader->rec->id;
    body.ack_trade_req.id = id;

    // Reserve so that push cannot fail after send.
    if (!sc_prioq_reserve(&clnt->prioq, sc_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!sc_send_body(clnt->trsock, &body, SC_FALSE))
        goto fail1;

    fig_accnt_remove_trade_id(trader, id);

    const ScMillis now = sc_millis();
    sc_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    sc_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

SC_API ScIden
sc_clnt_set_timer(ScClnt clnt, ScMillis absms)
{
    ScIden id = clnt->id++;
    if (!sc_prioq_push(&clnt->prioq, id, absms))
        id = -1;
    return id;
}

SC_API void
sc_clnt_cancel_timer(ScClnt clnt, ScIden id)
{
    sc_prioq_remove(&clnt->prioq, id);
}

SC_API struct ScRbNode*
sc_clnt_find_view(ScClnt clnt, ScIden cid, ScJd settl_day)
{
    const ScKey key = sc_view_key(cid, settl_day);
    return sc_tree_find(&clnt->views, key);
}

SC_API struct ScRbNode*
sc_clnt_first_view(ScClnt clnt)
{
    return sc_tree_first(&clnt->views);
}

SC_API struct ScRbNode*
sc_clnt_last_view(ScClnt clnt)
{
    return sc_tree_last(&clnt->views);
}

SC_API ScBool
sc_clnt_empty_view(ScClnt clnt)
{
    return sc_tree_empty(&clnt->views);
}

SC_API const unsigned char*
sc_clnt_uuid(ScClnt clnt)
{
    return clnt->sess.uuid;
}

SC_API ScBool
sc_clnt_is_closed(ScClnt clnt)
{
    return clnt->state == FIG_CLOSED;
}

SC_API ScBool
sc_clnt_is_ready(ScClnt clnt)
{
    return clnt->state == FIG_READY;
}
