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
    struct sqlite3_stmt* insert_order;
    struct sqlite3_stmt* update_order;
    struct sqlite3_stmt* archive_order;
    struct sqlite3_stmt* insert_trade;
    struct sqlite3_stmt* archive_trade;
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
fir_sqlite_insert_order(struct FirSqlite* sqlite, DbrIden id, int rev, int status, DbrIden tid,
                        DbrIden aid, DbrIden cid, DbrDate settl_date, const char* ref,
                        int action, DbrTicks ticks, DbrLots lots, DbrLots resd, DbrLots exec,
                        DbrTicks last_ticks, DbrLots last_lots, DbrLots min, DbrFlags flags,
                        DbrMillis now);

DBR_EXTERN DbrBool
fir_sqlite_update_order(struct FirSqlite* sqlite, DbrIden id, int rev, int status, DbrLots lots,
                        DbrLots resd, DbrLots exec, DbrTicks last_ticks, DbrLots last_lots,
                        DbrMillis now);

DBR_EXTERN DbrBool
fir_sqlite_archive_order(struct FirSqlite* sqlite, DbrIden id, DbrMillis now);

DBR_EXTERN DbrBool
fir_sqlite_insert_trade(struct FirSqlite* sqlite, DbrIden id, DbrIden order, int rev,
                        DbrIden match, int role, DbrIden cpty, DbrMillis now);

DBR_EXTERN DbrBool
fir_sqlite_archive_trade(struct FirSqlite* sqlite, DbrIden id, DbrMillis now);

DBR_EXTERN ssize_t
fir_sqlite_select_entity(struct FirSqlite* sqlite, int type, DbrPool pool,
                         struct DbrSlNode** first);

#endif // FIR_SQLITE_H
