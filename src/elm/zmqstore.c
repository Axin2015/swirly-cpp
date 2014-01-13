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
#include <dbr/zmqstore.h>

#include <dbr/err.h>
#include <dbr/msg.h>
#include <dbr/queue.h>

#include <zmq.h>

#include <stdlib.h>
#include <string.h> // strncpy()

struct ElmZmqStore {
    void* sock;
    DbrIden id;
    DbrPool pool;
    struct DbrIJourn journ;
    struct DbrIModel model;
};

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
insert_exec_list(DbrJourn journ, struct DbrSlNode* first, DbrBool enriched)
{
    struct ElmZmqStore* store = journ_implof(journ);
    struct DbrBody body = { .req_id = store->id++, .type = DBR_INSERT_EXEC_LIST_REQ,
                            .insert_exec_list_req = { .first = first, .count_ = 0 } };
    if (!(dbr_send_body(store->sock, &body, DBR_FALSE)))
        return DBR_FALSE;
    return dbr_recv_body(store->sock, store->pool, &body);
}

static DbrBool
update_exec(DbrJourn journ, DbrIden id, DbrMillis modified)
{
    struct ElmZmqStore* store = journ_implof(journ);
    struct DbrBody body = { .req_id = store->id++, .type = DBR_UPDATE_EXEC_REQ,
                            .update_exec_req = { .id = id, .modified = modified } };
    if (!(dbr_send_body(store->sock, &body, DBR_FALSE)))
        return DBR_FALSE;
    return dbr_recv_body(store->sock, store->pool, &body);
}

static const struct DbrJournVtbl JOURN_VTBL = {
    .alloc_id = alloc_id,
    .insert_exec_list = insert_exec_list,
    .update_exec = update_exec
};

static ssize_t
read_entity(DbrModel model, int type, DbrPool pool, struct DbrSlNode** first)
{
    struct ElmZmqStore* store = model_implof(model);
    struct DbrBody body = { .req_id = store->id++, .type = DBR_READ_ENTITY_REQ,
                            .read_entity_req.type = type };

    if (!dbr_send_body(store->sock, &body, DBR_FALSE))
        return -1;

    if (!dbr_recv_body(store->sock, pool, &body))
        return -1;

    *first = body.entity_list_rep.first;
    return body.entity_list_rep.count_;
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
