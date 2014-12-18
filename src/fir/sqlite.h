/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef FIR_SQLITE_H
#define FIR_SQLITE_H

#include <sc/elm/types.h>

#include <sys/types.h> // ssize_t

struct sqlite3;
struct sqlite3_stmt;

struct FirSqlite {
    struct sqlite3* db;
    struct sqlite3_stmt* insert_exec;
    struct sqlite3_stmt* update_exec;
    const struct ScModel* model_;
};

SC_EXTERN ScBool
fir_sqlite_init(struct FirSqlite* sqlite, const char* path);

// Assumes that pointer is not null.

SC_EXTERN void
fir_sqlite_term(struct FirSqlite* sqlite);

SC_EXTERN ScBool
fir_sqlite_load(struct FirSqlite* sqlite);

SC_EXTERN ScBool
fir_sqlite_begin_trans(struct FirSqlite* sqlite);

SC_EXTERN ScBool
fir_sqlite_commit_trans(struct FirSqlite* sqlite);

SC_EXTERN ScBool
fir_sqlite_rollback_trans(struct FirSqlite* sqlite);

SC_EXTERN ScBool
fir_sqlite_insert_exec(struct FirSqlite* sqlite, const struct ScExec* exec, ScBool enriched);

SC_EXTERN ScBool
fir_sqlite_update_exec(struct FirSqlite* sqlite, ScIden id, ScMillis modified);

SC_EXTERN ssize_t
fir_sqlite_select_entity(struct FirSqlite* sqlite, int type, ScPool pool,
                         struct ScSlNode** first);

#endif // FIR_SQLITE_H
