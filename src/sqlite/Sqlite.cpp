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
#include "Sqlite.hpp"

#include "Exception.hpp"

#include <sqlite3.h>

using namespace std;

namespace swirly {
namespace sqlite {

SqlitePtr open(const char* path)
{
  sqlite3* db;
  int rc{sqlite3_open_v2(path, &db, SQLITE_OPEN_READWRITE, nullptr)};
  SqlitePtr ptr{db, sqlite3_close};
  if (rc != SQLITE_OK) {
    throw Error{sqlite3_errmsg(db)};
  }
  return ptr;
}

} // sqlite
} // swirly
