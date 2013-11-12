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
#define _XOPEN_SOURCE 700 // strnlen()
#include <dbr/sqlstore.h>

#include "sqlite.h"

#include <dbr/err.h>
#include <dbr/journ.h>
#include <dbr/model.h>

#include <stdlib.h>

struct FirSqlStore {
    DbrIden id;
    struct FirSqlite impl;
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

static DbrIden
alloc_id(DbrJourn journ)
{
    struct FirSqlStore* store = journ_implof(journ);
    return store->id++;
}

static DbrBool
begin_trans(DbrJourn journ)
{
    struct FirSqlStore* store = journ_implof(journ);
    struct FirSqlite* impl = &store->impl;
    return fir_sqlite_begin_trans(impl);
}

static DbrBool
commit_trans(DbrJourn journ)
{
    struct FirSqlStore* store = journ_implof(journ);
    struct FirSqlite* impl = &store->impl;
    return fir_sqlite_commit_trans(impl);
}

static DbrBool
rollback_trans(DbrJourn journ)
{
    struct FirSqlStore* store = journ_implof(journ);
    struct FirSqlite* impl = &store->impl;
    return fir_sqlite_rollback_trans(impl);
}

static DbrBool
insert_order(DbrJourn journ, DbrIden id, int rev, int status, DbrIden tid, DbrIden aid,
             DbrIden cid, DbrDate settl_date, const char* ref, int action, DbrTicks ticks,
             DbrLots lots, DbrLots resd, DbrLots exec, DbrTicks last_ticks, DbrLots last_lots,
             DbrLots min, DbrFlags flags, DbrMillis now)
{
    struct FirSqlStore* store = journ_implof(journ);
    struct FirSqlite* impl = &store->impl;
    return fir_sqlite_insert_order(impl, id, rev, status, tid, aid, cid, settl_date, ref, action,
                                   lots, ticks, resd, exec, last_ticks, last_lots, min, flags, now);
}

static DbrBool
update_order(DbrJourn journ, DbrIden id, int rev, int status, DbrLots lots, DbrLots resd,
             DbrLots exec, DbrTicks last_ticks, DbrLots last_lots, DbrMillis now)
{
    struct FirSqlStore* store = journ_implof(journ);
    struct FirSqlite* impl = &store->impl;
    return fir_sqlite_update_order(impl, id, rev, status, lots, resd, exec, last_ticks,
                                   last_lots, now);
}

static DbrBool
archive_order(DbrJourn journ, DbrIden id, DbrMillis now)
{
    struct FirSqlStore* store = journ_implof(journ);
    struct FirSqlite* impl = &store->impl;
    return fir_sqlite_archive_order(impl, id, now);
}

static DbrBool
insert_trade(DbrJourn journ, DbrIden id, DbrIden order, int rev, DbrIden match, int role,
             DbrIden cpty, DbrMillis now)
{
    struct FirSqlStore* store = journ_implof(journ);
    struct FirSqlite* impl = &store->impl;
    return fir_sqlite_insert_trade(impl, id, order, rev, match, role, cpty, now);
}

static DbrBool
archive_trade(DbrJourn journ, DbrIden id, DbrMillis now)
{
    struct FirSqlStore* store = journ_implof(journ);
    struct FirSqlite* impl = &store->impl;
    return fir_sqlite_archive_trade(impl, id, now);
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
    struct FirSqlStore* store = model_implof(model);
    struct FirSqlite* impl = &store->impl;
    return fir_sqlite_select_entity(impl, type, pool, first);
}

static const struct DbrModelVtbl MODEL_VTBL = {
    .read_entity = read_entity
};

DBR_API DbrSqlStore
dbr_sqlstore_create(DbrIden seed, const char* path)
{
    struct FirSqlStore* store = malloc(sizeof(struct FirSqlStore));
    if (dbr_unlikely(!store)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    // Seed identity.
    store->id = seed;

    if (!fir_sqlite_init(&store->impl, path))
        goto fail2;

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
