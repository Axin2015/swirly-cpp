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
#ifndef DBR_SQLITE_H
#define DBR_SQLITE_H

#include <dbr/pool.h>
#include <dbr/types.h>

/**
 * @addtogroup Sqlite
 * @{
 */

typedef struct DbrSqlite_* DbrSqlite;

/**
 * @brief Create SQLite3 model.
 *
 * @param pool Pool.
 *
 * @param seed Seed identifier.
 *
 * @param path Path to the database.
 *
 * @return Handle to newly created model or null on failure.
 */

DBR_API DbrSqlite
dbr_sqlite_create(DbrPool pool, DbrIden seed, const char* path);

DBR_API void
dbr_sqlite_destroy(DbrSqlite sqlite);

DBR_API DbrJourn
dbr_sqlite_journ(DbrSqlite sqlite);

DBR_API DbrModel
dbr_sqlite_model(DbrSqlite sqlite);

/** @} */

#endif // DBR_SQLITE_H
