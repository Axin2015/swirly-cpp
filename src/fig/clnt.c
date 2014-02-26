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

#include "accnt.h"
#include "cache.h"
#include "trader.h"

#include <dbr/clnt.h>
#include <dbr/conv.h> // dbr_book_key()
#include <dbr/err.h>
#include <dbr/log.h>
#include <dbr/msg.h>
#include <dbr/prioq.h>
#include <dbr/queue.h>
#include <dbr/refcount.h>
#include <dbr/sess.h>
#include <dbr/util.h>

#include <zmq.h>

#include <stdlib.h> // malloc()
#include <string.h> // strncpy()

enum {
    MDSOCK,
    TRSOCK,
    ASOCK,
    NSOCK
};

// Other constants.

enum {
    // Non-negative timer ids are reserved for internal use.

    // The outbound heartbeat timer is scheduled once the logon response has been received along
    // with the heartbeat interval.
    HBTMR = -1,

    MDTMR = -2,
    MDINT = 1000,
    MDTMOUT = (MDINT * 3) / 2,

    // The transaction heartbeat timer is scheduled when the logon request is sent during
    // initialisation.
    TRTMR = -3,
    TRINT = 5000,
    TRTMOUT = (TRINT * 3) / 2,

    INIT_PENDING   = 0x01,
    TRADER_PENDING = 0x02,
    ACCNT_PENDING  = 0x04,
    CONTR_PENDING  = 0x08,
    VIEW_PENDING   = 0x10,
    TR_CLOSED      = 0x20,
    TR_DOWN        = 0x40,
    MD_DOWN        = 0x80
};

struct FigClnt {
    struct DbrSess sess;
    void* mdsock;
    void* trsock;
    void* asock;
    DbrIden id;
    DbrPool pool;
    unsigned flags;
    struct FigCache cache;
    struct FigOrdIdx ordidx;
    struct DbrQueue execs;
    struct DbrTree views;
    struct DbrTree posnups;
    struct DbrTree viewups;
    struct DbrPrioq prioq;
    DbrMillis mdlast;
    zmq_pollitem_t items[NSOCK];
};

static void
term_state(struct DbrRec* rec)
{
    switch (rec->type) {
    case DBR_ENTITY_TRADER:
        fig_trader_term(rec);
        break;
    case DBR_ENTITY_ACCNT:
        fig_accnt_term(rec);
        break;
    }
}

static inline struct DbrRec*
get_id(struct FigCache* cache, int type, DbrIden id)
{
    struct DbrSlNode* node = fig_cache_find_rec_id(cache, type, id);
    assert(node != FIG_CACHE_END_REC);
    return dbr_shared_rec_entry(node);
}

static inline struct DbrOrder*
enrich_order(struct FigCache* cache, struct DbrOrder* order)
{
    order->c.trader.rec = get_id(cache, DBR_ENTITY_TRADER, order->c.trader.id_only);
    order->c.accnt.rec = get_id(cache, DBR_ENTITY_ACCNT, order->c.accnt.id_only);
    order->c.contr.rec = get_id(cache, DBR_ENTITY_CONTR, order->c.contr.id_only);
    return order;
}

static inline struct DbrExec*
enrich_exec(struct FigCache* cache, struct DbrExec* exec)
{
    exec->c.trader.rec = get_id(cache, DBR_ENTITY_TRADER, exec->c.trader.id_only);
    exec->c.accnt.rec = get_id(cache, DBR_ENTITY_ACCNT, exec->c.accnt.id_only);
    exec->c.contr.rec = get_id(cache, DBR_ENTITY_CONTR, exec->c.contr.id_only);
    if (exec->cpty.id_only)
        exec->cpty.rec = get_id(cache, DBR_ENTITY_ACCNT, exec->cpty.id_only);
    else
        exec->cpty.rec = NULL;
    return exec;
}

static inline struct DbrMemb*
enrich_memb(struct FigCache* cache, struct DbrMemb* memb)
{
    memb->trader.rec = get_id(cache, DBR_ENTITY_TRADER, memb->trader.id_only);
    memb->accnt.rec = get_id(cache, DBR_ENTITY_ACCNT, memb->accnt.id_only);
    return memb;
}

