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
#ifndef FIR_SQLITE_H
#define FIR_SQLITE_H

#include <dbr/pool.h>
#include <dbr/types.h>

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
