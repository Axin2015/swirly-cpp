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

    ssize_t size = dbr_model_read_entity(&clnt->model_, DBR_ORDER, clnt->pool, &node);
    if (size < 0)
        goto fail1;

    return true;
 fail1:
    return false;
}

static DbrBool
emplace_trades(DbrClnt clnt)
{
    struct DbrSlNode* node;
    ssize_t size = dbr_model_read_entity(&clnt->model_, DBR_TRADE, clnt->pool, &node);
    if (size < 0)
        return false;
    return true;
}

static DbrBool
emplace_membs(DbrClnt clnt)
{
    struct DbrSlNode* node;
    ssize_t size = dbr_model_read_entity(&clnt->model_, DBR_MEMB, clnt->pool, &node);
    if (size < 0)
        return false;
    return true;
}

static DbrBool
emplace_posns(DbrClnt clnt)
{
    struct DbrSlNode* node;
    ssize_t size = dbr_model_read_entity(&clnt->model_, DBR_POSN, clnt->pool, &node);
    if (size < 0)
        return false;
    return true;
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
