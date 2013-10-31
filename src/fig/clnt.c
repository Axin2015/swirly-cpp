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
    return dbr_rec_entry(node);
}

static inline struct DbrOrder*
enrich_order(struct FigCache* cache, struct DbrOrder* order)
{
    order->trader.rec = get_id(cache, DBR_TRADER, order->trader.id_only);
    order->accnt.rec = get_id(cache, DBR_ACCNT, order->accnt.id_only);
    order->contr.rec = get_id(cache, DBR_CONTR, order->contr.id_only);
    return order;
}

static inline struct DbrTrade*
enrich_trade(struct FigCache* cache, struct DbrTrade* trade)
{
    trade->trader.rec = get_id(cache, DBR_TRADER, trade->trader.id_only);
    trade->accnt.rec = get_id(cache, DBR_ACCNT, trade->accnt.id_only);
    trade->contr.rec = get_id(cache, DBR_CONTR, trade->contr.id_only);
    trade->cpty.rec = get_id(cache, DBR_ACCNT, trade->cpty.id_only);
    return trade;
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
read_entity(DbrClnt clnt, const char* mnem, int type, struct DbrSlNode** first)
{
    struct DbrMsg msg;
    msg.type = DBR_SESS_ENTITY_REQ;
    msg.sess_entity_req.type = type;
    strncpy(msg.sess_entity_req.trader, mnem, DBR_MNEM_MAX);

    if (!dbr_send_msg(clnt->sock, &msg, false))
        return -1;

    if (!dbr_recv_msg(clnt->sock, clnt->pool, &msg))
        return -1;

    *first = msg.entity_rep.first;
    return msg.entity_rep.count_;
}

static DbrBool
emplace_recs(DbrClnt clnt, const char* mnem, int type)
{
    struct DbrSlNode* node;
    ssize_t size = read_entity(clnt, mnem, type, &node);
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

    struct DbrRec* trec = dbr_rec_entry(node);
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
emplace_orders(DbrClnt clnt, const char* mnem)
{
    struct DbrSlNode* node;
    if (read_entity(clnt, mnem, DBR_ORDER, &node) < 0)
        return false;

    for (; node; node = node->next) {
        struct DbrOrder* order = enrich_order(&clnt->cache, dbr_order_entry(node));
        // Transfer ownership.
        fig_trader_emplace_order(clnt->trader, order);
    }
    return true;
}

static DbrBool
emplace_trades(DbrClnt clnt, const char* mnem)
{
    struct DbrSlNode* node;
    if (read_entity(clnt, mnem, DBR_TRADE, &node) < 0)
        return false;

    for (; node; node = node->next) {
        struct DbrTrade* trade = enrich_trade(&clnt->cache, dbr_trade_entry(node));
        // Transfer ownership.
        fig_trader_emplace_trade(clnt->trader, trade);
    }
    return true;
}

static DbrBool
emplace_membs(DbrClnt clnt, const char* mnem)
{
    struct DbrSlNode* node;
    if (read_entity(clnt, mnem, DBR_MEMB, &node) < 0)
        return false;

    for (; node; node = node->next) {
        struct DbrMemb* memb = enrich_memb(&clnt->cache, dbr_memb_entry(node));
        // Transfer ownership.
        fig_trader_emplace_memb(clnt->trader, memb);
    }
    return true;
}

static DbrBool
emplace_posns(DbrClnt clnt, const char* mnem)
{
    struct DbrSlNode* node;
    if (read_entity(clnt, mnem, DBR_POSN, &node) < 0)
        return false;

    for (; node; node = node->next) {
        struct DbrPosn* posn = enrich_posn(&clnt->cache, dbr_posn_entry(node));
        // Transfer ownership.
        // All accnts that trader is member of are created in set_trader().
        fig_accnt_emplace_posn(posn->accnt.rec->accnt.state, posn);
    }
    return true;
}

DBR_API DbrClnt
dbr_clnt_create(void* ctx, const char* addr, const char* trader, DbrPool pool)
{
    DbrClnt clnt = malloc(sizeof(struct FigClnt));
    if (dbr_unlikely(!clnt)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    void* sock = zmq_socket(ctx, ZMQ_REQ);
    if (!sock) {
        dbr_err_setf(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }

    if (zmq_connect(sock, addr) < 0) {
        dbr_err_setf(DBR_EIO, "zmq_connect() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }

    clnt->sock = sock;
    clnt->trader = NULL;
    clnt->pool = pool;
    fig_cache_init(&clnt->cache, term_state, pool);
    fig_index_init(&clnt->index);

    // Data structures are fully initialised at this point.

    if (!emplace_recs(clnt, trader, DBR_TRADER)
        || !emplace_recs(clnt, trader, DBR_ACCNT)
        || !emplace_recs(clnt, trader, DBR_CONTR)
        || !set_trader(clnt, trader)
        || !emplace_orders(clnt, trader)
        || !emplace_trades(clnt, trader)
        || !emplace_membs(clnt, trader)
        || !emplace_posns(clnt, trader)) {
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
               DbrFlags flags, struct DbrResult* result)
{
    struct DbrMsg msg;
    msg.type = DBR_PLACE_ORDER_REQ;
    strncpy(msg.place_order_req.trader, clnt->trader->rec->mnem, DBR_MNEM_MAX);
    strncpy(msg.place_order_req.accnt, accnt, DBR_MNEM_MAX);
    strncpy(msg.place_order_req.contr, contr, DBR_MNEM_MAX);
    msg.place_order_req.settl_date = settl_date;
    strncpy(msg.place_order_req.ref, ref, DBR_REF_MAX);
    msg.place_order_req.action = action;
    msg.place_order_req.ticks = ticks;
    msg.place_order_req.lots = lots;
    msg.place_order_req.min = min;
    msg.place_order_req.flags = flags;

    if (!dbr_send_msg(clnt->sock, &msg, false))
        goto fail1;

    if (!dbr_recv_msg(clnt->sock, clnt->pool, &msg))
        goto fail1;

    if (msg.type == DBR_STATUS_REP) {
        dbr_err_set(msg.status_rep.num, msg.status_rep.msg);
        goto fail1;
    }

    assert(msg.type == DBR_RESULT_REP);

    fig_trader_emplace_order(clnt->trader, enrich_order(&clnt->cache, result->new_order));

    // Posn list is transformed to include existing positions with updates.
    struct DbrQueue q = DBR_QUEUE_INIT(q);
    for (struct DbrSlNode* node = result->first_posn; node; ) {
        struct DbrPosn* posn = enrich_posn(&clnt->cache, dbr_posn_entry(node));
        node = node->next;
        // Transfer ownership or free if update.
        posn = fig_accnt_update_posn(posn->accnt.rec->accnt.state, posn);
        dbr_queue_insert_back(&q, &posn->entity_node_);
    }
    result->first_posn = q.first;

    for (struct DbrSlNode* node = result->first_trade; node; node = node->next) {
        struct DbrTrade* trade = enrich_trade(&clnt->cache, dbr_trade_entry(node));
        // Transfer ownership.
        fig_trader_emplace_trade(clnt->trader, trade);
    }

    return result->new_order;
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_clnt_revise_id(DbrClnt clnt, DbrIden id, DbrLots lots)
{
    struct DbrMsg msg;
    msg.type = DBR_REVISE_ORDER_ID_REQ;
    strncpy(msg.revise_order_id_req.trader, clnt->trader->rec->mnem, DBR_MNEM_MAX);
    msg.revise_order_id_req.id = id;
    msg.revise_order_id_req.lots = lots;

    if (!dbr_send_msg(clnt->sock, &msg, false))
        goto fail1;

    if (!dbr_recv_msg(clnt->sock, clnt->pool, &msg))
        goto fail1;

    if (msg.type == DBR_STATUS_REP) {
        dbr_err_set(msg.status_rep.num, msg.status_rep.msg);
        goto fail1;
    }

    assert(msg.type == DBR_ORDER_REP);
    return fig_trader_update_order(clnt->trader, msg.order_rep.order);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_clnt_revise_ref(DbrClnt clnt, const char* ref, DbrLots lots)
{
    struct DbrMsg msg;
    msg.type = DBR_REVISE_ORDER_REF_REQ;
    strncpy(msg.revise_order_ref_req.trader, clnt->trader->rec->mnem, DBR_MNEM_MAX);
    strncpy(msg.revise_order_ref_req.ref, ref, DBR_REF_MAX);
    msg.revise_order_ref_req.lots = lots;

    if (!dbr_send_msg(clnt->sock, &msg, false))
        goto fail1;

    if (!dbr_recv_msg(clnt->sock, clnt->pool, &msg))
        goto fail1;

    if (msg.type == DBR_STATUS_REP) {
        dbr_err_set(msg.status_rep.num, msg.status_rep.msg);
        goto fail1;
    }

    assert(msg.type == DBR_ORDER_REP);
    return fig_trader_update_order(clnt->trader, msg.order_rep.order);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_clnt_cancel_id(DbrClnt clnt, DbrIden id)
{
    struct DbrMsg msg;
    msg.type = DBR_CANCEL_ORDER_ID_REQ;
    strncpy(msg.cancel_order_id_req.trader, clnt->trader->rec->mnem, DBR_MNEM_MAX);
    msg.cancel_order_id_req.id = id;

    if (!dbr_send_msg(clnt->sock, &msg, false))
        goto fail1;

    if (!dbr_recv_msg(clnt->sock, clnt->pool, &msg))
        goto fail1;

    if (msg.type == DBR_STATUS_REP) {
        dbr_err_set(msg.status_rep.num, msg.status_rep.msg);
        goto fail1;
    }

    assert(msg.type == DBR_ORDER_REP);
    return fig_trader_update_order(clnt->trader, msg.order_rep.order);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_clnt_cancel_ref(DbrClnt clnt, const char* ref)
{
    struct DbrMsg msg;
    msg.type = DBR_CANCEL_ORDER_REF_REQ;
    strncpy(msg.cancel_order_ref_req.trader, clnt->trader->rec->mnem, DBR_MNEM_MAX);
    strncpy(msg.cancel_order_ref_req.ref, ref, DBR_REF_MAX);

    if (!dbr_send_msg(clnt->sock, &msg, false))
        goto fail1;

    if (!dbr_recv_msg(clnt->sock, clnt->pool, &msg))
        goto fail1;

    if (msg.type == DBR_STATUS_REP) {
        dbr_err_set(msg.status_rep.num, msg.status_rep.msg);
        goto fail1;
    }

    assert(msg.type == DBR_ORDER_REP);
    return fig_trader_update_order(clnt->trader, msg.order_rep.order);
 fail1:
    return NULL;
}

DBR_API DbrBool
dbr_clnt_archive_order(DbrClnt clnt, DbrIden id)
{
    struct DbrMsg msg;
    msg.type = DBR_ARCHIVE_ORDER_REQ;
    strncpy(msg.archive_order_req.trader, clnt->trader->rec->mnem, DBR_MNEM_MAX);
    msg.archive_order_req.id = id;

    if (!dbr_send_msg(clnt->sock, &msg, false))
        goto fail1;

    if (!dbr_recv_msg(clnt->sock, clnt->pool, &msg))
        goto fail1;

    assert(msg.type == DBR_STATUS_REP);

    if (msg.status_rep.num != 0) {
        dbr_err_set(msg.status_rep.num, msg.status_rep.msg);
        goto fail1;
    }
    return true;
 fail1:
    return false;
}

DBR_API DbrBool
dbr_clnt_archive_trade(DbrClnt clnt, DbrIden id)
{
    struct DbrMsg msg;
    msg.type = DBR_ARCHIVE_TRADE_REQ;
    strncpy(msg.archive_trade_req.trader, clnt->trader->rec->mnem, DBR_MNEM_MAX);
    msg.archive_trade_req.id = id;

    if (!dbr_send_msg(clnt->sock, &msg, false))
        goto fail1;

    if (!dbr_recv_msg(clnt->sock, clnt->pool, &msg))
        goto fail1;

    assert(msg.type == DBR_STATUS_REP);

    if (msg.status_rep.num != 0) {
        dbr_err_set(msg.status_rep.num, msg.status_rep.msg);
        goto fail1;
    }
    return true;
 fail1:
    return false;
}
