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
#include <dbr/sqlstore.h>

#include "sqlite.h"

#include <dbr/err.h>

#include <stdlib.h>

struct FirSqlStore {
    struct FirSqlite impl;
    DbrIden id;
    struct DbrIJourn journ;
    struct DbrIModel model;
};

static inline struct FirSqlStore*
journ_implof(DbrJourn journ)
{
    return dbr_implof(struct FirSqlStore, journ, journ);
}

static inline struct FirSqlStore*
model_implof(DbrModel model)
{
    return dbr_implof(struct FirSqlStore, model, model);
}

static void
journ_destroy(DbrJourn journ)
{
}

static DbrIden
alloc_id(DbrJourn journ)
{
    struct FirSqlStore* store = journ_implof(journ);
    return store->id++;
}

static DbrBool
insert_exec_list(DbrJourn journ, struct DbrSlNode* first, DbrBool enriched)
{
    struct FirSqlStore* store = journ_implof(journ);
    struct FirSqlite* impl = &store->impl;

    if (!fir_sqlite_begin_trans(impl))
        goto fail1;

    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrExec* exec = dbr_shared_exec_entry(node);
        if (!fir_sqlite_insert_exec(impl, exec, enriched)) {
            fir_sqlite_rollback_trans(impl);
            goto fail1;
        }
    }
    if (!fir_sqlite_commit_trans(impl))
        goto fail1;

    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

static DbrBool
insert_exec(DbrJourn journ, struct DbrExec* exec, DbrBool enriched)
{
    struct FirSqlStore* store = journ_implof(journ);
    struct FirSqlite* impl = &store->impl;
    return fir_sqlite_insert_exec(impl, exec, enriched);
}

static DbrBool
update_exec(DbrJourn journ, DbrIden id, DbrMillis modified)
{
    struct FirSqlStore* store = journ_implof(journ);
    struct FirSqlite* impl = &store->impl;
    return fir_sqlite_update_exec(impl, id, modified);
}

static const struct DbrJournVtbl JOURN_VTBL = {
    .destroy = journ_destroy,
    .alloc_id = alloc_id,
    .insert_exec_list = insert_exec_list,
    .insert_exec = insert_exec,
    .update_exec = update_exec
};

static void
model_destroy(DbrModel model)
{
}

static ssize_t
read_entity(DbrModel model, int type, DbrPool pool, struct DbrSlNode** first)
{
    struct FirSqlStore* store = model_implof(model);
    struct FirSqlite* impl = &store->impl;
    return fir_sqlite_select_entity(impl, type, pool, first);
}

static const struct DbrModelVtbl MODEL_VTBL = {
    .destroy = model_destroy,
    .read_entity = read_entity
};

DBR_API DbrSqlStore
dbr_sqlstore_create(const char* path, DbrIden seed)
{
    struct FirSqlStore* store = malloc(sizeof(struct FirSqlStore));
    if (dbr_unlikely(!store)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    if (!fir_sqlite_init(&store->impl, path))
        goto fail2;

    // Seed identity.
    store->id = seed;
    store->journ.vtbl = &JOURN_VTBL;
    store->model.vtbl = &MODEL_VTBL;
    return store;
 fail2:
    free(store);
 fail1:
    return NULL;
}

DBR_API void
dbr_sqlstore_destroy(DbrSqlStore store)
{
    if (store) {
        fir_sqlite_term(&store->impl);
        free(store);
    }
}

DBR_API DbrJourn
dbr_sqlstore_journ(DbrSqlStore store)
{
    return &store->journ;
}

DBR_API DbrModel
dbr_sqlstore_model(DbrSqlStore store)
{
    return &store->model;
}
