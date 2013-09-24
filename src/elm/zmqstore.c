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

#include <zmq.h>

#include <stdbool.h>
#include <stdlib.h>

struct DbrZmqStore_ {
    void* ctx;
    DbrIden id;
    void* sock;
    struct DbrIJourn journ_;
    struct DbrIModel model_;
};

static inline struct DbrZmqStore_*
journ_implof(DbrJourn journ)
{
    return dbr_implof(struct DbrZmqStore_, journ_, journ);
}

static inline struct DbrZmqStore_*
model_implof(DbrModel model)
{
    return dbr_implof(struct DbrZmqStore_, model_, model);
}

static DbrIden
alloc_id(DbrJourn journ)
{
    struct DbrZmqStore_* store = journ_implof(journ);
    return store->id++;
}

static DbrBool
begin_trans(DbrJourn journ)
{
    return true;
}

static DbrBool
commit_trans(DbrJourn journ)
{
    return true;
}

static DbrBool
rollback_trans(DbrJourn journ)
{
    return true;
}

static DbrBool
insert_order(DbrJourn journ, DbrIden id, int rev, int status, DbrIden tid, DbrIden aid,
             DbrIden cid, DbrDate settl_date, const char* ref, int action, DbrTicks ticks,
             DbrLots resd, DbrLots exec, DbrLots lots, DbrLots min, DbrFlags flags,
             DbrMillis created, DbrMillis modified)
{
    return true;
}

static DbrBool
update_order(DbrJourn journ, DbrIden id, int rev, int status, DbrLots resd, DbrLots exec,
             DbrLots lots, DbrMillis now)
{
    return true;
}

static DbrBool
archive_order(DbrJourn journ, DbrIden id, DbrMillis now)
{
    return true;
}

static DbrBool
insert_trade(DbrJourn journ, DbrIden id, DbrIden match, DbrIden order, int order_rev,
             DbrIden tid, DbrIden aid, DbrIden cid, DbrDate settl_date, const char* ref,
             DbrIden cpty, int role, int action, DbrTicks ticks, DbrLots resd,
             DbrLots exec, DbrLots lots, DbrMillis created, DbrMillis modified)
{
    return true;
}

static DbrBool
archive_trade(DbrJourn journ, DbrIden id, DbrMillis now)
{
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
    struct DbrZmqStore_* store = model_implof(model);
    struct DbrMsg msg = { .type = DBR_READ_ENTITY_REQ, .read_entity_req.type = type };

    if (!dbr_send_msg(store->sock, &msg))
        return -1;

    if (!dbr_recv_msg(store->sock, pool, &msg))
        return -1;

    *first = msg.read_entity_rep.first;
    return msg.read_entity_rep.count;
}

static const struct DbrModelVtbl MODEL_VTBL = {
    .read_entity = read_entity
};

DBR_API DbrZmqStore
dbr_zmqstore_create(void* ctx, DbrIden seed, const char* addr)
{
    struct DbrZmqStore_* store = malloc(sizeof(struct DbrZmqStore_));
    if (dbr_unlikely(!store))
        goto fail1;

    void* sock = zmq_socket(ctx, ZMQ_REQ);
    if (!sock) {
        dbr_err_set(DBR_EIO, "zmq_socket() failed: %s", zmq_strerror(zmq_errno()));
        goto fail2;
    }

    if (zmq_bind(sock, addr) < 0) {
        dbr_err_set(DBR_EIO, "zmq_bind() failed: %s", zmq_strerror(zmq_errno()));
        goto fail3;
    }

    // Seed identity.
    store->ctx = ctx;
    store->id = seed;
    store->sock = sock;

    store->journ_.vtbl = &JOURN_VTBL;
    store->model_.vtbl = &MODEL_VTBL;

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
        zmq_close(store->sock);
        free(store);
    }
}

DBR_API DbrJourn
dbr_zmqstore_journ(DbrZmqStore store)
{
    return &store->journ_;
}

DBR_API DbrModel
dbr_zmqstore_model(DbrZmqStore store)
{
    return &store->model_;
}