static inline struct DbrPosn*
enrich_posn(struct FigCache* cache, struct DbrPosn* posn)
{
    posn->accnt.rec = get_id(cache, DBR_ENTITY_ACCNT, posn->accnt.id_only);
    posn->contr.rec = get_id(cache, DBR_ENTITY_CONTR, posn->contr.id_only);
    return posn;
}

static inline struct DbrView*
enrich_view(struct FigCache* cache, struct DbrView* view)
{
    view->contr.rec = get_id(cache, DBR_ENTITY_CONTR, view->contr.id_only);
    return view;
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

static inline void
insert_posnup(struct DbrTree* posnups, struct DbrPosn* posn)
{
    dbr_tree_insert(posnups, (DbrKey)posn, &posn->update_node_);
}

static inline void
insert_viewup(struct DbrTree* viewups, struct DbrView* view)
{
    dbr_tree_insert(viewups, (DbrKey)view, &view->update_node_);
}

static DbrTrader
get_trader(DbrClnt clnt, DbrIden tid)
{
    struct DbrRec* trec = get_id(&clnt->cache, DBR_ENTITY_TRADER, tid);
    assert(trec->trader.state);
    return trec->trader.state;
}

static DbrIden
init(DbrClnt clnt, DbrMillis now)
{
    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    struct DbrBody body = { .req_id = clnt->id++, .type = DBR_SESS_OPEN,
                            .sess_open = { .hbint = TRINT } };
    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    dbr_prioq_push(&clnt->prioq, TRTMR, now + TRTMOUT);
    return body.req_id;
 fail1:
    return -1;
}

static void
emplace_rec_list(DbrClnt clnt, int type, unsigned flag, struct DbrSlNode* first, size_t count)
{
    fig_cache_emplace_rec_list(&clnt->cache, type, first, count);
    clnt->flags &= ~flag;
}

static void
emplace_order_list(DbrClnt clnt, struct DbrSlNode* first)
{
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrOrder* order = enrich_order(&clnt->cache, dbr_shared_order_entry(node));
        DbrTrader trader = order->c.trader.rec->trader.state;
        assert(trader);
        // Transfer ownership.
        fig_trader_emplace_order(trader, order);
    }
}

static void
emplace_exec_list(DbrClnt clnt, struct DbrSlNode* first)
{
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrExec* exec = enrich_exec(&clnt->cache, dbr_shared_exec_entry(node));
        assert(exec->c.state == DBR_STATE_TRADE);
        DbrTrader trader = exec->c.trader.rec->trader.state;
        assert(trader);
        // Transfer ownership.
        fig_trader_insert_trade(trader, exec);
        dbr_exec_decref(exec, clnt->pool);
    }
}

static DbrBool
emplace_memb_list(DbrClnt clnt, struct DbrSlNode* first)
{
    DbrBool nomem = DBR_FALSE;
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrMemb* memb = enrich_memb(&clnt->cache, dbr_shared_memb_entry(node));

        DbrTrader trader = memb->trader.rec->trader.state;
        assert(trader);
        // Transfer ownership.
        fig_trader_emplace_memb(trader, memb);

        DbrAccnt accnt = fig_accnt_lazy(memb->accnt.rec, clnt->pool);
        if (dbr_likely(accnt)) {
            fig_accnt_insert_memb(accnt, memb);
        } else {
            // Member is owned by trader so no need to free here.
            nomem = DBR_TRUE;
        }
    }
    if (dbr_unlikely(nomem)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return DBR_FALSE;
    }
    return DBR_TRUE;
}

static DbrBool
emplace_posn_list(DbrClnt clnt, struct DbrSlNode* first)
{
    DbrBool nomem = DBR_FALSE;
    for (struct DbrSlNode* node = first; node; ) {
        struct DbrPosn* posn = enrich_posn(&clnt->cache, dbr_shared_posn_entry(node));
        node = node->next;
        // Transfer ownership.
        // All accnts that trader is member of are created in emplace_membs().
        DbrAccnt accnt = fig_accnt_lazy(posn->accnt.rec, clnt->pool);
        if (dbr_likely(accnt)) {
            fig_accnt_emplace_posn(accnt, posn);
        } else {
            dbr_pool_free_posn(clnt->pool, posn);
            nomem = DBR_TRUE;
        }
    }
    if (dbr_unlikely(nomem)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return DBR_FALSE;
    }
    return DBR_TRUE;
}

