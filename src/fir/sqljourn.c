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
#include <dbr/sqljourn.h>

#include "sqlite.h"

#include <dbr/err.h>

#include <stdlib.h>

struct FirSqlJourn {
    struct FirSqlite impl;
    DbrIden id;
    struct DbrIJourn journ;
};

static inline struct FirSqlJourn*
journ_implof(DbrJourn journ)
{
    return dbr_implof(struct FirSqlJourn, journ, journ);
}

static void
destroy(DbrJourn journ)
{
    struct FirSqlJourn* journ = journ_implof(journ);
    fir_sqlite_term(&journ->impl);
    free(journ);
}

static DbrIden
alloc_id(DbrJourn journ)
{
    struct FirSqlJourn* journ = journ_implof(journ);
    return journ->id++;
}

static DbrBool
insert_exec_list(DbrJourn journ, struct DbrSlNode* first, DbrBool enriched)
{
    struct FirSqlJourn* journ = journ_implof(journ);
    struct FirSqlite* impl = &journ->impl;

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
    struct FirSqlJourn* journ = journ_implof(journ);
    struct FirSqlite* impl = &journ->impl;
    return fir_sqlite_insert_exec(impl, exec, enriched);
}

static DbrBool
update_exec(DbrJourn journ, DbrIden id, DbrMillis modified)
{
    struct FirSqlJourn* journ = journ_implof(journ);
    struct FirSqlite* impl = &journ->impl;
    return fir_sqlite_update_exec(impl, id, modified);
}

static const struct DbrJournVtbl JOURN_VTBL = {
    .destroy = journ_destroy,
    .alloc_id = alloc_id,
    .insert_exec_list = insert_exec_list,
    .insert_exec = insert_exec,
    .update_exec = update_exec
};

DBR_API DbrJourn
dbr_sqljourn_create(const char* path, DbrIden seed)
{
    struct FirSqlJourn* journ = malloc(sizeof(struct FirSqlJourn));
    if (dbr_unlikely(!journ)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    if (!fir_sqlite_init(&journ->impl, path))
        goto fail2;

    // Seed identity.
    journ->id = seed;
    journ->journ.vtbl = &JOURN_VTBL;
    return &journ->journ;
 fail2:
    free(journ);
 fail1:
    return NULL;
}
