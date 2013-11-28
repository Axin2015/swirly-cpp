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
insert_exec(DbrJourn journ, const struct DbrExec* exec)
{
    struct FirSqlStore* store = journ_implof(journ);
    struct FirSqlite* impl = &store->impl;
    return fir_sqlite_insert_exec(impl, exec->id, exec->order, exec->c.trader.rec->id,
                                  exec->c.accnt.rec->id, exec->c.contr.rec->id,
                                  exec->c.settl_date, exec->c.ref, exec->c.status, exec->c.action,
                                  exec->c.ticks, exec->c.lots, exec->c.resd, exec->c.exec,
                                  exec->c.last_ticks, exec->c.last_lots, exec->c.min_lots,
                                  exec->match, exec->role, exec->cpty.rec->id, exec->created);
}

static DbrBool
insert_stmt(DbrJourn journ, const struct DbrStmt* stmt)
{
    struct FirSqlStore* store = journ_implof(journ);
    struct FirSqlite* impl = &store->impl;
    switch (stmt->type) {
    case DBR_INSERT_EXEC:
        return fir_sqlite_insert_exec(impl,
                                      stmt->insert_exec.id,
                                      stmt->insert_exec.order,
                                      stmt->insert_exec.c.trader.rec->id,
                                      stmt->insert_exec.c.accnt.rec->id,
                                      stmt->insert_exec.c.contr.rec->id,
                                      stmt->insert_exec.c.settl_date,
                                      stmt->insert_exec.c.ref,
                                      stmt->insert_exec.c.status,
                                      stmt->insert_exec.c.action,
                                      stmt->insert_exec.c.ticks,
                                      stmt->insert_exec.c.lots,
                                      stmt->insert_exec.c.resd,
                                      stmt->insert_exec.c.exec,
                                      stmt->insert_exec.c.last_ticks,
                                      stmt->insert_exec.c.last_lots,
                                      stmt->insert_exec.c.min_lots,
                                      stmt->insert_exec.match,
                                      stmt->insert_exec.role,
                                      stmt->insert_exec.cpty.rec->id,
                                      stmt->insert_exec.created);
    case DBR_ARCHIVE_TRADE:
        return fir_sqlite_archive_trade(impl,
                                        stmt->archive_trade.id,
                                        stmt->archive_trade.now);
    default:
        abort();
    }
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
    .insert_exec = insert_exec,
    .insert_stmt = insert_stmt,
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
