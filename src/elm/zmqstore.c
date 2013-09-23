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
    struct DbrZmqStore_* zmqstore = journ_implof(journ);
    return zmqstore->id++;
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
insert_order(DbrJourn journ, struct DbrOrder* order)
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
insert_trade(DbrJourn journ, struct DbrTrade* trade)
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
    struct DbrZmqStore_* zmqstore = model_implof(model);
    struct DbrMsg msg = { .type = DBR_READ_ENTITY_REQ, .read_entity_req.type = type };

    if (!dbr_send_msg(zmqstore->sock, &msg))
        return -1;

    if (!dbr_recv_msg(zmqstore->sock, pool, &msg))
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
    struct DbrZmqStore_* zmqstore = malloc(sizeof(struct DbrZmqStore_));
    if (dbr_unlikely(!zmqstore))
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
    zmqstore->ctx = ctx;
    zmqstore->id = seed;
    zmqstore->sock = sock;

    zmqstore->journ_.vtbl = &JOURN_VTBL;
    zmqstore->model_.vtbl = &MODEL_VTBL;

    return zmqstore;
 fail3:
    zmq_close(sock);
 fail2:
    free(zmqstore);
 fail1:
    return NULL;
}

DBR_API void
dbr_zmqstore_destroy(DbrZmqStore zmqstore)
{
    if (zmqstore) {
        zmq_close(zmqstore->sock);
        free(zmqstore);
    }
}

DBR_API DbrJourn
dbr_zmqstore_journ(DbrZmqStore zmqstore)
{
    return &zmqstore->journ_;
}

DBR_API DbrModel
dbr_zmqstore_model(DbrZmqStore zmqstore)
{
    return &zmqstore->model_;
}
