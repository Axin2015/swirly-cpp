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
#ifndef FIG_SQLITE_H
#define FIG_SQLITE_H

#include <dbr/defs.h>
#include <dbr/types.h>

#include <sys/types.h> // ssize_t

struct sqlite3;
struct sqlite3_stmt;

struct FigSqlite {
    /**
     * @publicsection
     */
    struct ElmCtx* ctx;
    struct sqlite3* db;
    struct sqlite3_stmt* insert_order;
    struct sqlite3_stmt* update_order;
    struct sqlite3_stmt* archive_order;
    struct sqlite3_stmt* insert_trade;
    struct sqlite3_stmt* archive_trade;
    /**
     * @privatesection
     */
    const struct DbrModel* model_;
};

DBR_EXTERN DbrBool
fig_sqlite_init(struct FigSqlite* sqlite, struct ElmCtx* ctx, const char* path);

// Assumes that pointer is not null.

DBR_EXTERN void
fig_sqlite_term(struct FigSqlite* sqlite);

DBR_EXTERN DbrBool
fig_sqlite_load(struct FigSqlite* sqlite);

DBR_EXTERN DbrBool
fig_sqlite_begin(struct FigSqlite* sqlite);

DBR_EXTERN DbrBool
fig_sqlite_commit(struct FigSqlite* sqlite);

DBR_EXTERN DbrBool
fig_sqlite_rollback(struct FigSqlite* sqlite);

DBR_EXTERN DbrBool
fig_sqlite_insert_order(struct FigSqlite* sqlite, const struct DbrOrder* order);

DBR_EXTERN DbrBool
fig_sqlite_update_order(struct FigSqlite* sqlite, DbrIden id, int rev, int status,
                        DbrLots resd, DbrLots exec, DbrLots lots, DbrMillis now);

DBR_EXTERN DbrBool
fig_sqlite_archive_order(struct FigSqlite* sqlite, DbrIden id, DbrMillis now);

DBR_EXTERN DbrBool
fig_sqlite_insert_trade(struct FigSqlite* sqlite, const struct DbrTrade* trade);

DBR_EXTERN DbrBool
fig_sqlite_archive_trade(struct FigSqlite* sqlite, DbrIden id, DbrMillis now);

DBR_EXTERN ssize_t
fig_sqlite_select(struct FigSqlite* sqlite, int type, struct DbrSlNode** first);

DBR_EXTERN struct DbrSlNode*
fig_sqlite_end(struct FigSqlite* sqlite);

#endif // FIG_SQLITE_H
