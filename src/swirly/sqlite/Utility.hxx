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
#ifndef SWIRLY_SQLITE_UTILITY_HXX
#define SWIRLY_SQLITE_UTILITY_HXX

#include <swirly/util/Enum.hpp>
#include <swirly/util/Finally.hpp>
#include <swirly/util/IntWrapper.hpp>

#include <swirly/sys/Time.hpp>

#include <string_view>

#include <memory>
#include <type_traits>

#include <sqlite3.h>

namespace swirly {

class Config;

namespace sqlite {
namespace detail {

void bind32(sqlite3_stmt& stmt, int col, int32_t val);
void bind64(sqlite3_stmt& stmt, int col, int64_t val);
void bindsv(sqlite3_stmt& stmt, int col, std::string_view val);

template <typename ValueT, typename EnableT = void>
struct DbTraits;

/**
 * Integer 32bit.
 */
template <typename ValueT>
struct DbTraits<ValueT,
                std::enable_if_t<std::is_integral<ValueT>::value && (sizeof(ValueT) <= 4)>> {
    static constexpr bool isNull(ValueT val) noexcept { return val == 0; }
    static void bind(sqlite3_stmt& stmt, int col, ValueT val) { bind32(stmt, col, val); }
    static ValueT column(sqlite3_stmt& stmt, int col) noexcept
    {
        return sqlite3_column_int(&stmt, col);
    }
};

/**
 * Integer 64bit.
 */
template <typename ValueT>
struct DbTraits<ValueT, std::enable_if_t<std::is_integral<ValueT>::value && (sizeof(ValueT) > 4)>> {
    static constexpr bool isNull(ValueT val) noexcept { return val == 0; }
    static void bind(sqlite3_stmt& stmt, int col, ValueT val) { bind64(stmt, col, val); }
    static ValueT column(sqlite3_stmt& stmt, int col) noexcept
    {
        return sqlite3_column_int64(&stmt, col);
    }
};

/**
 * IntWrapper.
 */
template <typename ValueT>
struct DbTraits<ValueT, std::enable_if_t<isIntWrapper<ValueT>>> {

    using UnderlyingTraits = DbTraits<typename ValueT::ValueType>;

    static constexpr bool isNull(ValueT val) noexcept { return val == ValueT{0}; }
    static void bind(sqlite3_stmt& stmt, int col, ValueT val)
    {
        UnderlyingTraits::bind(stmt, col, val.count());
    }
    static ValueT column(sqlite3_stmt& stmt, int col) noexcept
    {
        return ValueT{UnderlyingTraits::column(stmt, col)};
    }
};

/**
 * Enum.
 */
template <typename ValueT>
struct DbTraits<ValueT, std::enable_if_t<std::is_enum<ValueT>::value>> {

    using UnderlyingTraits = DbTraits<std::underlying_type_t<ValueT>>;

    static constexpr bool isNull(ValueT val) noexcept { return unbox(val) == 0; }
    static void bind(sqlite3_stmt& stmt, int col, ValueT val)
    {
        UnderlyingTraits::bind(stmt, col, unbox(val));
    }
    static ValueT column(sqlite3_stmt& stmt, int col) noexcept
    {
        return static_cast<ValueT>(UnderlyingTraits::column(stmt, col));
    }
};

/**
 * Time.
 */
template <>
struct DbTraits<Time> {
    static constexpr bool isNull(Time val) noexcept { return val == Time{}; }
    static void bind(sqlite3_stmt& stmt, int col, Time val)
    {
        bind64(stmt, col, msSinceEpoch(val));
    }
    static Time column(sqlite3_stmt& stmt, int col) noexcept
    {
        return toTime(Millis{sqlite3_column_int64(&stmt, col)});
    }
};

/**
 * StringView.
 */
template <>
struct DbTraits<std::string_view> {
    static constexpr bool isNull(std::string_view val) noexcept { return val.empty(); }
    static void bind(sqlite3_stmt& stmt, int col, std::string_view val) { bindsv(stmt, col, val); }
    static std::string_view column(sqlite3_stmt& stmt, int col) noexcept
    {
        return reinterpret_cast<const char*>(sqlite3_column_text(&stmt, col));
    }
};

} // namespace detail

using DbPtr = std::unique_ptr<sqlite3, int (*)(sqlite3*)>;
using StmtPtr = std::unique_ptr<sqlite3_stmt, int (*)(sqlite3_stmt*)>;

DbPtr openDb(const char* path, int flags, const Config& config);

StmtPtr prepare(sqlite3& db, std::string_view sql);

bool step(sqlite3_stmt& stmt);

inline bool stepOnce(sqlite3_stmt& stmt)
{
    auto finally = makeFinally([&stmt]() { sqlite3_reset(&stmt); });
    return step(stmt);
}

template <typename ValueT>
inline ValueT column(sqlite3_stmt& stmt, int col) noexcept
{
    using Traits = detail::DbTraits<ValueT>;
    return Traits::column(stmt, col);
}

constexpr struct MaybeNullTag {
} MaybeNull{};

void bind(sqlite3_stmt& stmt, int col, std::nullptr_t);

template <typename ValueT>
void bind(sqlite3_stmt& stmt, int col, ValueT val)
{
    using Traits = detail::DbTraits<ValueT>;
    Traits::bind(stmt, col, val);
}

template <typename ValueT>
void bind(sqlite3_stmt& stmt, int col, ValueT val, MaybeNullTag)
{
    using Traits = detail::DbTraits<ValueT>;
    if (!Traits::isNull(val)) {
        bind(stmt, col, val);
    } else {
        bind(stmt, col, nullptr);
    }
}

class ScopedBind {
  public:
    explicit ScopedBind(sqlite3_stmt& stmt) noexcept
      : stmt_{stmt}
    {
    }
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
    explicit ScopedStep(sqlite3_stmt& stmt) noexcept
      : stmt_{stmt}
    {
    }
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

} // namespace sqlite
} // namespace swirly

#endif // SWIRLY_SQLITE_UTILITY_HXX
