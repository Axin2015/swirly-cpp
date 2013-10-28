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
#include <dbr/model.h>
#include <dbr/msg.h>

#include <zmq.h>

#include <stdbool.h>
#include <stdlib.h> // malloc()
#include <string.h> // strncpy()

struct FigClnt {
    void* ctx;
    void* sock;
    union {
        const char* mnem;
        DbrTrader state;
    } trader;
    union {
        const char* mnem;
        DbrAccnt state;
    } accnt;
    DbrPool pool;
    struct FigCache cache;
    struct FigIndex index;
    struct DbrIModel model_;
};

static inline struct FigClnt*
model_implof(DbrModel model)
{
    return dbr_implof(struct FigClnt, model_, model);
}

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
get_id(DbrClnt clnt, int type, DbrIden id)
{
    struct DbrSlNode* node = fig_cache_find_rec_id(&clnt->cache, type, id);
    assert(node != FIG_CACHE_END_REC);
    return dbr_rec_entry(node);
}

static ssize_t
read_entity(DbrModel model, int type, DbrPool pool, struct DbrSlNode** first)
{
    struct FigClnt* clnt = model_implof(model);
    assert(clnt->pool == pool);

    struct DbrMsg msg;
    switch (type) {
    case DBR_TRADER:
    case DBR_ACCNT:
    case DBR_CONTR:
        msg.type = DBR_READ_REC_REQ;
        msg.read_entity_req.type = type;
       break;
    case DBR_ORDER:
        msg.type = DBR_READ_TRADER_ORDER_REQ;
        strncpy(msg.read_trader_order_req.trader, clnt->trader.state->rec->mnem, DBR_MNEM_MAX);
        break;
    case DBR_TRADE:
        msg.type = DBR_READ_TRADER_TRADE_REQ;
        strncpy(msg.read_trader_trade_req.trader, clnt->trader.state->rec->mnem, DBR_MNEM_MAX);
        break;
    case DBR_MEMB:
        msg.type = DBR_READ_TRADER_MEMB_REQ;
        strncpy(msg.read_trader_memb_req.trader, clnt->trader.state->rec->mnem, DBR_MNEM_MAX);
        break;
    case DBR_POSN:
        msg.type = DBR_READ_ACCNT_POSN_REQ;
        strncpy(msg.read_accnt_posn_req.accnt, clnt->accnt.state->rec->mnem, DBR_MNEM_MAX);
        break;
    }
    if (!dbr_send_msg(clnt->sock, &msg, false))
        return -1;

    if (!dbr_recv_msg(clnt->sock, pool, &msg))
        return -1;

    *first = msg.entity_rep.first;
    return msg.entity_rep.count_;
}

static const struct DbrModelVtbl MODEL_VTBL = {
    .read_entity = read_entity
};

static DbrBool
emplace_recs(DbrClnt clnt, int type)
{
    struct DbrSlNode* node;
    ssize_t size = dbr_model_read_entity(&clnt->model_, type, clnt->pool, &node);
    if (size < 0)
        return false;

    fig_cache_emplace_recs(&clnt->cache, type, node, size);
    return true;
}

static DbrBool
emplace_orders(DbrClnt clnt)
{
    struct DbrSlNode* node = fig_cache_find_rec_mnem(&clnt->cache, DBR_TRADER, clnt->trader.mnem);
    if (node == FIG_CACHE_END_REC)
        goto fail1;

    struct DbrRec* trec = dbr_rec_entry(node);
    DbrTrader state = fig_trader_lazy(trec, &clnt->index, clnt->pool);
    if (!state)
        goto fail1;

    clnt->trader.state = state;

    if (dbr_model_read_entity(&clnt->model_, DBR_ORDER, clnt->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrOrder* order = dbr_order_entry(node);

        // Enrich.
        order->trader.rec = get_id(clnt, DBR_TRADER, order->trader.id_only);
        order->accnt.rec = get_id(clnt, DBR_ACCNT, order->accnt.id_only);
        order->contr.rec = get_id(clnt, DBR_CONTR, order->contr.id_only);

        // Transfer ownership.
        fig_trader_emplace_order(clnt->trader.state, order);
    }
    return true;
 fail1:
    return false;
}

static DbrBool
emplace_trades(DbrClnt clnt)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(&clnt->model_, DBR_TRADE, clnt->pool, &node) < 0)
        return false;

    for (; node; node = node->next) {
        struct DbrTrade* trade = dbr_trade_entry(node);

        // Enrich.
        trade->trader.rec = get_id(clnt, DBR_TRADER, trade->trader.id_only);
        trade->accnt.rec = get_id(clnt, DBR_ACCNT, trade->accnt.id_only);
        trade->contr.rec = get_id(clnt, DBR_CONTR, trade->contr.id_only);
        trade->cpty.rec = get_id(clnt, DBR_ACCNT, trade->cpty.id_only);

        // Transfer ownership.
        fig_trader_emplace_trade(clnt->trader.state, trade);
    }
    return true;
}

