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
#ifndef FIR_SQLSTORE_H
#define FIR_SQLSTORE_H

#include <dbr/pool.h>
#include <dbr/types.h>

#include <sys/types.h> // ssize_t

struct sqlite3;
struct sqlite3_stmt;

struct FirSqlStore {
    struct sqlite3* db;
    struct sqlite3_stmt* insert_order;
    struct sqlite3_stmt* update_order;
    struct sqlite3_stmt* archive_order;
    struct sqlite3_stmt* insert_trade;
    struct sqlite3_stmt* archive_trade;
    const struct DbrModel* model_;
};

DBR_EXTERN DbrBool
fir_sqlstore_init(struct FirSqlStore* sqlstore, const char* path);

// Assumes that pointer is not null.

DBR_EXTERN void
fir_sqlstore_term(struct FirSqlStore* sqlstore);

DBR_EXTERN DbrBool
fir_sqlstore_load(struct FirSqlStore* sqlstore);

DBR_EXTERN DbrBool
fir_sqlstore_begin_trans(struct FirSqlStore* sqlstore);

DBR_EXTERN DbrBool
fir_sqlstore_commit_trans(struct FirSqlStore* sqlstore);

DBR_EXTERN DbrBool
fir_sqlstore_rollback_trans(struct FirSqlStore* sqlstore);

DBR_EXTERN DbrBool
fir_sqlstore_insert_order(struct FirSqlStore* sqlstore, const struct DbrOrder* order);

DBR_EXTERN DbrBool
fir_sqlstore_update_order(struct FirSqlStore* sqlstore, DbrIden id, int rev, int status,
                          DbrLots resd, DbrLots exec, DbrLots lots, DbrMillis now);

DBR_EXTERN DbrBool
fir_sqlstore_archive_order(struct FirSqlStore* sqlstore, DbrIden id, DbrMillis now);

DBR_EXTERN DbrBool
fir_sqlstore_insert_trade(struct FirSqlStore* sqlstore, const struct DbrTrade* trade);

DBR_EXTERN DbrBool
fir_sqlstore_archive_trade(struct FirSqlStore* sqlstore, DbrIden id, DbrMillis now);

DBR_EXTERN ssize_t
fir_sqlstore_select_entity(struct FirSqlStore* sqlstore, int type, DbrPool pool,
                           struct DbrSlNode** first);

#endif // FIR_SQLSTORE_H
