/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#include "Utility.hpp"

#include "Exception.hpp"

#include <swirly/ash/Conf.hpp>
#include <swirly/ash/Log.hpp>
#include <swirly/ash/String.hpp>

#include <cassert>

using namespace std;

namespace swirly {
namespace sqlite {
namespace {

const char* lastError(sqlite3& db)
{
  return sqlite3_errmsg(&db);
}

const char* lastError(sqlite3_stmt& stmt)
{
  return sqlite3_errmsg(sqlite3_db_handle(&stmt));
}

void trace(void* unused, const char* sql)
{
  SWIRLY_INFO(sql);
}

} // anonymous

DbPtr openDb(const char* path, int flags, const Conf& conf)
{
  sqlite3* db;
  int rc{sqlite3_open_v2(path, &db, flags, nullptr)};
  DbPtr ptr{db, sqlite3_close};
  if (rc != SQLITE_OK) {
    throw Error{errMsg() << "sqlite3_open_v2 failed: " << path << ": " << lastError(*db)};
  }
  if (conf.get("sqlite_enable_trace", false)) {
    sqlite3_trace(db, trace, nullptr);
  }
  if (conf.get("sqlite_enable_fkey", false)) {
    rc = sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, 1, nullptr);
    if (rc != SQLITE_OK) {
      throw Error{errMsg() << "sqlite3_db_config failed: " << path << ": " << lastError(*db)};
    }
  }
  {
    StmtPtr stmt{prepare(*db, "PRAGMA journal_mode = MEMORY"_sv)};
    stepOnce(*stmt);
  }
  {
    StmtPtr stmt{prepare(*db, "PRAGMA synchronous = OFF"_sv)};
    stepOnce(*stmt);
  }
  return ptr;
}

StmtPtr prepare(sqlite3& db, std::string_view sql)
{
  sqlite3_stmt* stmt;
  int rc{sqlite3_prepare_v2(&db, sql.data(), sql.size(), &stmt, nullptr)};
  StmtPtr ptr{stmt, sqlite3_finalize};
  if (rc != SQLITE_OK) {
    throw Error{errMsg() << "sqlite3_prepare_v2 failed: " << sql << ": " << lastError(db)};
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
    throw Error{errMsg() << "sqlite3_step failed: " << lastError(stmt)};
    break;
  }
}

void bind(sqlite3_stmt& stmt, int col, nullptr_t)
{
  int rc{sqlite3_bind_null(&stmt, col)};
  if (rc != SQLITE_OK) {
    throw Error{errMsg() << "sqlite3_bind_null failed: " << lastError(stmt)};
  }
}

void bind(sqlite3_stmt& stmt, int col, int val)
{
  int rc{sqlite3_bind_int(&stmt, col, val)};
  if (rc != SQLITE_OK) {
    throw Error{errMsg() << "sqlite3_bind_int failed: " << lastError(stmt)};
  }
}

void bind(sqlite3_stmt& stmt, int col, int64_t val)
{
  int rc{sqlite3_bind_int64(&stmt, col, val)};
  if (rc != SQLITE_OK) {
    throw Error{errMsg() << "sqlite3_bind_int64 failed: " << lastError(stmt)};
  }
}

void bind(sqlite3_stmt& stmt, int col, string_view val)
{
  int rc{sqlite3_bind_text(&stmt, col, val.data(), val.size(), SQLITE_STATIC)};
  if (rc != SQLITE_OK) {
    throw Error{errMsg() << "sqlite3_bind_text failed: " << lastError(stmt)};
  }
}

TransCtx::TransCtx(sqlite3& db)
  : beginStmt_{prepare(db, "BEGIN TRANSACTION"_sv)},
    commitStmt_{prepare(db, "COMMIT TRANSACTION"_sv)},
    rollbackStmt_{prepare(db, "ROLLBACK TRANSACTION"_sv)}
{
}

ScopedTrans::~ScopedTrans() noexcept
{
  if (!done_) {
    try {
      ctx_.rollback();
    } catch (const exception& e) {
      SWIRLY_ERROR(logMsg() << "exception: " << e.what());
    }
  }
}

} // sqlite
} // swirly
