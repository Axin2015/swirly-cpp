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
#include <dbr/util.h>

#include <stdlib.h> // malloc()
#include <string.h> // strncpy()

enum { DEALER_SOCK, SUB_SOCK, HBINT_IN = 2000, HBTIMEOUT_IN = (HBINT_IN * 3) / 2 };

struct FigClnt {
    void* ctx;
    void* dealer;
    void* sub;
    DbrMnem mnem;
    DbrIden id;
    DbrPool pool;
    int pending;
    DbrTrader trader;
    struct FigCache cache;
    struct FigIndex index;
    struct DbrQueue execs;
    struct DbrTree views;
    struct DbrTree posnups;
    struct DbrTree viewups;
    struct DbrPrioq prioq;
    DbrIden hbid_in;
    DbrIden hbid_out;
    // The heartbeat interval requested by the server.
    DbrMillis hbint_out;
    zmq_pollitem_t* items;
    int nitems;
};

static void
term_state(struct DbrRec* rec)
{
    switch (rec->type) {
    case DBR_TRADER:
        fig_trader_term(rec);
        break;
    case DBR_ACCNT:
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
    order->c.trader.rec = get_id(cache, DBR_TRADER, order->c.trader.id_only);
    order->c.accnt.rec = get_id(cache, DBR_ACCNT, order->c.accnt.id_only);
    order->c.contr.rec = get_id(cache, DBR_CONTR, order->c.contr.id_only);
    return order;
}

static inline struct DbrExec*
enrich_exec(struct FigCache* cache, struct DbrExec* exec)
{
    exec->c.trader.rec = get_id(cache, DBR_TRADER, exec->c.trader.id_only);
    exec->c.accnt.rec = get_id(cache, DBR_ACCNT, exec->c.accnt.id_only);
    exec->c.contr.rec = get_id(cache, DBR_CONTR, exec->c.contr.id_only);
    if (exec->cpty.id_only)
        exec->cpty.rec = get_id(cache, DBR_ACCNT, exec->cpty.id_only);
    else
        exec->cpty.rec = NULL;
    return exec;
}

static inline struct DbrMemb*
enrich_memb(struct FigCache* cache, struct DbrMemb* memb)
{
    memb->trader.rec = get_id(cache, DBR_TRADER, memb->trader.id_only);
    memb->accnt.rec = get_id(cache, DBR_ACCNT, memb->accnt.id_only);
    return memb;
}

static inline struct DbrPosn*
enrich_posn(struct FigCache* cache, struct DbrPosn* posn)
{
    posn->accnt.rec = get_id(cache, DBR_ACCNT, posn->accnt.id_only);
    posn->contr.rec = get_id(cache, DBR_CONTR, posn->contr.id_only);
    return posn;
}

static inline struct DbrView*
enrich_view(struct FigCache* cache, struct DbrView* view)
{
    view->contr.rec = get_id(cache, DBR_CONTR, view->contr.id_only);
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

static DbrIden
logon(DbrClnt clnt)
{
    struct DbrBody body = { .req_id = clnt->id++, .type = DBR_SESS_LOGON,
                            .sess_logon = { .hbint = HBINT_IN } };
    if (!dbr_send_body(clnt->dealer, &body, DBR_FALSE))
        return -1;
    return body.req_id;
}

static DbrIden
heartbt(DbrClnt clnt)
{
    struct DbrBody body = { .req_id = clnt->id++, .type = DBR_SESS_HEARTBT };
    if (!dbr_send_body(clnt->dealer, &body, DBR_FALSE))
        return -1;
    return body.req_id;
}

static inline DbrBool
set_trader(DbrClnt clnt)
{
    struct DbrSlNode* node = fig_cache_find_rec_mnem(&clnt->cache, DBR_TRADER, clnt->mnem);
    if (node == FIG_CACHE_END_REC)
        goto fail1;

    struct DbrRec* trec = dbr_shared_rec_entry(node);
    DbrTrader trader = fig_trader_lazy(trec, &clnt->index, clnt->pool);
    if (!trader)
        goto fail1;

    clnt->trader = trader;
    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

static void
emplace_rec_list(DbrClnt clnt, int type, struct DbrSlNode* first, size_t count)
{
    fig_cache_emplace_rec_list(&clnt->cache, type, first, count);
    clnt->pending &= ~type;
    if ((clnt->pending & (DBR_TRADER | DBR_ACCNT | DBR_CONTR)) == 0) {
        if (!set_trader(clnt))
            abort();
    }
}

static void
emplace_order_list(DbrClnt clnt, struct DbrSlNode* first)
{
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrOrder* order = enrich_order(&clnt->cache, dbr_shared_order_entry(node));
        // Transfer ownership.
        fig_trader_emplace_order(clnt->trader, order);
    }
    clnt->pending &= ~DBR_ORDER;
}

static void
emplace_exec_list(DbrClnt clnt, struct DbrSlNode* first)
{
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrExec* exec = enrich_exec(&clnt->cache, dbr_shared_exec_entry(node));
        assert(exec->c.state == DBR_TRADE);
        // Transfer ownership.
        fig_trader_insert_trade(clnt->trader, exec);
        dbr_exec_decref(exec, clnt->pool);
    }
    clnt->pending &= ~DBR_EXEC;
}

static void
emplace_memb_list(DbrClnt clnt, struct DbrSlNode* first)
{
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrMemb* memb = enrich_memb(&clnt->cache, dbr_shared_memb_entry(node));
        // Transfer ownership.
        fig_trader_emplace_memb(clnt->trader, memb);
        DbrAccnt accnt = fig_accnt_lazy(memb->accnt.rec, clnt->pool);
        if (!accnt)
            abort();
    }
    clnt->pending &= ~DBR_MEMB;
}

