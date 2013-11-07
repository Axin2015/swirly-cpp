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
#include <dbr/zmqstore.h>

#include <dbr/err.h>
#include <dbr/journ.h>
#include <dbr/model.h>
#include <dbr/msg.h>
#include <dbr/queue.h>

#include <zmq.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h> // strncpy()

struct ElmZmqStore {
    void* sock;
    DbrIden id;
    DbrPool pool;
    struct DbrQueue queue;
    struct DbrIJourn journ;
    struct DbrIModel model;
};

static void
free_stmts(struct DbrSlNode* first, DbrPool pool)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrStmt* stmt = dbr_trans_stmt_entry(node);
        node = node->next;
        dbr_pool_free_stmt(pool, stmt);
    }
}

static inline struct ElmZmqStore*
journ_implof(DbrJourn journ)
{
    return dbr_implof(struct ElmZmqStore, journ, journ);
}

static inline struct ElmZmqStore*
model_implof(DbrModel model)
{
    return dbr_implof(struct ElmZmqStore, model, model);
}

static DbrIden
alloc_id(DbrJourn journ)
{
    struct ElmZmqStore* store = journ_implof(journ);
    return store->id++;
}

static DbrBool
begin_trans(DbrJourn journ)
{
    struct ElmZmqStore* store = journ_implof(journ);
    if (!dbr_queue_empty(&store->queue)) {
        dbr_err_set(DBR_EIO, "incomplete transaction");
        return false;
    }
    return true;
}

static DbrBool
commit_trans(DbrJourn journ)
{
    struct ElmZmqStore* store = journ_implof(journ);
    struct DbrMsg msg = { .req_id = store->id++, .type = DBR_WRITE_TRANS_REQ,
                          .write_trans_req = { .count_ = 0, .first = store->queue.first } };
    DbrBool ok = dbr_send_msg(store->sock, &msg, false);
    free_stmts(store->queue.first, store->pool);
    dbr_queue_init(&store->queue);
    if (!ok)
        return false;
    // FIXME: validate return type.
    return dbr_recv_msg(store->sock, store->pool, &msg);
}

static DbrBool
rollback_trans(DbrJourn journ)
{
    struct ElmZmqStore* store = journ_implof(journ);
    free_stmts(store->queue.first, store->pool);
    dbr_queue_init(&store->queue);
    return true;
}

static DbrBool
insert_order(DbrJourn journ, DbrIden id, int rev, int status, DbrIden tid, DbrIden aid,
             DbrIden cid, DbrDate settl_date, const char* ref, int action, DbrTicks ticks,
             DbrLots resd, DbrLots exec, DbrLots lots, DbrLots min, DbrFlags flags,
             DbrMillis now)
{
    struct ElmZmqStore* store = journ_implof(journ);
    struct DbrStmt* stmt = dbr_pool_alloc_stmt(store->pool);
    if (!stmt)
        return false;
    stmt->type = DBR_INSERT_ORDER;
    stmt->insert_order.id = id;
    stmt->insert_order.rev = rev;
    stmt->insert_order.status = status;
    stmt->insert_order.tid = tid;
    stmt->insert_order.aid = aid;
    stmt->insert_order.cid = cid;
    stmt->insert_order.settl_date = settl_date;
    strncpy(stmt->insert_order.ref, ref, DBR_REF_MAX);
    stmt->insert_order.action = action;
    stmt->insert_order.ticks = ticks;
    stmt->insert_order.resd = resd;
    stmt->insert_order.exec = exec;
    stmt->insert_order.lots = lots;
    stmt->insert_order.min = min;
    stmt->insert_order.flags = flags;
    stmt->insert_order.now = now;
    dbr_queue_insert_back(&store->queue, &stmt->trans_node_);
    return true;
}

static DbrBool
update_order(DbrJourn journ, DbrIden id, int rev, int status, DbrLots resd, DbrLots exec,
             DbrLots lots, DbrMillis now)
{
    struct ElmZmqStore* store = journ_implof(journ);
    struct DbrStmt* stmt = dbr_pool_alloc_stmt(store->pool);
    if (!stmt)
        return false;
    stmt->type = DBR_UPDATE_ORDER;
    stmt->update_order.id = id;
    stmt->update_order.rev = rev;
    stmt->update_order.status = status;
    stmt->update_order.resd = resd;
    stmt->update_order.exec = exec;
    stmt->update_order.lots = lots;
    stmt->update_order.now = now;
    dbr_queue_insert_back(&store->queue, &stmt->trans_node_);
    return true;
}

