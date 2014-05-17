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
#include "clnt.h"

#include "accnt.h"

#include <dbr/err.h>
#include <dbr/msg.h>
#include <dbr/sess.h>
#include <dbr/util.h>

#include <stdlib.h> // malloc()
#include <string.h> // strncpy()

#include <uuid/uuid.h>

static void
term_state(struct DbrRec* rec)
{
    switch (rec->type) {
    case DBR_ENTITY_ACCNT:
        fig_accnt_term(rec);
        break;
    }
}

static void
free_views(struct DbrTree* views, DbrPool pool)
{
    assert(views);
    struct DbrRbNode* node;
    while ((node = views->root)) {
        struct DbrView* view = dbr_clnt_view_entry(node);
        dbr_tree_remove(views, node);
        dbr_pool_free_view(pool, view);
    }
}

DBR_EXTERN void
fig_sess_reset(DbrClnt clnt)
{
    // This function does not schedule any new timers.

    dbr_sess_reset(&clnt->sess);
    clnt->state = FIG_DELTA_WAIT;
    fig_cache_reset(&clnt->cache);
    fig_ordidx_init(&clnt->ordidx);
    free_views(&clnt->views, clnt->pool);
    dbr_prioq_reset(&clnt->prioq);
    clnt->mdlast = 0;
}

DBR_EXTERN DbrIden
fig_sess_close(DbrClnt clnt, DbrMillis now)
{
    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    const DbrIden req_id = clnt->close_id;
    struct DbrBody body = { .req_id = req_id, .type = DBR_SESS_CLOSE };

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    dbr_prioq_push(&clnt->prioq, req_id, now + clnt->tmout);
    dbr_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return req_id;
 fail1:
    return -1;
}

DBR_EXTERN DbrIden
fig_sess_open(DbrClnt clnt, DbrMillis now)
{
    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    const DbrIden req_id = clnt->open_id;
    struct DbrBody body = { .req_id = req_id, .type = DBR_SESS_OPEN,
                            .sess_open = { .hbint = FIG_TRINT } };
    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    dbr_prioq_push(&clnt->prioq, req_id, now + clnt->tmout);
    dbr_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return req_id;
 fail1:
    return -1;
}

DBR_API DbrClnt
dbr_clnt_create(void* zctx, const DbrUuid uuid, const char* mdaddr, const char* traddr,
                DbrIden seed, DbrMillis tmout, DbrPool pool)
{
    // 1.
    DbrClnt clnt = malloc(sizeof(struct FigClnt));
    if (dbr_unlikely(!clnt)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    // 2.
    dbr_sess_init(&clnt->sess, uuid, pool);

    // 3.
    void* mdsock = zmq_socket(zctx, ZMQ_SUB);
    if (!mdsock) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    const int opt = 0;
    zmq_setsockopt(mdsock, ZMQ_LINGER, &opt, sizeof(opt));
    zmq_setsockopt(mdsock, ZMQ_SUBSCRIBE, "", 0);

    if (zmq_connect(mdsock, mdaddr) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }

    // 4.
    void* trsock = zmq_socket(zctx, ZMQ_DEALER);
    if (!trsock) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }
    zmq_setsockopt(trsock, ZMQ_LINGER, &opt, sizeof(opt));
    zmq_setsockopt(trsock, ZMQ_IDENTITY, clnt->sess.uuid, 16);

    if (zmq_connect(trsock, traddr) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail4;
    }

    // 5.
    void* asock = zmq_socket(zctx, ZMQ_REP);
    if (!asock) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail4;
    }
    zmq_setsockopt(asock, ZMQ_LINGER, &opt, sizeof(opt));

    enum { INPROC_LEN = sizeof("inproc://") - 1 };
    char aaddr[INPROC_LEN + DBR_UUID_MAX + 1];
    __builtin_memcpy(aaddr, "inproc://", INPROC_LEN);
    uuid_unparse_lower(clnt->sess.uuid, aaddr + INPROC_LEN);

    if (zmq_bind(asock, aaddr) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_bind() failed: %s", zmq_strerror(zmq_errno()));
        goto fail5;
    }

    clnt->mdsock = mdsock;
    clnt->trsock = trsock;
    clnt->asock = asock;
    clnt->id = seed;
    clnt->tmout = tmout;
    clnt->pool = pool;
    clnt->state = FIG_DELTA_WAIT;
    // 6.
    fig_cache_init(&clnt->cache, term_state, pool);
    fig_ordidx_init(&clnt->ordidx);
    // 7.
    dbr_tree_init(&clnt->views);
    // 8.
    if (!dbr_prioq_init(&clnt->prioq))
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

    if (!dbr_prioq_push(&clnt->prioq, FIG_MDTMR, dbr_millis() + FIG_MDTMOUT))
        goto fail7;

    return clnt;
 fail7:
    // 8.
    dbr_prioq_term(&clnt->prioq);
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
    dbr_sess_term(&clnt->sess);
    // 1.
    free(clnt);
 fail1:
    return NULL;
}