static struct DbrOrder*
create_order(DbrClnt clnt, struct DbrExec* exec)
{
    struct DbrOrder* order = dbr_pool_alloc_order(clnt->pool);
    if (!order)
        return NULL;
    dbr_order_init(order);

    order->level = NULL;
    order->id = exec->order;
    __builtin_memcpy(&order->c, &exec->c, sizeof(struct DbrCommon));
    order->created = exec->created;
    order->modified = exec->created;
    return order;
}

static DbrBool
apply_new(DbrClnt clnt, struct DbrExec* exec)
{
    struct DbrOrder* order = create_order(clnt, exec);
    if (!order)
        return DBR_FALSE;
    DbrTrader trader = order->c.trader.rec->trader.state;
    assert(trader);
    // Transfer ownership.
    fig_trader_emplace_order(trader, order);
    dbr_exec_incref(exec);
    dbr_queue_insert_back(&clnt->execs, &exec->shared_node_);
    return DBR_TRUE;
}

static DbrBool
apply_update(DbrClnt clnt, struct DbrExec* exec)
{
    DbrTrader trader = exec->c.trader.rec->trader.state;
    assert(trader);
    struct DbrRbNode* node = fig_trader_find_order_id(trader, exec->order);
    if (!node) {
        dbr_err_setf(DBR_EINVAL, "no such order '%ld'", exec->order);
        return DBR_FALSE;
    }

    struct DbrOrder* order = dbr_trader_order_entry(node);
    order->c.state = exec->c.state;
    order->c.lots = exec->c.lots;
    order->c.resd = exec->c.resd;
    order->c.exec = exec->c.exec;
    order->c.last_ticks = exec->c.last_ticks;
    order->c.last_lots = exec->c.last_lots;
    order->modified = exec->created;

    if (exec->c.state == DBR_STATE_TRADE) {
        // Transfer ownership.
        fig_trader_insert_trade(trader, exec);
    }
    dbr_exec_incref(exec);
    dbr_queue_insert_back(&clnt->execs, &exec->shared_node_);
    return DBR_TRUE;
}

static DbrBool
apply_posnup(DbrClnt clnt, struct DbrPosn* posn)
{
    DbrAccnt accnt = fig_accnt_lazy(posn->accnt.rec, clnt->pool);
    if (dbr_unlikely(!accnt)) {
        dbr_pool_free_posn(accnt->pool, posn);
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return DBR_FALSE;
    }
    posn = fig_accnt_update_posn(accnt, posn);
    insert_posnup(&clnt->posnups, posn);
    return DBR_TRUE;
}

static void
apply_viewup(DbrClnt clnt, struct DbrView* view, DbrBool overwrite)
{
    const DbrIden key = dbr_book_key(view->contr.rec->id, view->settl_date);
    struct DbrRbNode* node = dbr_tree_insert(&clnt->views, key, &view->clnt_node_);
    if (node != &view->clnt_node_) {

        // Drop if node already exists and overwrite is not set.
        if (dbr_unlikely(!overwrite)) {
            dbr_pool_free_view(clnt->pool, view);
            return;
        }

        struct DbrView* exist = dbr_clnt_view_entry(node);

        // Update existing position.

        assert(exist->contr.rec == view->contr.rec);
        assert(exist->settl_date == view->settl_date);

        for (size_t i = 0; i < DBR_LEVEL_MAX; ++i) {
            exist->bid_ticks[i] = view->bid_ticks[i];
            exist->bid_lots[i] = view->bid_lots[i];
            exist->bid_count[i] = view->bid_count[i];
            exist->ask_ticks[i] = view->ask_ticks[i];
            exist->ask_lots[i] = view->ask_lots[i];
            exist->ask_count[i] = view->ask_count[i];
        }

        dbr_pool_free_view(clnt->pool, view);
        view = exist;
    }
    insert_viewup(&clnt->viewups, view);
}