static DbrBool
archive_order(DbrJourn journ, DbrIden id, DbrMillis now)
{
    struct ElmZmqStore* store = journ_implof(journ);
    struct DbrStmt* stmt = dbr_pool_alloc_stmt(store->pool);
    if (!stmt)
        return false;
    stmt->type = DBR_ARCHIVE_ORDER;
    stmt->archive_order.id = id;
    stmt->archive_order.now = now;
    dbr_queue_insert_back(&store->queue, &stmt->trans_node_);
    return true;
}

static DbrBool
insert_trade(DbrJourn journ, DbrIden id, DbrIden order, int rev, DbrIden tid, DbrIden aid,
             DbrIden cid, DbrDate settl_date, const char* ref, int action, DbrTicks ticks,
             DbrLots resd, DbrLots exec, DbrLots lots, DbrIden match, DbrIden cpty, int role,
             DbrMillis now)
{
    struct ElmZmqStore* store = journ_implof(journ);
    struct DbrStmt* stmt = dbr_pool_alloc_stmt(store->pool);
    if (!stmt)
        return false;
    stmt->type = DBR_INSERT_TRADE;
    stmt->insert_trade.id = id;
    stmt->insert_trade.order = order;
    stmt->insert_trade.rev = rev;
    stmt->insert_trade.tid = tid;
    stmt->insert_trade.aid = aid;
    stmt->insert_trade.cid = cid;
    stmt->insert_trade.settl_date = settl_date;
    strncpy(stmt->insert_trade.ref, ref, DBR_REF_MAX);
    stmt->insert_trade.action = action;
    stmt->insert_trade.ticks = ticks;
    stmt->insert_trade.resd = resd;
    stmt->insert_trade.exec = exec;
    stmt->insert_trade.lots = lots;
    stmt->insert_trade.match = match;
    stmt->insert_trade.cpty = cpty;
    stmt->insert_trade.role = role;
    stmt->insert_trade.now = now;
    dbr_queue_insert_back(&store->queue, &stmt->trans_node_);
    return true;
}

static DbrBool
archive_trade(DbrJourn journ, DbrIden id, DbrMillis now)
{
    struct ElmZmqStore* store = journ_implof(journ);
    struct DbrStmt* stmt = dbr_pool_alloc_stmt(store->pool);
    if (!stmt)
        return false;
    stmt->type = DBR_ARCHIVE_TRADE;
    stmt->archive_trade.id = id;
    stmt->archive_trade.now = now;
    dbr_queue_insert_back(&store->queue, &stmt->trans_node_);
    return true;
}

static const struct DbrJournVtbl JOURN_VTBL = {
    .alloc_id = alloc_id,
    .begin_trans = begin_trans,
    .commit_trans = commit_trans,
    .rollback_trans = rollback_trans,
    .insert_order = insert_order,
    .update_order = update_order,
    .archive_order = archive_order,
    .insert_trade = insert_trade,
    .archive_trade = archive_trade
};

static ssize_t
read_entity(DbrModel model, int type, DbrPool pool, struct DbrSlNode** first)
{
    struct ElmZmqStore* store = model_implof(model);
    struct DbrMsg msg = { .req_id = store->id++, .type = DBR_READ_ENTITY_REQ,
                          .read_entity_req.type = type };

    if (!dbr_send_msg(store->sock, &msg, false))
        return -1;

    if (!dbr_recv_msg(store->sock, pool, &msg))
        return -1;

    *first = msg.entity_rep.first;
    return msg.entity_rep.count_;
}

static const struct DbrModelVtbl MODEL_VTBL = {
    .read_entity = read_entity
};

DBR_API DbrZmqStore
dbr_zmqstore_create(void* ctx, const char* addr, DbrIden seed, DbrPool pool)
{
    struct ElmZmqStore* store = malloc(sizeof(struct ElmZmqStore));
    if (dbr_unlikely(!store)) {
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

    // Seed identity.
    store->sock = sock;
    store->id = seed;
    store->pool = pool;
    dbr_queue_init(&store->queue);

    store->journ.vtbl = &JOURN_VTBL;
    store->model.vtbl = &MODEL_VTBL;

    return store;
 fail3:
    zmq_close(sock);
 fail2:
    free(store);
 fail1:
    return NULL;
}

DBR_API void
dbr_zmqstore_destroy(DbrZmqStore store)
{
    if (store) {
        free_stmts(store->queue.first, store->pool);
        dbr_queue_init(&store->queue);
        zmq_close(store->sock);
        free(store);
    }
}

DBR_API DbrJourn
dbr_zmqstore_journ(DbrZmqStore store)
{
    return &store->journ;
}

DBR_API DbrModel
dbr_zmqstore_model(DbrZmqStore store)
{
    return &store->model;
}
