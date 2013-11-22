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
#include "accnt.h"
#include "cache.h"
#include "trader.h"

#include <dbr/clnt.h>
#include <dbr/err.h>
#include <dbr/msg.h>
#include <dbr/queue.h>

#include <zmq.h>

#include <stdbool.h>
#include <stdlib.h> // malloc()
#include <string.h> // strncpy()

struct FigClnt {
    void* ctx;
    void* sock;
    // See set_trader().
    DbrTrader trader;
    DbrIden id;
    DbrPool pool;
    struct FigCache cache;
    struct FigIndex index;
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
    return dbr_entity_rec_entry(node);
}

static inline struct DbrOrder*
enrich_order(struct FigCache* cache, struct DbrOrder* order)
{
    order->trader.rec = get_id(cache, DBR_TRADER, order->trader.id_only);
    order->accnt.rec = get_id(cache, DBR_ACCNT, order->accnt.id_only);
    order->contr.rec = get_id(cache, DBR_CONTR, order->contr.id_only);
    return order;
}

static inline struct DbrExec*
enrich_exec(struct FigCache* cache, struct DbrExec* exec)
{
    exec->trader.rec = get_id(cache, DBR_TRADER, exec->trader.id_only);
    exec->accnt.rec = get_id(cache, DBR_ACCNT, exec->accnt.id_only);
    exec->contr.rec = get_id(cache, DBR_CONTR, exec->contr.id_only);
    exec->cpty.rec = get_id(cache, DBR_ACCNT, exec->cpty.id_only);
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

static ssize_t
read_entity(DbrClnt clnt, int type, struct DbrSlNode** first)
{
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_SESS_ENTITY_REQ;
    body.sess_entity_req.type = type;

    if (!dbr_send_body(clnt->sock, &body, false))
        return -1;

    if (!dbr_recv_body(clnt->sock, clnt->pool, &body))
        return -1;

    *first = body.entity_rep.first;
    return body.entity_rep.count_;
}

static DbrBool
emplace_recs(DbrClnt clnt, int type)
{
    struct DbrSlNode* node;
    ssize_t size = read_entity(clnt, type, &node);
    if (size < 0)
        return false;

    fig_cache_emplace_recs(&clnt->cache, type, node, size);
    return true;
}

static DbrBool
set_trader(DbrClnt clnt, const char* mnem)
{
    struct DbrSlNode* node = fig_cache_find_rec_mnem(&clnt->cache, DBR_TRADER, mnem);
    if (node == FIG_CACHE_END_REC)
        goto fail1;

    struct DbrRec* trec = dbr_entity_rec_entry(node);
    DbrTrader trader = fig_trader_lazy(trec, &clnt->index, clnt->pool);
    if (!trader)
        goto fail1;

    clnt->trader = trader;

    // Force instantiation of all accounts now.
    for (struct DbrRbNode* node = dbr_trader_first_memb(trader);
         node != DBR_TRADER_END_MEMB; node = dbr_rbnode_next(node)) {
        struct DbrMemb* memb = dbr_trader_memb_entry(node);
        DbrAccnt accnt = fig_accnt_lazy(memb->accnt.rec, clnt->pool);
        if (!accnt)
            goto fail1;
    }
    return true;
 fail1:
    return false;
}

static DbrBool
emplace_orders(DbrClnt clnt)
{
    struct DbrSlNode* node;
    if (read_entity(clnt, DBR_ORDER, &node) < 0)
        return false;

    for (; node; node = node->next) {
        struct DbrOrder* order = enrich_order(&clnt->cache, dbr_entity_order_entry(node));
        // Transfer ownership.
        fig_trader_emplace_order(clnt->trader, order);
    }
    return true;
}

static DbrBool
emplace_trades(DbrClnt clnt)
{
    struct DbrSlNode* node;
    if (read_entity(clnt, DBR_TRADE, &node) < 0)
        return false;

    for (; node; node = node->next) {
        struct DbrExec* exec = enrich_exec(&clnt->cache, dbr_entity_exec_entry(node));
        // Transfer ownership.
        fig_trader_emplace_trade(clnt->trader, exec);
    }
    return true;
}

static DbrBool
emplace_membs(DbrClnt clnt)
{
    struct DbrSlNode* node;
    if (read_entity(clnt, DBR_MEMB, &node) < 0)
        return false;

    for (; node; node = node->next) {
        struct DbrMemb* memb = enrich_memb(&clnt->cache, dbr_entity_memb_entry(node));
        // Transfer ownership.
        fig_trader_emplace_memb(clnt->trader, memb);
    }
    return true;
}

static DbrBool
emplace_posns(DbrClnt clnt)
{
    struct DbrSlNode* node;
    if (read_entity(clnt, DBR_POSN, &node) < 0)
        return false;

    for (; node; node = node->next) {
        struct DbrPosn* posn = enrich_posn(&clnt->cache, dbr_entity_posn_entry(node));
        // Transfer ownership.
        // All accnts that trader is member of are created in set_trader().
        fig_accnt_emplace_posn(posn->accnt.rec->accnt.state, posn);
    }
    return true;
}

DBR_API DbrClnt
dbr_clnt_create(void* ctx, const char* addr, const char* trader, DbrIden seed, DbrPool pool)
{
    DbrClnt clnt = malloc(sizeof(struct FigClnt));
    if (dbr_unlikely(!clnt)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    void* sock = zmq_socket(ctx, ZMQ_DEALER);
    if (!sock) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }
    zmq_setsockopt(sock, ZMQ_IDENTITY, trader, strnlen(trader, DBR_MNEM_MAX));

    if (zmq_connect(sock, addr) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }

    clnt->sock = sock;
    clnt->trader = NULL;
    clnt->id = seed;
    clnt->pool = pool;
    fig_cache_init(&clnt->cache, term_state, pool);
    fig_index_init(&clnt->index);

    // Data structures are fully initialised at this point.

    if (!emplace_recs(clnt, DBR_TRADER)
        || !emplace_recs(clnt, DBR_ACCNT)
        || !emplace_recs(clnt, DBR_CONTR)
        || !set_trader(clnt, trader)
        || !emplace_orders(clnt)
        || !emplace_trades(clnt)
        || !emplace_membs(clnt)
        || !emplace_posns(clnt)) {
        // Use destroy since fully initialised.
        dbr_clnt_destroy(clnt);
        goto fail1;
    }
    return clnt;
 fail3:
    zmq_close(sock);
 fail2:
    free(clnt);
 fail1:
    return NULL;
}

DBR_API void
dbr_clnt_destroy(DbrClnt clnt)
{
    if (clnt) {
        fig_cache_term(&clnt->cache);
        zmq_close(clnt->sock);
        free(clnt);
    }
}

DBR_API struct DbrSlNode*
dbr_clnt_first_rec(DbrClnt clnt, int type, size_t* size)
{
    return fig_cache_first_rec(&clnt->cache, type, size);
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

DBR_API struct DbrOrder*
dbr_clnt_place(DbrClnt clnt, const char* accnt, const char* contr, DbrDate settl_date,
               const char* ref, int action, DbrTicks ticks, DbrLots lots, DbrLots min,
               DbrFlags flags)
{
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_PLACE_ORDER_REQ;
    strncpy(body.place_order_req.accnt, accnt, DBR_MNEM_MAX);
    strncpy(body.place_order_req.contr, contr, DBR_MNEM_MAX);
    body.place_order_req.settl_date = settl_date;
    strncpy(body.place_order_req.ref, ref, DBR_REF_MAX);
    body.place_order_req.action = action;
    body.place_order_req.ticks = ticks;
    body.place_order_req.lots = lots;
    body.place_order_req.min = min;
    body.place_order_req.flags = flags;

    if (!dbr_send_body(clnt->sock, &body, false))
        goto fail1;

    if (!dbr_recv_body(clnt->sock, clnt->pool, &body))
        goto fail1;

    if (body.type == DBR_STATUS_REP) {
        dbr_err_set(body.status_rep.num, body.status_rep.msg);
        goto fail1;
    }

    assert(body.type == DBR_CYCLE_REP);

    fig_trader_emplace_order(clnt->trader, enrich_order(&clnt->cache, body.cycle_rep.new_order));

    // Posn list is transformed to include existing positions with updates.
    struct DbrQueue q = DBR_QUEUE_INIT(q);
    for (struct DbrSlNode* node = body.cycle_rep.first_posn; node; ) {
        struct DbrPosn* posn = enrich_posn(&clnt->cache, dbr_entity_posn_entry(node));
        node = node->next;
        // Transfer ownership or free if update.
        posn = fig_accnt_update_posn(posn->accnt.rec->accnt.state, posn);
        dbr_queue_insert_back(&q, &posn->entity_node_);
    }
    body.cycle_rep.first_posn = q.first;

    for (struct DbrSlNode* node = body.cycle_rep.first_exec; node; node = node->next) {
        struct DbrExec* exec = enrich_exec(&clnt->cache, dbr_entity_exec_entry(node));
        // Transfer ownership.
        fig_trader_emplace_trade(clnt->trader, exec);
    }

    return body.cycle_rep.new_order;
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_clnt_revise_id(DbrClnt clnt, DbrIden id, DbrLots lots)
{
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_REVISE_ORDER_ID_REQ;
    body.revise_order_id_req.id = id;
    body.revise_order_id_req.lots = lots;

    if (!dbr_send_body(clnt->sock, &body, false))
        goto fail1;

    if (!dbr_recv_body(clnt->sock, clnt->pool, &body))
        goto fail1;

    if (body.type == DBR_STATUS_REP) {
        dbr_err_set(body.status_rep.num, body.status_rep.msg);
        goto fail1;
    }

    assert(body.type == DBR_ORDER_REP);
    return fig_trader_update_order(clnt->trader, body.order_rep.order);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_clnt_revise_ref(DbrClnt clnt, const char* ref, DbrLots lots)
{
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_REVISE_ORDER_REF_REQ;
    strncpy(body.revise_order_ref_req.ref, ref, DBR_REF_MAX);
    body.revise_order_ref_req.lots = lots;

    if (!dbr_send_body(clnt->sock, &body, false))
        goto fail1;

    if (!dbr_recv_body(clnt->sock, clnt->pool, &body))
        goto fail1;

    if (body.type == DBR_STATUS_REP) {
        dbr_err_set(body.status_rep.num, body.status_rep.msg);
        goto fail1;
    }

    assert(body.type == DBR_ORDER_REP);
    return fig_trader_update_order(clnt->trader, body.order_rep.order);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_clnt_cancel_id(DbrClnt clnt, DbrIden id)
{
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_CANCEL_ORDER_ID_REQ;
    body.cancel_order_id_req.id = id;

    if (!dbr_send_body(clnt->sock, &body, false))
        goto fail1;

    if (!dbr_recv_body(clnt->sock, clnt->pool, &body))
        goto fail1;

    if (body.type == DBR_STATUS_REP) {
        dbr_err_set(body.status_rep.num, body.status_rep.msg);
        goto fail1;
    }

    assert(body.type == DBR_ORDER_REP);
    return fig_trader_update_order(clnt->trader, body.order_rep.order);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_clnt_cancel_ref(DbrClnt clnt, const char* ref)
{
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_CANCEL_ORDER_REF_REQ;
    strncpy(body.cancel_order_ref_req.ref, ref, DBR_REF_MAX);

    if (!dbr_send_body(clnt->sock, &body, false))
        goto fail1;

    if (!dbr_recv_body(clnt->sock, clnt->pool, &body))
        goto fail1;

    if (body.type == DBR_STATUS_REP) {
        dbr_err_set(body.status_rep.num, body.status_rep.msg);
        goto fail1;
    }

    assert(body.type == DBR_ORDER_REP);
    return fig_trader_update_order(clnt->trader, body.order_rep.order);
 fail1:
    return NULL;
}

DBR_API DbrBool
dbr_clnt_archive_order(DbrClnt clnt, DbrIden id)
{
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_ARCHIVE_ORDER_REQ;
    body.archive_order_req.id = id;

    if (!dbr_send_body(clnt->sock, &body, false))
        goto fail1;

    if (!dbr_recv_body(clnt->sock, clnt->pool, &body))
        goto fail1;

    assert(body.type == DBR_STATUS_REP);

    if (body.status_rep.num != 0) {
        dbr_err_set(body.status_rep.num, body.status_rep.msg);
        goto fail1;
    }
    return true;
 fail1:
    return false;
}

DBR_API DbrBool
dbr_clnt_archive_trade(DbrClnt clnt, DbrIden id)
{
    struct DbrBody body;
    body.req_id = clnt->id++;
    body.type = DBR_ARCHIVE_TRADE_REQ;
    body.archive_trade_req.id = id;

    if (!dbr_send_body(clnt->sock, &body, false))
        goto fail1;

    if (!dbr_recv_body(clnt->sock, clnt->pool, &body))
        goto fail1;

    assert(body.type == DBR_STATUS_REP);

    if (body.status_rep.num != 0) {
        dbr_err_set(body.status_rep.num, body.status_rep.msg);
        goto fail1;
    }
    return true;
 fail1:
    return false;
}