static void
apply_views(DbrClnt clnt, struct DbrSlNode* first, DbrMillis created, DbrHandler handler)
{
    const DbrBool overwrite = clnt->mdlast < created;
    clnt->mdlast = created;
    if (dbr_unlikely(!overwrite))
        dbr_log_warn("received stale market-data");
    for (struct DbrSlNode* node = first; node; ) {
        struct DbrView* view = dbr_shared_view_entry(node);
        node = node->next;
        // Transfer ownership or free.
        enrich_view(&clnt->cache, view);
        apply_viewup(clnt, view, overwrite);
        dbr_handler_on_view(handler, view);
    }
    dbr_handler_on_flush(handler);
}

static DbrBool
async_send(void* sock, void* val)
{
    if (zmq_send(sock, &val, sizeof(void*), 0) != sizeof(void*)) {
        dbr_err_setf(DBR_EIO, "zmq_send() failed: %s", zmq_strerror(zmq_errno()));
        return DBR_FALSE;
    }
    return DBR_TRUE;
}

static DbrBool
async_recv(void* sock, void** val)
{
    if (zmq_recv(sock, val, sizeof(void*), 0) != sizeof(void*)) {
        const int num = zmq_errno() == EINTR ? DBR_EINTR : DBR_EIO;
        dbr_err_setf(num, "zmq_msg_recv() failed: %s", zmq_strerror(zmq_errno()));
        return DBR_FALSE;
    }
    return DBR_TRUE;
}

DBR_API DbrClnt
dbr_clnt_create(void* ctx, const char* sess, const char* mdaddr, const char* traddr,
                DbrIden seed, DbrPool pool)
{
    // 1.
    DbrClnt clnt = malloc(sizeof(struct FigClnt));
    if (dbr_unlikely(!clnt)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    // 2.
    dbr_sess_init(&clnt->sess);
    strncpy(clnt->sess.mnem, sess, DBR_MNEM_MAX);
    clnt->sess.pool = pool;
    clnt->sess.hbint = 0;
    dbr_tree_init(&clnt->sess.traders);
    dbr_tree_init(&clnt->sess.subs);

    // 3.
    void* mdsock = zmq_socket(ctx, ZMQ_SUB);
    if (!mdsock) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    zmq_setsockopt(mdsock, ZMQ_SUBSCRIBE, "", 0);

    if (zmq_connect(mdsock, mdaddr) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }

    // 4.
    void* trsock = zmq_socket(ctx, ZMQ_DEALER);
    if (!trsock) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }
    zmq_setsockopt(trsock, ZMQ_IDENTITY, sess, strnlen(sess, DBR_MNEM_MAX));

    if (zmq_connect(trsock, traddr) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail4;
    }

    // 5.
    void* asock = zmq_socket(ctx, ZMQ_REP);
    if (!asock) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail4;
    }

    char aaddr[sizeof("inproc://") + DBR_MNEM_MAX];
    sprintf(aaddr, "inproc://%.16s", sess);
    if (zmq_bind(asock, aaddr) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_bind() failed: %s", zmq_strerror(zmq_errno()));
        goto fail5;
    }

    clnt->mdsock = mdsock;
    clnt->trsock = trsock;
    clnt->asock = asock;
    clnt->id = seed;
    clnt->pool = pool;
    clnt->flags = INIT_PENDING | TRADER_PENDING | ACCNT_PENDING | CONTR_PENDING
        | VIEW_PENDING | TR_DOWN | MD_DOWN;
    // 6.
    fig_cache_init(&clnt->cache, term_state, pool);
    fig_ordidx_init(&clnt->ordidx);
    dbr_queue_init(&clnt->execs);
    // 7.
    dbr_tree_init(&clnt->views);
    dbr_tree_init(&clnt->posnups);
    dbr_tree_init(&clnt->viewups);
    // 8.
    if (!dbr_prioq_init(&clnt->prioq))
        goto fail6;

    clnt->mdlast = 0;

    clnt->items[MDSOCK].socket = mdsock;
    clnt->items[MDSOCK].fd = 0;
    clnt->items[MDSOCK].events = ZMQ_POLLIN;
    clnt->items[MDSOCK].revents = 0;

    clnt->items[TRSOCK].socket = trsock;
    clnt->items[TRSOCK].fd = 0;
    clnt->items[TRSOCK].events = ZMQ_POLLIN;
    clnt->items[TRSOCK].revents = 0;

    clnt->items[ASOCK].socket = asock;
    clnt->items[ASOCK].fd = 0;
    clnt->items[ASOCK].events = ZMQ_POLLIN;
    clnt->items[ASOCK].revents = 0;

    if (!dbr_prioq_push(&clnt->prioq, MDTMR, dbr_millis() + MDTMOUT))
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
        // Ensure that executions are freed.
        dbr_clnt_clear(clnt);
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

