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
#ifndef SWIRLY_DB_TYPES_HPP
#define SWIRLY_DB_TYPES_HPP

#include <memory>

using sqlite3 = struct sqlite3;
using sqlite3_stmt = struct sqlite3_stmt;

using MYSQL = struct st_mysql;
using MYSQL_BIND = struct st_mysql_bind;
using MYSQL_RES = struct st_mysql_res;
using MYSQL_STMT = struct st_mysql_stmt;
using my_bool = char;

namespace swirly {
inline namespace util {
class Config;
} // namespace util
inline namespace db {

enum class DbType : int { Null, Int, BigInt, Char, VarChar };

constexpr struct MaybeNullTag {
} MaybeNull{};

namespace sqlite {
using Db = sqlite3;
using DbPtr = std::shared_ptr<sqlite3>;
using Stmt = sqlite3_stmt;
using StmtPtr = std::unique_ptr<sqlite3_stmt, int (*)(sqlite3_stmt*)>;
} // namespace sqlite

namespace mysql {
using Db = MYSQL;
using DbPtr = std::shared_ptr<MYSQL>;
using Res = MYSQL_RES;
using ResPtr = std::unique_ptr<MYSQL_RES, void (*)(MYSQL_RES*)>;
using Stmt = MYSQL_STMT;
using StmtPtr = std::unique_ptr<MYSQL_STMT, my_bool (*)(MYSQL_STMT*)>;
} // namespace mysql

} // namespace db
} // namespace swirly

#endif // SWIRLY_DB_TYPES_HPP
