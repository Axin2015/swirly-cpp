/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#include "Sqlite.hxx"

#include <swirly/app/Exception.hpp>

#include <swirly/util/Config.hpp>
#include <swirly/util/Log.hpp>

#include <cassert>

namespace swirly {
inline namespace db {
namespace sqlite {
using namespace std;
namespace {

const char* last_error(sqlite3& db)
{
    return sqlite3_errmsg(&db);
}

const char* last_error(sqlite3_stmt& stmt)
{
    return sqlite3_errmsg(sqlite3_db_handle(&stmt));
}

void trace(void* unused, const char* sql)
{
    SWIRLY_INFO << sql;
}

} // namespace

namespace detail {

void bind32(sqlite3_stmt& stmt, int col, int32_t val)
{
    int rc{sqlite3_bind_int(&stmt, col, val)};
    if (rc != SQLITE_OK) {
        throw DatabaseException{err_msg() << "sqlite3_bind_int: " << last_error(stmt)};
    }
}

void bind64(sqlite3_stmt& stmt, int col, int64_t val)
{
    int rc{sqlite3_bind_int64(&stmt, col, val)};
    if (rc != SQLITE_OK) {
        throw DatabaseException{err_msg() << "sqlite3_bind_int64: " << last_error(stmt)};
    }
}

void bindsv(sqlite3_stmt& stmt, int col, string_view val)
{
    int rc{sqlite3_bind_text(&stmt, col, val.data(), val.size(), SQLITE_STATIC)};
    if (rc != SQLITE_OK) {
        throw DatabaseException{err_msg() << "sqlite3_bind_text: " << last_error(stmt)};
    }
}

} // namespace detail

DbPtr open_db(const char* path, int flags, const Config& config)
{
    sqlite3* db;
    int rc{sqlite3_open_v2(path, &db, flags, nullptr)};
    DbPtr ptr{db, sqlite3_close};
    if (rc != SQLITE_OK) {
        throw DatabaseException{err_msg()
                                << "sqlite3_open_v2: " << last_error(*db) << ": " << path};
    }
    if (config.get("sqlite_enable_trace", false)) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        sqlite3_trace(db, trace, nullptr);
#pragma GCC diagnostic pop
    }
    if (config.get("sqlite_enable_fkey", false)) {
        rc = sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, 1, nullptr);
        if (rc != SQLITE_OK) {
            throw DatabaseException{err_msg()
                                    << "sqlite3_db_config: " << last_error(*db) << ": " << path};
        }
    }
    {
        StmtPtr stmt{prepare(*db, "PRAGMA journal_mode = MEMORY"sv)};
        step_once(*stmt);
    }
    {
        StmtPtr stmt{prepare(*db, "PRAGMA synchronous = OFF"sv)};
        step_once(*stmt);
    }
    return ptr;
}

StmtPtr prepare(sqlite3& db, string_view sql)
{
    sqlite3_stmt* stmt;
    int rc{sqlite3_prepare_v2(&db, sql.data(), sql.size(), &stmt, nullptr)};
    StmtPtr ptr{stmt, sqlite3_finalize};
    if (rc != SQLITE_OK) {
        throw DatabaseException{err_msg()
                                << "sqlite3_prepare_v2: " << last_error(db) << ": " << sql};
    }
    return ptr;
}

bool step(sqlite3_stmt& stmt)
{
    int rc{sqlite3_step(&stmt)};
    switch (rc) {
    case SQLITE_OK:
        assert(false);
    case SQLITE_ROW:
        return true;
    case SQLITE_DONE:
        return false;
    default:
        throw DatabaseException{err_msg() << "sqlite3_step: " << last_error(stmt)};
        break;
    }
}

void bind(sqlite3_stmt& stmt, int col, nullptr_t)
{
    int rc{sqlite3_bind_null(&stmt, col)};
    if (rc != SQLITE_OK) {
        throw DatabaseException{err_msg() << "sqlite3_bind_null: " << last_error(stmt)};
    }
}

} // namespace sqlite
} // namespace db
} // namespace swirly