DBR_API DbrIden
dbr_clnt_close(DbrClnt clnt, DbrMillis ms)
{
    if (clnt->flags != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }

    struct DbrBody body = { .req_id = clnt->id++, .type = DBR_SESS_CLOSE };

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + ms);
    dbr_prioq_replace(&clnt->prioq, HBTMR, now + clnt->sess.hbint);
    return body.req_id;
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

DBR_API DbrTrader
dbr_clnt_trader(DbrClnt clnt, struct DbrRec* trec)
{
    return fig_trader_lazy(trec, &clnt->ordidx, clnt->pool);
}

DBR_API DbrAccnt
dbr_clnt_accnt(DbrClnt clnt, struct DbrRec* arec)
{
    return fig_accnt_lazy(arec, clnt->pool);
}

DBR_API DbrIden
dbr_clnt_logon(DbrClnt clnt, DbrTrader trader, DbrMillis ms)
{
    if (clnt->flags != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }

    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_SESS_LOGON;
    body.sess_logon.tid = trader->rec->id;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + ms);
    dbr_prioq_replace(&clnt->prioq, HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_logoff(DbrClnt clnt, DbrTrader trader, DbrMillis ms)
{
    if (clnt->flags != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_SESS_LOGOFF;
    body.sess_logoff.tid = trader->rec->id;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + ms);
    dbr_prioq_replace(&clnt->prioq, HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_place(DbrClnt clnt, DbrTrader trader, DbrAccnt accnt, struct DbrRec* crec,
               DbrDate settl_date, const char* ref, int action, DbrTicks ticks, DbrLots lots,
               DbrLots min_lots, DbrMillis ms)
{
    if (clnt->flags != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_PLACE_ORDER_REQ;
    body.place_order_req.tid = trader->rec->id;
    body.place_order_req.aid = accnt->rec->id;
    body.place_order_req.cid = crec->id;
    body.place_order_req.settl_date = settl_date;
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
    dbr_prioq_push(&clnt->prioq, body.req_id, now + ms);
    dbr_prioq_replace(&clnt->prioq, HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_revise_id(DbrClnt clnt, DbrTrader trader, DbrIden id, DbrLots lots, DbrMillis ms)
{
    if (clnt->flags != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_REVISE_ORDER_ID_REQ;
    body.revise_order_id_req.tid = trader->rec->id;
    body.revise_order_id_req.id = id;
    body.revise_order_id_req.lots = lots;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + ms);
    dbr_prioq_replace(&clnt->prioq, HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_revise_ref(DbrClnt clnt, DbrTrader trader, const char* ref, DbrLots lots, DbrMillis ms)
{
    if (clnt->flags != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_REVISE_ORDER_REF_REQ;
    body.revise_order_ref_req.tid = trader->rec->id;
    strncpy(body.revise_order_ref_req.ref, ref, DBR_REF_MAX);
    body.revise_order_ref_req.lots = lots;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + ms);
    dbr_prioq_replace(&clnt->prioq, HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_cancel_id(DbrClnt clnt, DbrTrader trader, DbrIden id, DbrMillis ms)
{
    if (clnt->flags != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_CANCEL_ORDER_ID_REQ;
    body.cancel_order_id_req.tid = trader->rec->id;
    body.cancel_order_id_req.id = id;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + ms);
    dbr_prioq_replace(&clnt->prioq, HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_cancel_ref(DbrClnt clnt, DbrTrader trader, const char* ref, DbrMillis ms)
{
    if (clnt->flags != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_CANCEL_ORDER_REF_REQ;
    body.cancel_order_ref_req.tid = trader->rec->id;
    strncpy(body.cancel_order_ref_req.ref, ref, DBR_REF_MAX);

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + ms);
    dbr_prioq_replace(&clnt->prioq, HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_ack_trade(DbrClnt clnt, DbrTrader trader, DbrIden id, DbrMillis ms)
{
    if (clnt->flags != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_ACK_TRADE_REQ;
    body.ack_trade_req.tid = trader->rec->id;
    body.ack_trade_req.id = id;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
        goto fail1;

    fig_trader_remove_trade_id(trader, id);

    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, body.req_id, now + ms);
    dbr_prioq_replace(&clnt->prioq, HBTMR, now + clnt->sess.hbint);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrBool
dbr_clnt_is_open(DbrClnt clnt)
{
    return (clnt->flags & TR_CLOSED) == 0;
}

DBR_API DbrBool
dbr_clnt_is_ready(DbrClnt clnt)
{
    return clnt->flags == 0;
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

DBR_API DbrBool
dbr_clnt_dispatch(DbrClnt clnt, DbrMillis ms, DbrHandler handler)
{
    assert(ms >= 0);
    DbrMillis now = dbr_millis();
    const DbrMillis absms = now + ms;
    // At least one iteration.
    do {
        const struct DbrElem* elem;
        while ((elem = dbr_prioq_top(&clnt->prioq))) {
            if (elem->key > now) {
                // Millis until next timeout.
                ms = dbr_min(absms, elem->key) - now;
                break;
            }
            // Internal timeout.
            const DbrKey key = elem->key;
            const DbrIden id = elem->id;
            dbr_prioq_pop(&clnt->prioq);

            if (id == HBTMR) {
                // Cannot fail due to pop.
                dbr_prioq_push(&clnt->prioq, id, key + clnt->sess.hbint);
                struct DbrBody body = { .req_id = 0, .type = DBR_SESS_HEARTBT };
                if (!dbr_send_body(clnt->trsock, &body, DBR_FALSE))
                    goto fail1;
                // Next heartbeat may have already expired.
            } else if (id == MDTMR) {
                // Cannot fail due to pop.
                dbr_prioq_push(&clnt->prioq, MDTMR, key + MDTMOUT);
                if (!(clnt->flags & MD_DOWN)) {
                    clnt->flags |= MD_DOWN;
                    dbr_handler_on_down(handler, DBR_CONN_MD);
                }
            } else if (id == TRTMR) {
                // Cannot fail due to pop.
                dbr_prioq_push(&clnt->prioq, id, key + TRTMOUT);
                if (!(clnt->flags & TR_DOWN)) {
                    clnt->flags |= TR_DOWN;
                    dbr_handler_on_down(handler, DBR_CONN_TR);
                }
            } else {
                // Assumed that these "top-half" handlers do not block.
                dbr_handler_on_timeout(handler, id);
            }
        }

        // From the zmq_poll() man page:

        // For each zmq_pollitem_t item, zmq_poll() shall first clear the revents member, and then
        // indicate any requested events that have occurred by setting the bit corresponding to the
        // event condition in the revents member.

        // Note also that zmq_poll() is level-triggered.

        const int nevents = zmq_poll(clnt->items, NSOCK, ms);
        if (nevents < 0) {
            dbr_err_setf(DBR_EIO, "zmq_poll() failed: %s", zmq_strerror(zmq_errno()));
            goto fail1;
        }
        // Current time after slow operation.
        now = dbr_millis();
        if (nevents == 0)
            continue;

        struct DbrBody body;
        if ((clnt->items[TRSOCK].revents & ZMQ_POLLIN)) {

            if (!dbr_recv_body(clnt->trsock, clnt->pool, &body))
                goto fail1;

            if (body.req_id > 0)
                dbr_prioq_remove(&clnt->prioq, body.req_id);

            dbr_prioq_replace(&clnt->prioq, TRTMR, now + TRTMOUT);

            if (body.type != DBR_SESS_CLOSE && (clnt->flags & (TR_CLOSED | TR_DOWN)) == TR_DOWN) {
                clnt->flags &= ~TR_DOWN;
                dbr_handler_on_up(handler, DBR_CONN_TR);
            }
            switch (body.type) {
            case DBR_SESS_OPEN:
                clnt->sess.hbint = body.sess_open.hbint;
                if (!dbr_prioq_push(&clnt->prioq, HBTMR, now + clnt->sess.hbint))
                    goto fail1;
                break;
            case DBR_SESS_CLOSE:
                clnt->flags |= (TR_CLOSED | TR_DOWN);
                dbr_handler_on_down(handler, DBR_CONN_TR);
                break;
            case DBR_SESS_LOGON:
                dbr_sess_logon(&clnt->sess, get_trader(clnt, body.sess_logon.tid));
                dbr_handler_on_logon(handler, body.sess_logon.tid);
                break;
            case DBR_SESS_LOGOFF:
                dbr_handler_on_logoff(handler, body.sess_logoff.tid);
                {
                    DbrTrader trader = get_trader(clnt, body.sess_logoff.tid);
                    dbr_sess_logoff(&clnt->sess, trader, DBR_TRUE);
                    fig_trader_clear(trader);
                }
                break;
            case DBR_SESS_HEARTBT:
                break;
            case DBR_STATUS_REP:
                dbr_handler_on_status(handler, body.req_id, body.status_rep.num,
                                      body.status_rep.msg);
                break;
            case DBR_TRADER_LIST_REP:
                emplace_rec_list(clnt, DBR_ENTITY_TRADER, TRADER_PENDING,
                                 body.entity_list_rep.first, body.entity_list_rep.count_);
                break;
            case DBR_ACCNT_LIST_REP:
                emplace_rec_list(clnt, DBR_ENTITY_ACCNT, ACCNT_PENDING,
                                 body.entity_list_rep.first, body.entity_list_rep.count_);
                break;
            case DBR_CONTR_LIST_REP:
                emplace_rec_list(clnt, DBR_ENTITY_CONTR, CONTR_PENDING,
                                 body.entity_list_rep.first, body.entity_list_rep.count_);
                break;
            case DBR_ORDER_LIST_REP:
                emplace_order_list(clnt, body.entity_list_rep.first);
                break;
            case DBR_EXEC_LIST_REP:
                emplace_exec_list(clnt, body.entity_list_rep.first);
                break;
            case DBR_MEMB_LIST_REP:
                // This function can fail is there is no memory available for a lazily created
                // account.
                if (dbr_unlikely(!emplace_memb_list(clnt, body.entity_list_rep.first)))
                    goto fail1;
                break;
            case DBR_POSN_LIST_REP:
                // This function can fail is there is no memory available for a lazily created
                // account.
                if (dbr_unlikely(!emplace_posn_list(clnt, body.entity_list_rep.first)))
                    goto fail1;
                break;
            case DBR_VIEW_LIST_REP:
                apply_views(clnt, body.view_list_rep.first, body.view_list_rep.created,
                            handler);
                clnt->flags &= ~VIEW_PENDING;
                break;
            case DBR_EXEC_REP:
                enrich_exec(&clnt->cache, body.exec_rep.exec);
                switch (body.exec_rep.exec->c.state) {
                case DBR_STATE_NEW:
                    apply_new(clnt, body.exec_rep.exec);
                    break;
                case DBR_STATE_REVISE:
                case DBR_STATE_CANCEL:
                case DBR_STATE_TRADE:
                    apply_update(clnt, body.exec_rep.exec);
                    break;
                }
                dbr_handler_on_exec(handler, body.req_id, body.exec_rep.exec);
                dbr_exec_decref(body.exec_rep.exec, clnt->pool);
                break;
            case DBR_POSN_REP:
                enrich_posn(&clnt->cache, body.posn_rep.posn);
                // This function can fail is there is no memory available for a lazily created
                // account.
                if (dbr_unlikely(!apply_posnup(clnt, body.posn_rep.posn)))
                    goto fail1;
                dbr_handler_on_posn(handler, body.posn_rep.posn);
                break;
            default:
                dbr_err_setf(DBR_EIO, "unknown body-type '%d'", body.type);
                goto fail1;
            }
        }

        if ((clnt->items[MDSOCK].revents & ZMQ_POLLIN)) {

            if (!dbr_recv_body(clnt->mdsock, clnt->pool, &body))
                goto fail1;

            dbr_prioq_replace(&clnt->prioq, MDTMR, now + MDTMOUT);

            if ((clnt->flags & MD_DOWN)) {
                clnt->flags &= ~MD_DOWN;
                dbr_handler_on_up(handler, DBR_CONN_MD);
            }
            switch (body.type) {
            case DBR_VIEW_LIST_REP:
                if (dbr_unlikely(clnt->flags & INIT_PENDING)) {
                    if (init(clnt, now) < 0)
                        goto fail1;
                    clnt->flags &= ~INIT_PENDING;
                } else
                    apply_views(clnt, body.view_list_rep.first, body.view_list_rep.created,
                                handler);
                break;
            default:
                dbr_err_setf(DBR_EIO, "unknown body-type '%d'", body.type);
                goto fail1;
            }
        }

        if ((clnt->items[ASOCK].revents & ZMQ_POLLIN)) {

            void* val;
            if (!async_recv(clnt->asock, &val))
                goto fail1;

            val = dbr_handler_on_async(handler, val);

            if (!async_send(clnt->asock, val))
                goto fail1;
        }
    } while (now < absms);
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

DBR_API void
dbr_clnt_mdclear(DbrClnt clnt)
{
    dbr_tree_init(&clnt->viewups);
}

DBR_API void
dbr_clnt_trclear(DbrClnt clnt)
{
    struct DbrSlNode* node = clnt->execs.first;
    while (node) {
        struct DbrExec* exec = dbr_clnt_exec_entry(node);
        node = node->next;
        // Free completed orders.
        if (dbr_exec_done(exec)) {
            DbrTrader trader = exec->c.trader.rec->trader.state;
            assert(trader);
            struct DbrOrder* order = fig_trader_release_order_id(trader, exec->order);
            if (order)
                dbr_pool_free_order(clnt->pool, order);
        }
        dbr_exec_decref(exec, clnt->pool);
    }
    dbr_queue_init(&clnt->execs);
    dbr_tree_init(&clnt->posnups);
}

DBR_API struct DbrSlNode*
dbr_clnt_first_exec(DbrClnt clnt)
{
    return dbr_queue_first(&clnt->execs);
}

DBR_API DbrBool
dbr_clnt_empty_exec(DbrClnt clnt)
{
    return dbr_queue_empty(&clnt->execs);
}

DBR_API struct DbrRbNode*
dbr_clnt_first_posnup(DbrClnt clnt)
{
    return dbr_tree_first(&clnt->posnups);
}

DBR_API struct DbrRbNode*
dbr_clnt_last_posnup(DbrClnt clnt)
{
    return dbr_tree_last(&clnt->posnups);
}

DBR_API DbrBool
dbr_clnt_empty_posnup(DbrClnt clnt)
{
    return dbr_tree_empty(&clnt->posnups);
}

DBR_API struct DbrRbNode*
dbr_clnt_find_view(DbrClnt clnt, DbrIden cid, DbrDate settl_date)
{
    const DbrIden key = dbr_book_key(cid, settl_date);
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

DBR_API struct DbrRbNode*
dbr_clnt_first_viewup(DbrClnt clnt)
{
    return dbr_tree_first(&clnt->viewups);
}

DBR_API struct DbrRbNode*
dbr_clnt_last_viewup(DbrClnt clnt)
{
    return dbr_tree_last(&clnt->viewups);
}

DBR_API DbrBool
dbr_clnt_empty_viewup(DbrClnt clnt)
{
    return dbr_tree_empty(&clnt->viewups);
}