DBR_API void
dbr_clnt_destroy(DbrClnt clnt)
{
    if (clnt) {
        // 8.
        dbr_prioq_term(&clnt->prioq);
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
        dbr_sess_term(&clnt->sess);
        // 1.
        free(clnt);
    }
}

DBR_API void
dbr_clnt_reset(DbrClnt clnt)
{
    fig_sess_reset(clnt);
    // Cannot fail due to prioq reset.
    dbr_prioq_push(&clnt->prioq, FIG_MDTMR, dbr_millis() + FIG_MDTMOUT);
}

DBR_API DbrIden
dbr_clnt_close(DbrClnt clnt)
{
    const DbrMillis now = dbr_millis();
    const DbrIden req_id = fig_sess_close(clnt, now);
    if (req_id < 0)
        goto fail1;
    return req_id;
 fail1:
    return -1;
}

DBR_API struct DbrSlNode*
dbr_clnt_find_rec_id(DbrClnt clnt, int type, DbrIden id)
{
    return fig_cache_find_rec_id(&clnt->cache, type, id);
}

DBR_API struct DbrSlNode*
dbr_clnt_find_rec_mnem(DbrClnt clnt, int type, const char* mnem)
{
    return fig_cache_find_rec_mnem(&clnt->cache, type, mnem);
}

DBR_API struct DbrSlNode*
dbr_clnt_first_rec(DbrClnt clnt, int type, size_t* size)
{
    return fig_cache_first_rec(&clnt->cache, type, size);
}

DBR_API DbrBool
dbr_clnt_empty_rec(DbrClnt clnt, int type)
{
    return fig_cache_empty_rec(&clnt->cache, type);
}

DBR_API DbrAccnt
dbr_clnt_accnt(DbrClnt clnt, struct DbrRec* arec)
{
    return fig_accnt_lazy(arec, &clnt->ordidx, clnt->pool);
}

