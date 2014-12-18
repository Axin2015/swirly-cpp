/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef FIR_SQLITE_H
#define FIR_SQLITE_H

#include <dbr/elm/types.h>

#include <sys/types.h> // ssize_t

struct sqlite3;
struct sqlite3_stmt;

struct FirSqlite {
    struct sqlite3* db;
    struct sqlite3_stmt* insert_exec;
    struct sqlite3_stmt* update_exec;
    const struct DbrModel* model_;
};

DBR_EXTERN DbrBool
fir_sqlite_init(struct FirSqlite* sqlite, const char* path);

// Assumes that pointer is not null.

DBR_EXTERN void
fir_sqlite_term(struct FirSqlite* sqlite);

DBR_EXTERN DbrBool
fir_sqlite_load(struct FirSqlite* sqlite);

DBR_EXTERN DbrBool
fir_sqlite_begin_trans(struct FirSqlite* sqlite);

DBR_EXTERN DbrBool
fir_sqlite_commit_trans(struct FirSqlite* sqlite);

DBR_EXTERN DbrBool
fir_sqlite_rollback_trans(struct FirSqlite* sqlite);

DBR_EXTERN DbrBool
fir_sqlite_insert_exec(struct FirSqlite* sqlite, const struct DbrExec* exec, DbrBool enriched);

DBR_EXTERN DbrBool
fir_sqlite_update_exec(struct FirSqlite* sqlite, DbrIden id, DbrMillis modified);

DBR_EXTERN ssize_t
fir_sqlite_select_entity(struct FirSqlite* sqlite, int type, DbrPool pool,
                         struct DbrSlNode** first);

#endif // FIR_SQLITE_H