static void
emplace_posn_list(DbrClnt clnt, struct DbrSlNode* first)
{
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrPosn* posn = enrich_posn(&clnt->cache, dbr_shared_posn_entry(node));
        // Transfer ownership.
        // All accnts that trader is member of are created in emplace_membs().
        DbrAccnt accnt = posn->accnt.rec->accnt.state;
        assert(accnt);
        fig_accnt_emplace_posn(accnt, posn);
    }
    clnt->pending &= ~DBR_POSN;
}

static void
emplace_view_list(DbrClnt clnt, struct DbrSlNode* first)
{
    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrView* view = enrich_view(&clnt->cache, dbr_shared_view_entry(node));
        dbr_tree_insert(&clnt->views, dbr_book_key(view->contr.rec->id, view->settl_date),
                        &view->clnt_node_);
    }
    clnt->pending &= ~DBR_VIEW;
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
    // Transfer ownership.
    fig_trader_emplace_order(clnt->trader, order);
    dbr_exec_incref(exec);
    dbr_queue_insert_back(&clnt->execs, &exec->shared_node_);
    return DBR_TRUE;
}

static DbrBool
apply_update(DbrClnt clnt, struct DbrExec* exec)
{
    struct DbrRbNode* node = fig_trader_find_order_id(clnt->trader, exec->order);
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

    if (exec->c.state == DBR_TRADE) {
        // Transfer ownership.
        fig_trader_insert_trade(clnt->trader, exec);
    }
    dbr_exec_incref(exec);
    dbr_queue_insert_back(&clnt->execs, &exec->shared_node_);
    return DBR_TRUE;
}

static void
apply_posnup(DbrClnt clnt, struct DbrPosn* posn)
{
    posn = fig_accnt_update_posn(posn->accnt.rec->accnt.state, posn);
    insert_posnup(&clnt->posnups, posn);
}