static DbrBool
emplace_membs(DbrClnt clnt)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(&clnt->model_, DBR_MEMB, clnt->pool, &node) < 0)
        return false;

    for (; node; node = node->next) {
        struct DbrMemb* memb = dbr_memb_entry(node);

        // Enrich.
        memb->trader.rec = get_id(clnt, DBR_TRADER, memb->trader.id_only);
        memb->accnt.rec = get_id(clnt, DBR_ACCNT, memb->accnt.id_only);

        // Transfer ownership.
        fig_trader_emplace_memb(clnt->trader.state, memb);
    }
    return true;
}

static DbrBool
emplace_posns(DbrClnt clnt)
{
    struct DbrSlNode* node = fig_cache_find_rec_mnem(&clnt->cache, DBR_ACCNT, clnt->accnt.mnem);
    if (node == FIG_CACHE_END_REC)
        goto fail1;

    struct DbrRec* arec = dbr_rec_entry(node);
    DbrAccnt state = fig_accnt_lazy(arec, clnt->pool);
    if (!state)
        goto fail1;

    clnt->accnt.state = state;

    if (dbr_model_read_entity(&clnt->model_, DBR_POSN, clnt->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrPosn* posn = dbr_posn_entry(node);

        // Enrich.
        posn->accnt.rec = get_id(clnt, DBR_ACCNT, posn->accnt.id_only);
        posn->contr.rec = get_id(clnt, DBR_CONTR, posn->contr.id_only);

        // Transfer ownership.
        fig_accnt_emplace_posn(clnt->accnt.state, posn);
    }
    return true;
 fail1:
    return false;
}

DBR_API DbrClnt
dbr_clnt_create(void* ctx, const char* addr, const char* trader, const char* accnt, DbrPool pool)
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
    clnt->trader.mnem = trader;
    clnt->accnt.mnem = accnt;
    clnt->pool = pool;
    fig_cache_init(&clnt->cache, term_state, pool);
    fig_index_init(&clnt->index);
    clnt->model_.vtbl = &MODEL_VTBL;

    // Data structures are fully initialised at this point.

    if (!emplace_recs(clnt, DBR_TRADER)
        || !emplace_recs(clnt, DBR_ACCNT)
        || !emplace_recs(clnt, DBR_CONTR)
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
    return clnt->trader.state;
}

DBR_API DbrAccnt
dbr_clnt_accnt(DbrClnt clnt)
{
    return clnt->accnt.state;
}

DBR_API struct DbrOrder*
dbr_clnt_place(DbrClnt clnt, const char* contr, DbrDate settl_date, const char* ref, int action,
               DbrTicks ticks, DbrLots lots, DbrLots min, DbrFlags flags, struct DbrResult* result)
{
    struct DbrMsg msg;
    msg.type = DBR_PLACE_ORDER_REQ;
    strncpy(msg.place_order_req.trader, clnt->trader.state->rec->mnem, DBR_MNEM_MAX);
    strncpy(msg.place_order_req.accnt, clnt->accnt.state->rec->mnem, DBR_MNEM_MAX);
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

    return result->new_order;
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_clnt_revise_id(DbrClnt clnt, DbrIden id, DbrLots lots)
{
    struct DbrMsg msg;
    msg.type = DBR_REVISE_ORDER_ID_REQ;
    strncpy(msg.revise_order_id_req.trader, clnt->trader.state->rec->mnem, DBR_MNEM_MAX);
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

    return msg.order_rep.order;
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_clnt_revise_ref(DbrClnt clnt, const char* ref, DbrLots lots)
{
    struct DbrMsg msg;
    msg.type = DBR_REVISE_ORDER_REF_REQ;
    strncpy(msg.revise_order_ref_req.trader, clnt->trader.state->rec->mnem, DBR_MNEM_MAX);
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

    return msg.order_rep.order;
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_clnt_cancel_id(DbrClnt clnt, DbrIden id)
{
    struct DbrMsg msg;
    msg.type = DBR_CANCEL_ORDER_ID_REQ;
    strncpy(msg.cancel_order_id_req.trader, clnt->trader.state->rec->mnem, DBR_MNEM_MAX);
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

    return msg.order_rep.order;
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_clnt_cancel_ref(DbrClnt clnt, const char* ref)
{
    struct DbrMsg msg;
    msg.type = DBR_CANCEL_ORDER_REF_REQ;
    strncpy(msg.cancel_order_ref_req.trader, clnt->trader.state->rec->mnem, DBR_MNEM_MAX);
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

    return msg.order_rep.order;
 fail1:
    return NULL;
}

DBR_API DbrBool
dbr_clnt_archive_order(DbrClnt clnt, DbrIden id)
{
    struct DbrMsg msg;
    msg.type = DBR_ARCHIVE_ORDER_REQ;
    strncpy(msg.archive_order_req.trader, clnt->trader.state->rec->mnem, DBR_MNEM_MAX);
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
    strncpy(msg.archive_trade_req.trader, clnt->trader.state->rec->mnem, DBR_MNEM_MAX);
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