DBR_API DbrIden
dbr_clnt_logon(DbrClnt clnt, DbrAccnt user)
{
    if (clnt->state != FIG_READY) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }

    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_SESS_LOGON;
    body.sess_logon.uid = user->rec->id;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    dbr_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_logoff(DbrClnt clnt, DbrAccnt user)
{
    if (clnt->state != FIG_READY) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_SESS_LOGOFF;
    body.sess_logoff.uid = user->rec->id;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    dbr_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_place(DbrClnt clnt, DbrAccnt user, DbrAccnt group, struct DbrRec* crec,
               DbrJd settl_day, const char* ref, int action, DbrTicks ticks, DbrLots lots,
               DbrLots min_lots)
{
    if (clnt->state != FIG_READY) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_PLACE_ORDER_REQ;
    body.place_order_req.uid = user->rec->id;
    body.place_order_req.gid = group->rec->id;
    body.place_order_req.cid = crec->id;
    body.place_order_req.settl_day = settl_day;
    if (ref)
        strncpy(body.place_order_req.ref, ref, DBR_REF_MAX);
    else
        body.place_order_req.ref[0] = '\0';
    body.place_order_req.action = action;
    body.place_order_req.ticks = ticks;
    body.place_order_req.lots = lots;
    body.place_order_req.min_lots = min_lots;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    dbr_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_revise_id(DbrClnt clnt, DbrAccnt user, DbrIden id, DbrLots lots)
{
    if (clnt->state != FIG_READY) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_REVISE_ORDER_ID_REQ;
    body.revise_order_id_req.uid = user->rec->id;
    body.revise_order_id_req.id = id;
    body.revise_order_id_req.lots = lots;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    dbr_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_revise_ref(DbrClnt clnt, DbrAccnt user, const char* ref, DbrLots lots)
{
    if (clnt->state != FIG_READY) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_REVISE_ORDER_REF_REQ;
    body.revise_order_ref_req.uid = user->rec->id;
    strncpy(body.revise_order_ref_req.ref, ref, DBR_REF_MAX);
    body.revise_order_ref_req.lots = lots;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    dbr_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_cancel_id(DbrClnt clnt, DbrAccnt user, DbrIden id)
{
    if (clnt->state != FIG_READY) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_CANCEL_ORDER_ID_REQ;
    body.cancel_order_id_req.uid = user->rec->id;
    body.cancel_order_id_req.id = id;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    dbr_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_cancel_ref(DbrClnt clnt, DbrAccnt user, const char* ref)
{
    if (clnt->state != FIG_READY) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_CANCEL_ORDER_REF_REQ;
    body.cancel_order_ref_req.uid = user->rec->id;
    strncpy(body.cancel_order_ref_req.ref, ref, DBR_REF_MAX);

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    dbr_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_ack_trade(DbrClnt clnt, DbrAccnt user, DbrIden id)
{
    if (clnt->state != FIG_READY) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_ACK_TRADE_REQ;
    body.ack_trade_req.uid = user->rec->id;
    body.ack_trade_req.id = id;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    fig_accnt_remove_trade_id(user, id);

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + clnt->tmout);
    dbr_prioq_replace(&clnt->prioq, FIG_HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_settimer(DbrClnt clnt, DbrMillis absms)
{
    DbrIden id = clnt->id++;
    if (!dbr_prioq_push(&clnt->prioq, id, absms))
        id = -1;
    return id;
}

DBR_API void
dbr_clnt_canceltimer(DbrClnt clnt, DbrIden id)
{
    dbr_prioq_remove(&clnt->prioq, id);
}

DBR_API struct DbrRbNode*
dbr_clnt_find_view(DbrClnt clnt, DbrIden cid, DbrJd settl_day)
{
    const DbrKey key = dbr_view_key(cid, settl_day);
    return dbr_tree_find(&clnt->views, key);
}

DBR_API struct DbrRbNode*
dbr_clnt_first_view(DbrClnt clnt)
{
    return dbr_tree_first(&clnt->views);
}

DBR_API struct DbrRbNode*
dbr_clnt_last_view(DbrClnt clnt)
{
    return dbr_tree_last(&clnt->views);
}

DBR_API DbrBool
dbr_clnt_empty_view(DbrClnt clnt)
{
    return dbr_tree_empty(&clnt->views);
}

DBR_API const unsigned char*
dbr_clnt_uuid(DbrClnt clnt)
{
    return clnt->sess.uuid;
}

DBR_API DbrBool
dbr_clnt_is_closed(DbrClnt clnt)
{
    return clnt->state == FIG_CLOSED;
}

DBR_API DbrBool
dbr_clnt_is_ready(DbrClnt clnt)
{
    return clnt->state == FIG_READY;
}