static void
apply_viewup(DbrClnt clnt, struct DbrView* view)
{
    const DbrIden key = dbr_book_key(view->contr.rec->id, view->settl_date);
    struct DbrRbNode* node = dbr_tree_insert(&clnt->views, key, &view->clnt_node_);
    if (node != &view->clnt_node_) {
        struct DbrView* curr = dbr_clnt_view_entry(node);

        // Update existing position.

        assert(curr->contr.rec == view->contr.rec);
        assert(curr->settl_date == view->settl_date);

        for (size_t i = 0; i < DBR_LEVEL_MAX; ++i) {
            curr->bid_ticks[i] = view->bid_ticks[i];
            curr->bid_lots[i] = view->bid_lots[i];
            curr->bid_count[i] = view->bid_count[i];
            curr->ask_ticks[i] = view->ask_ticks[i];
            curr->ask_lots[i] = view->ask_lots[i];
            curr->ask_count[i] = view->ask_count[i];
        }

        dbr_pool_free_view(clnt->pool, view);
        view = curr;
    }
    insert_viewup(&clnt->viewups, view);
}

static inline void
resethb_in(DbrClnt clnt)
{
    dbr_prioq_replace(&clnt->prioq, clnt->hbid_in, dbr_millis() + HBTIMEOUT_IN);
}

static inline void
resethb_out(DbrClnt clnt, DbrIden req_id, DbrMillis ms)
{
    const DbrMillis now = dbr_millis();
    dbr_prioq_push(&clnt->prioq, req_id, now + ms);
    dbr_prioq_replace(&clnt->prioq, clnt->hbid_out, now + clnt->hbint_out);
}

static void
settimeout(DbrIden req_id, struct DbrEvent* event)
{
    event->req_id = req_id;
    event->type = DBR_STATUS_REP;
    event->status_rep.num = DBR_ETIMEOUT;
    strncpy(event->status_rep.msg, "request timeout", DBR_ERRMSG_MAX);
}

DBR_API DbrClnt
dbr_clnt_create(void* ctx, const char* dealer_addr, const char* sub_addr, const char* trader,
                DbrIden seed, DbrPool pool)
{
    DbrClnt clnt = malloc(sizeof(struct FigClnt));
    if (dbr_unlikely(!clnt)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    void* dealer = zmq_socket(ctx, ZMQ_DEALER);
    if (!dealer) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    zmq_setsockopt(dealer, ZMQ_IDENTITY, trader, strnlen(trader, DBR_MNEM_MAX));

    if (zmq_connect(dealer, dealer_addr) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }

    void* sub = zmq_socket(ctx, ZMQ_SUB);
    if (!sub) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }
    zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "", 0);

    if (zmq_connect(sub, sub_addr) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail4;
    }

    clnt->dealer = dealer;
    clnt->sub = sub;
    strncpy(clnt->mnem, trader, DBR_MNEM_MAX);
    clnt->id = seed;
    clnt->pool = pool;
    clnt->pending = DBR_TRADER | DBR_ACCNT | DBR_CONTR | DBR_ORDER | DBR_EXEC | DBR_MEMB
        | DBR_POSN | DBR_VIEW;
    clnt->trader = NULL;
    fig_cache_init(&clnt->cache, term_state, pool);
    fig_index_init(&clnt->index);
    dbr_queue_init(&clnt->execs);
    dbr_tree_init(&clnt->views);
    dbr_tree_init(&clnt->posnups);
    dbr_tree_init(&clnt->viewups);
    if (!dbr_prioq_init(&clnt->prioq))
        goto fail5;

    // The inbound heartbeat timeout timer is scheduled prior to sending our logon request.
    clnt->hbid_in = clnt->id++;

    // The outbound heartbeat timer is actually scheduled once the logon response has been received.
    clnt->hbid_out = clnt->id++;
    clnt->hbint_out = 0; // Pending hbint from server's logon message.

    clnt->items = malloc(2 * sizeof(zmq_pollitem_t));
    if (!clnt->items)
        goto fail6;

    clnt->items[DEALER_SOCK].socket = dealer;
    clnt->items[DEALER_SOCK].fd = 0;
    clnt->items[DEALER_SOCK].events = ZMQ_POLLIN;
    clnt->items[DEALER_SOCK].revents = 0;

    clnt->items[SUB_SOCK].socket = sub;
    clnt->items[SUB_SOCK].fd = 0;
    clnt->items[SUB_SOCK].events = ZMQ_POLLIN;
    clnt->items[SUB_SOCK].revents = 0;

    clnt->nitems = 2;

    if (!dbr_prioq_push(&clnt->prioq, clnt->hbid_in, dbr_millis() + HBTIMEOUT_IN))
        goto fail7;

    if (!logon(clnt))
        goto fail8;
    return clnt;
 fail8:
    dbr_prioq_remove(&clnt->prioq, clnt->hbid_in);
 fail7:
    free(clnt->items);
 fail6:
    dbr_prioq_term(&clnt->prioq);
 fail5:
    fig_cache_term(&clnt->cache);
 fail4:
    zmq_close(sub);
 fail3:
    zmq_close(dealer);
 fail2:
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
        free_views(&clnt->views, clnt->pool);
        free(clnt->items);
        dbr_prioq_term(&clnt->prioq);
        fig_cache_term(&clnt->cache);
        zmq_close(clnt->sub);
        zmq_close(clnt->dealer);
        free(clnt);
    }
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
dbr_clnt_trader(DbrClnt clnt)
{
    return clnt->trader;
}

