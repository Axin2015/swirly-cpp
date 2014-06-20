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
#include <dbr/fir/sqljourn.h>

#include "sqlite.h"

#include <dbr/ash/err.h>

#include <stdlib.h>

struct SqlJourn {
    struct FirSqlite sqlite;
    struct DbrIJourn i_journ;
};

static inline struct SqlJourn*
journ_implof(DbrJourn journ)
{
    return dbr_implof(struct SqlJourn, i_journ, journ);
}

static void
destroy(DbrJourn journ)
{
    struct SqlJourn* impl = journ_implof(journ);
    fir_sqlite_term(&impl->sqlite);
    free(impl);
}

static DbrBool
insert_exec_list(DbrJourn journ, struct DbrSlNode* first, DbrBool enriched)
{
    struct SqlJourn* impl = journ_implof(journ);
    struct FirSqlite* sqlite = &impl->sqlite;

    if (!fir_sqlite_begin_trans(sqlite))
        goto fail1;

    for (struct DbrSlNode* node = first; node; node = node->next) {
        struct DbrExec* exec = dbr_shared_exec_entry(node);
        if (!fir_sqlite_insert_exec(sqlite, exec, enriched)) {
            fir_sqlite_rollback_trans(sqlite);
            goto fail1;
        }
    }
    if (!fir_sqlite_commit_trans(sqlite))
        goto fail1;

    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}

static DbrBool
insert_exec(DbrJourn journ, struct DbrExec* exec, DbrBool enriched)
{
    struct SqlJourn* impl = journ_implof(journ);
    struct FirSqlite* sqlite = &impl->sqlite;
    return fir_sqlite_insert_exec(sqlite, exec, enriched);
}

static DbrBool
update_exec(DbrJourn journ, DbrIden id, DbrMillis modified)
{
    struct SqlJourn* impl = journ_implof(journ);
    struct FirSqlite* sqlite = &impl->sqlite;
    return fir_sqlite_update_exec(sqlite, id, modified);
}

static const struct DbrJournVtbl JOURN_VTBL = {
    .destroy = destroy,
    .insert_exec_list = insert_exec_list,
    .insert_exec = insert_exec,
    .update_exec = update_exec
};

DBR_API DbrJourn
dbr_sqljourn_create(const char* path)
{
    struct SqlJourn* impl = malloc(sizeof(struct SqlJourn));
    if (dbr_unlikely(!impl)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    if (!fir_sqlite_init(&impl->sqlite, path))
        goto fail2;

    impl->i_journ.vtbl = &JOURN_VTBL;
    return &impl->i_journ;
 fail2:
    free(impl);
 fail1:
    return NULL;
}
