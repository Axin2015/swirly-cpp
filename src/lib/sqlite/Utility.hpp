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
#ifndef SWIRLY_SQLITE_UTILITY_HPP
#define SWIRLY_SQLITE_UTILITY_HPP

#include <swirly/ash/Finally.hpp>
#include <swirly/ash/Mnem.hpp>

#include <sqlite3.h>

#include <memory>
#include <type_traits>

namespace std {
using experimental::string_view;
}

namespace swirly {

class Conf;

namespace sqlite {

using DbPtr = std::unique_ptr<sqlite3, int (*)(sqlite3*)>;
using StmtPtr = std::unique_ptr<sqlite3_stmt, int (*)(sqlite3_stmt*)>;

DbPtr openDb(const char* path, int flags, const Conf& conf);

StmtPtr prepare(sqlite3& db, std::string_view sql);

bool step(sqlite3_stmt& stmt);

inline bool stepOnce(sqlite3_stmt& stmt)
{
  auto finally = makeFinally([&stmt]() { sqlite3_reset(&stmt); });
  return step(stmt);
}

template <typename ValueT,
          typename std::enable_if_t<std::is_same<ValueT, int>::value
                                    || std::is_same<ValueT, unsigned>::value>* = nullptr>
inline ValueT column(sqlite3_stmt& stmt, int col) noexcept
{
  return sqlite3_column_int(&stmt, col);
}

template <typename ValueT,
          typename std::enable_if_t<std::is_same<ValueT, int64_t>::value
                                    || std::is_same<ValueT, uint64_t>::value>* = nullptr>
inline ValueT column(sqlite3_stmt& stmt, int col) noexcept
{
  return sqlite3_column_int64(&stmt, col);
}

// FIXME: use is_enum_v<> when C++17.
template <typename ValueT, typename std::enable_if_t<std::is_enum<ValueT>::value>* = nullptr>
inline ValueT column(sqlite3_stmt& stmt, int col) noexcept
{
  return static_cast<ValueT>(column<std::underlying_type_t<ValueT>>(stmt, col));
}

template <typename ValueT,
          typename std::enable_if_t<std::is_same<ValueT, const char*>::value
                                    || std::is_same<ValueT, std::string_view>::value>* = nullptr>
inline ValueT column(sqlite3_stmt& stmt, int col) noexcept
{
  return reinterpret_cast<const char*>(sqlite3_column_text(&stmt, col));
}

void bind(sqlite3_stmt& stmt, int col, std::nullptr_t);

void bind(sqlite3_stmt& stmt, int col, int val);

inline void bind(sqlite3_stmt& stmt, int col, unsigned val)
{
  bind(stmt, col, static_cast<int>(val));
}

void bind(sqlite3_stmt& stmt, int col, int64_t val);

inline void bind(sqlite3_stmt& stmt, int col, uint64_t val)
{
  bind(stmt, col, static_cast<int64_t>(val));
}

void bind(sqlite3_stmt& stmt, int col, std::string_view val);

void bind(sqlite3_stmt& stmt, int col, Mnem val);

constexpr struct MaybeNullTag {
} MaybeNull{};

template <typename ValueT, typename std::enable_if_t<std::is_integral<ValueT>::value>* = nullptr>
inline void bind(sqlite3_stmt& stmt, int col, ValueT val, MaybeNullTag)
{
  if (val != 0) {
    bind(stmt, col, val);
  } else {
    bind(stmt, col, nullptr);
  }
}

template <typename ValueT,
          typename std::enable_if_t<std::is_same<ValueT, std::string_view>::value
                                    || std::is_same<ValueT, Mnem>::value>* = nullptr>
inline void bind(sqlite3_stmt& stmt, int col, ValueT val, MaybeNullTag)
{
  if (!val.empty()) {
    bind(stmt, col, val);
  } else {
    bind(stmt, col, nullptr);
  }
}

template <typename ValueT, typename std::enable_if_t<std::is_enum<ValueT>::value>* = nullptr>
inline void bind(sqlite3_stmt& stmt, int col, ValueT val)
{
  bind(stmt, col, unbox(val));
}

template <typename ValueT, typename std::enable_if_t<std::is_enum<ValueT>::value>* = nullptr>
inline void bind(sqlite3_stmt& stmt, int col, ValueT val, MaybeNullTag)
{
  bind(stmt, col, unbox(val), MaybeNull);
}

class ScopedBind {
 public:
  explicit ScopedBind(sqlite3_stmt& stmt) noexcept : stmt_{stmt} {}
  ~ScopedBind() noexcept { sqlite3_clear_bindings(&stmt_); }
  // Copy.
  ScopedBind(const ScopedBind&) = delete;
  ScopedBind& operator=(const ScopedBind&) = delete;

  // Move.
  ScopedBind(ScopedBind&&) = delete;
  ScopedBind& operator=(ScopedBind&&) = delete;

  template <typename ValueT>
  void operator()(ValueT val)
  {
    sqlite::bind(stmt_, ++col_, val);
  }

  template <typename ValueT>
  void operator()(ValueT val, MaybeNullTag)
  {
    sqlite::bind(stmt_, ++col_, val, MaybeNull);
  }

 private:
  sqlite3_stmt& stmt_;
  int col_{0};
};

class ScopedStep {
 public:
  explicit ScopedStep(sqlite3_stmt& stmt) noexcept : stmt_{stmt} {}
  ~ScopedStep() noexcept { sqlite3_reset(&stmt_); }
  // Copy.
  ScopedStep(const ScopedStep&) = delete;
  ScopedStep& operator=(const ScopedStep&) = delete;

  // Move.
  ScopedStep(ScopedStep&&) = delete;
  ScopedStep& operator=(ScopedStep&&) = delete;

  bool operator()() { return sqlite::step(stmt_); }

 private:
  sqlite3_stmt& stmt_;
};

} // sqlite
} // swirly

#endif // SWIRLY_SQLITE_UTILITY_HPP