DBR_API DbrAccnt
dbr_clnt_accnt(DbrClnt clnt, struct DbrRec* arec)
{
    return fig_accnt_lazy(arec, clnt->pool);
}

DBR_API DbrIden
dbr_clnt_place(DbrClnt clnt, const char* accnt, const char* contr, DbrDate settl_date,
               const char* ref, int action, DbrTicks ticks, DbrLots lots, DbrLots min_lots,
               DbrMillis ms)
{
    if (clnt->pending != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_PLACE_ORDER_REQ;
    strncpy(body.place_order_req.accnt, accnt, DBR_MNEM_MAX);
    strncpy(body.place_order_req.contr, contr, DBR_MNEM_MAX);
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

    if (!dbr_send_body(clnt->dealer, &body, DBR_FALSE))
        goto fail1;

    resethb_out(clnt, body.req_id, ms);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_revise_id(DbrClnt clnt, DbrIden id, DbrLots lots, DbrMillis ms)
{
    if (clnt->pending != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_REVISE_ORDER_ID_REQ;
    body.revise_order_id_req.id = id;
    body.revise_order_id_req.lots = lots;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->dealer, &body, DBR_FALSE))
        goto fail1;

    resethb_out(clnt, body.req_id, ms);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_revise_ref(DbrClnt clnt, const char* ref, DbrLots lots, DbrMillis ms)
{
    if (clnt->pending != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_REVISE_ORDER_REF_REQ;
    strncpy(body.revise_order_ref_req.ref, ref, DBR_REF_MAX);
    body.revise_order_ref_req.lots = lots;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->dealer, &body, DBR_FALSE))
        goto fail1;

    resethb_out(clnt, body.req_id, ms);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_cancel_id(DbrClnt clnt, DbrIden id, DbrMillis ms)
{
    if (clnt->pending != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_CANCEL_ORDER_ID_REQ;
    body.cancel_order_id_req.id = id;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->dealer, &body, DBR_FALSE))
        goto fail1;

    resethb_out(clnt, body.req_id, ms);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_cancel_ref(DbrClnt clnt, const char* ref, DbrMillis ms)
{
    if (clnt->pending != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_CANCEL_ORDER_REF_REQ;
    strncpy(body.cancel_order_ref_req.ref, ref, DBR_REF_MAX);

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->dealer, &body, DBR_FALSE))
        goto fail1;

    resethb_out(clnt, body.req_id, ms);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrIden
dbr_clnt_ack_trade(DbrClnt clnt, DbrIden id, DbrMillis ms)
{
    if (clnt->pending != 0) {
        dbr_err_set(DBR_EBUSY, "client not ready");
        goto fail1;
    }
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_ACK_TRADE_REQ;
    body.ack_trade_req.id = id;

    // Reserve so that push cannot fail after send.
    if (!dbr_prioq_reserve(&clnt->prioq, dbr_prioq_size(&clnt->prioq) + 1))
        goto fail1;

    if (!dbr_send_body(clnt->dealer, &body, DBR_FALSE))
        goto fail1;

    fig_trader_remove_trade_id(clnt->trader, id);

    resethb_out(clnt, body.req_id, ms);
    return body.req_id;
 fail1:
    return -1;
}

DBR_API DbrBool
dbr_clnt_ready(DbrClnt clnt)
{
    return clnt->pending == 0;
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

DBR_API zmq_pollitem_t*
dbr_clnt_setitems(DbrClnt clnt, zmq_pollitem_t* items, int nitems)
{
    // The first two items are reserved for the dealer and sub sockets.
    zmq_pollitem_t* new_items = realloc(clnt->items, (2 + nitems) * sizeof(zmq_pollitem_t));
    if (!new_items) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        return NULL;
    }
    // Copy new items to tail.
    __builtin_memcpy(&new_items[2], items, nitems * sizeof(zmq_pollitem_t));
    clnt->items = new_items;
    clnt->nitems = 2 + nitems;
    // Return pointer to third item, reserving the first two for internal use.
    return &new_items[2];
}

DBR_API int
dbr_clnt_poll(DbrClnt clnt, DbrMillis ms, struct DbrEvent* event)
{
    event->req_id = 0;
    event->type = 0;

    // TODO: min of ms and timer.

    // Implementation notes: events on the dealer socket take precedence over those on the sub
    // socket. If events are triggered on both, then the event on the sub socket will be processed
    // on the next call to dbr_clnt_poll(). Once an event has been processed, the revents member of
    // zmq_pollitem_t is cleared. If revents is still set on entry to this function, then an event
    // is assumed to be pending from a previous call.

    int nevents;
    if (!(clnt->items[SUB_SOCK].revents & ZMQ_POLLIN)) {

        const DbrMillis now = dbr_millis();

        const struct DbrElem* elem;
        while ((elem = dbr_prioq_top(&clnt->prioq))) {
            if (elem->key > now) {
                // Millis until next timeout.
                ms = dbr_min(ms, elem->key - now);
                break;
            }
            // Timeout.
            const DbrKey key = elem->key;
            const DbrIden id = elem->id;
            dbr_prioq_pop(&clnt->prioq);

            if (id == clnt->hbid_out) {
                if (!dbr_prioq_push(&clnt->prioq, id, key + clnt->hbint_out))
                    goto fail1;
                if (!heartbt(clnt))
                    goto fail1;
                // Next heartbeat may have already expired.
            } else {
                settimeout(id, event);
                return 0;
            }
        }

        // From the zmq_poll() man page:

        // For each zmq_pollitem_t item, zmq_poll() shall first clear the revents member, and then
        // indicate any requested events that have occurred by setting the bit corresponding to the
        // event condition in the revents member.

        nevents = zmq_poll(clnt->items, clnt->nitems, ms);
        if (nevents < 0) {
            dbr_err_setf(DBR_EIO, "zmq_poll() failed: %s", zmq_strerror(zmq_errno()));
            goto fail1;
        }

        if (nevents == 0) {

            // Timeout.

            while (elem && elem->key <= dbr_millis()) {
                const DbrKey key = elem->key;
                const DbrIden id = elem->id;
                dbr_prioq_pop(&clnt->prioq);

                if (id == clnt->hbid_out) {
                    if (!dbr_prioq_push(&clnt->prioq, id, key + clnt->hbint_out))
                        goto fail1;
                    if (!heartbt(clnt))
                        goto fail1;
                    // Next heartbeat may have already expired.
                    elem = dbr_prioq_top(&clnt->prioq);
                } else {
                    settimeout(id, event);
                    break;
                }
            }
            return 0;
        }

    } else {

        // Consume pending input event on the sub socket from previous call.

        nevents = 1;
    }

    struct DbrBody body;
    if ((clnt->items[DEALER_SOCK].revents & ZMQ_POLLIN)) {

        nevents -= (clnt->items[SUB_SOCK].revents & ZMQ_POLLIN) ? 2 : 1;
        // Clear event once consumed.
        clnt->items[DEALER_SOCK].revents &= ~ZMQ_POLLIN;

        if (!dbr_recv_body(clnt->dealer, clnt->pool, &body))
            goto fail1;

        if ((event->req_id = body.req_id) > 0)
            dbr_prioq_remove(&clnt->prioq, body.req_id);

        switch ((event->type = body.type)) {
        case DBR_SESS_LOGON:
            dbr_log_info("hbint: %d", body.sess_logon.hbint);
            clnt->hbint_out = body.sess_logon.hbint;
            if (!dbr_prioq_push(&clnt->prioq, clnt->hbid_out, dbr_millis() + clnt->hbint_out))
                goto fail1;
            break;
        case DBR_STATUS_REP:
            event->status_rep.num = body.status_rep.num;
            strncpy(event->status_rep.msg, body.status_rep.msg, DBR_ERRMSG_MAX);
            break;
        case DBR_TRADER_LIST_REP:
            emplace_rec_list(clnt, DBR_TRADER, body.entity_list_rep.first,
                             body.entity_list_rep.count_);
            break;
        case DBR_ACCNT_LIST_REP:
            emplace_rec_list(clnt, DBR_ACCNT, body.entity_list_rep.first,
                             body.entity_list_rep.count_);
            break;
        case DBR_CONTR_LIST_REP:
            emplace_rec_list(clnt, DBR_CONTR, body.entity_list_rep.first,
                             body.entity_list_rep.count_);
            break;
        case DBR_ORDER_LIST_REP:
            emplace_order_list(clnt, body.entity_list_rep.first);
            break;
        case DBR_EXEC_LIST_REP:
            emplace_exec_list(clnt, body.entity_list_rep.first);
            break;
        case DBR_MEMB_LIST_REP:
            emplace_memb_list(clnt, body.entity_list_rep.first);
            break;
        case DBR_POSN_LIST_REP:
            emplace_posn_list(clnt, body.entity_list_rep.first);
            break;
        case DBR_VIEW_LIST_REP:
            emplace_view_list(clnt, body.view_list_rep.first);
            break;
        case DBR_EXEC_REP:
            enrich_exec(&clnt->cache, body.exec_rep.exec);
            switch (body.exec_rep.exec->c.state) {
            case DBR_NEW:
                apply_new(clnt, body.exec_rep.exec);
                break;
            case DBR_REVISE:
            case DBR_CANCEL:
            case DBR_TRADE:
                apply_update(clnt, body.exec_rep.exec);
                break;
            }
            event->exec_rep.exec = body.exec_rep.exec;
            dbr_exec_decref(body.exec_rep.exec, clnt->pool);
            break;
        case DBR_POSN_REP:
            enrich_posn(&clnt->cache, body.posn_rep.posn);
            apply_posnup(clnt, body.posn_rep.posn);
            break;
        default:
            dbr_err_setf(DBR_EIO, "unknown body-type '%d'", body.type);
            goto fail1;
        }

    } else if ((clnt->items[SUB_SOCK].revents & ZMQ_POLLIN)) {

        --nevents;
        // Clear event once consumed.
        clnt->items[SUB_SOCK].revents &= ~ZMQ_POLLIN;

        if (!dbr_recv_body(clnt->sub, clnt->pool, &body))
            goto fail1;

        switch ((event->type = body.type)) {
        case DBR_VIEW_LIST_REP:
            for (struct DbrSlNode* node = body.view_list_rep.first; node; ) {
                struct DbrView* view = dbr_shared_view_entry(node);
                node = node->next;
                // Transfer ownership.
                enrich_view(&clnt->cache, view);
                apply_viewup(clnt, view);
            }
            break;
        default:
            dbr_err_setf(DBR_EIO, "unknown body-type '%d'", body.type);
            goto fail1;
        }
    }
    return nevents;
 fail1:
    return -1;
}

DBR_API void
dbr_clnt_clear(DbrClnt clnt)
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
    dbr_tree_init(&clnt->viewups);
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
