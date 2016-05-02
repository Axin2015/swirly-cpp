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
#ifndef SWIRLY_SQLITE_SQLITE_HPP
#define SWIRLY_SQLITE_SQLITE_HPP

#include <experimental/string_view>

#include <sqlite3.h>

#include <memory>
#include <type_traits>

namespace std {
using experimental::string_view;
}

namespace swirly {
namespace sqlite {

/**
 * @addtogroup IO
 * @{
 */

using DbPtr = std::unique_ptr<sqlite3, int (*)(sqlite3*)>;
using StmtPtr = std::unique_ptr<sqlite3_stmt, int (*)(sqlite3_stmt*)>;

DbPtr openDb(const char* path);

StmtPtr prepare(sqlite3& db, std::string_view sql);

bool step(sqlite3_stmt& stmt);

// FIXME: use is_enum_v<> when C++17.
template <typename ValueT>
inline ValueT column(sqlite3_stmt& stmt, int col,
                     std::enable_if_t<std::is_enum<ValueT>::value, ValueT>* = nullptr) noexcept
{
  return static_cast<ValueT>(sqlite3_column_int(&stmt, col));
}

template <typename ValueT>
inline ValueT
column(sqlite3_stmt& stmt, int col,
       std::enable_if_t<std::is_same<ValueT, int>::value || std::is_same<ValueT, unsigned>::value,
                        ValueT>* = nullptr) noexcept
{
  return sqlite3_column_int(&stmt, col);
}

template <typename ValueT>
inline ValueT column(
  sqlite3_stmt& stmt, int col,
  std::enable_if_t<std::is_same<ValueT, int64_t>::value || std::is_same<ValueT, uint64_t>::value,
                   ValueT>* = nullptr) noexcept
{
  return sqlite3_column_int64(&stmt, col);
}

template <typename ValueT>
inline ValueT column(sqlite3_stmt& stmt, int col,
                     std::enable_if_t<std::is_same<ValueT, const char*>::value
                                        || std::is_same<ValueT, std::string_view>::value,
                                      ValueT>* = nullptr) noexcept
{
  return reinterpret_cast<const char*>(sqlite3_column_text(&stmt, col));
}

/** @} */

} // sqlite
} // swirly

#endif // SWIRLY_SQLITE_SQLITE_HPP
