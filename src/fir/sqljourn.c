/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/fir/sqljourn.h>

#include "sqlite.h"

#include <sc/ash/err.h>

#include <stdlib.h>

struct SqlJourn {
    struct FirSqlite sqlite;
    struct ScIJourn i_journ;
};

static inline struct SqlJourn*
journ_implof(ScJourn journ)
{
    return sc_implof(struct SqlJourn, i_journ, journ);
}

static void
destroy(ScJourn journ)
{
    struct SqlJourn* impl = journ_implof(journ);
    fir_sqlite_term(&impl->sqlite);
    free(impl);
}

static ScBool
insert_exec_list(ScJourn journ, struct ScSlNode* first, ScBool enriched)
{
    struct SqlJourn* impl = journ_implof(journ);
    struct FirSqlite* sqlite = &impl->sqlite;

    if (!fir_sqlite_begin_trans(sqlite))
        goto fail1;

    for (struct ScSlNode* node = first; node; node = node->next) {
        struct ScExec* exec = sc_shared_exec_entry(node);
        if (!fir_sqlite_insert_exec(sqlite, exec, enriched)) {
            fir_sqlite_rollback_trans(sqlite);
            goto fail1;
        }
    }
    if (!fir_sqlite_commit_trans(sqlite))
        goto fail1;

    return SC_TRUE;
 fail1:
    return SC_FALSE;
}

static ScBool
insert_exec(ScJourn journ, struct ScExec* exec, ScBool enriched)
{
    struct SqlJourn* impl = journ_implof(journ);
    struct FirSqlite* sqlite = &impl->sqlite;
    return fir_sqlite_insert_exec(sqlite, exec, enriched);
}

static ScBool
update_exec(ScJourn journ, ScIden id, ScMillis modified)
{
    struct SqlJourn* impl = journ_implof(journ);
    struct FirSqlite* sqlite = &impl->sqlite;
    return fir_sqlite_update_exec(sqlite, id, modified);
}

static const struct ScJournVtbl JOURN_VTBL = {
    .destroy = destroy,
    .insert_exec_list = insert_exec_list,
    .insert_exec = insert_exec,
    .update_exec = update_exec
};

SC_API ScJourn
sc_sqljourn_create(const char* path)
{
    struct SqlJourn* impl = malloc(sizeof(struct SqlJourn));
    if (sc_unlikely(!impl)) {
        sc_err_set(SC_ENOMEM, "out of memory");
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
