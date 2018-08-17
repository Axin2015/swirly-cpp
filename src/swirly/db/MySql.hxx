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
#ifndef SWIRLY_DB_MYSQL_HXX
#define SWIRLY_DB_MYSQL_HXX

#include <swirly/db/Types.hpp>

#include <swirly/util/IntWrapper.hpp>
#include <swirly/util/String.hpp>

#include <mysql.h>

namespace swirly {
inline namespace db {
namespace mysql {

constexpr my_bool False = '\0';
constexpr my_bool True = '\1';

template <std::size_t N>
using BindArray = std::array<MYSQL_BIND, N>;

template <enum_field_types TypeN, typename ValueT>
class DbInt {
  public:
    using ValueType = ValueT;
    static constexpr enum_field_types Type = TypeN;
    explicit DbInt(MYSQL_BIND& bind, ValueT val = 0) noexcept
    : value_{val}
    {
        std::memset(&bind, 0, sizeof(bind));
        bind.buffer_type = Type;
        bind.buffer = &value_;
        bind.is_null = &is_null_;
    }
    DbInt(MYSQL_BIND& bind, ValueT val, MaybeNullTag) noexcept
    : DbInt{bind, val}
    {
        if (val == 0) {
            is_null_ = True;
        }
    }
    template <typename PolicyT>
    DbInt(MYSQL_BIND& bind, IntWrapper<PolicyT> val) noexcept
    : DbInt{bind, val.count()}
    {
    }
    template <typename PolicyT>
    DbInt(MYSQL_BIND& bind, IntWrapper<PolicyT> val, MaybeNullTag) noexcept
    : DbInt{bind, val.count(), MaybeNull}
    {
    }
    ValueT value() const noexcept { return value_; }
    bool is_null() const noexcept { return is_null_ != False; }

    void set_value(ValueT val) noexcept
    {
        value_ = val;
        is_null_ = False;
    }
    void set_value(ValueT val, MaybeNullTag) noexcept
    {
        value_ = val;
        is_null_ = val == 0 ? True : False;
    }
    template <typename PolicyT>
    void set_value(IntWrapper<PolicyT> val) noexcept
    {
        set_value(val.count());
    }
    template <typename PolicyT>
    void set_value(IntWrapper<PolicyT> val, MaybeNullTag) noexcept
    {
        set_value(val.count(), MaybeNull);
    }
    void set_null(bool is_null = true) noexcept { is_null_ = is_null ? True : False; }

  private:
    ValueT value_{};
    my_bool is_null_{};
};

template <enum_field_types TypeN, std::size_t SizeN>
class DbString {
  public:
    static constexpr enum_field_types Type = TypeN;
    explicit DbString(MYSQL_BIND& bind) noexcept
    {
        std::memset(&bind, 0, sizeof(bind));
        bind.buffer_type = Type;
        bind.buffer = value_;
        bind.buffer_length = SizeN;
        bind.length = &size_;
        bind.is_null = &is_null_;
    }
    static void bind(MYSQL_BIND& bind, std::string_view val, bool is_null = false) noexcept
    {
        std::memset(&bind, 0, sizeof(bind));
        bind.buffer_type = Type;
        bind.buffer = const_cast<char*>(val.data());
        bind.buffer_length = val.size();
        bind.length = &bind.buffer_length;

        static my_bool bool_val[] = {False, True};
        bind.is_null = &bool_val[static_cast<int>(is_null)];
    }
    static void bind(MYSQL_BIND& bind, std::string_view val, MaybeNullTag) noexcept
    {
        DbString::bind(bind, val, val.empty());
    }
    static void bind(MYSQL_BIND& bind, const char (&val)[SizeN]) noexcept
    {
        DbString::bind(bind, to_string_view(val));
    }
    static void bind(MYSQL_BIND& bind, const char (&val)[SizeN], MaybeNullTag) noexcept
    {
        DbString::bind(bind, to_string_view(val), MaybeNull);
    }
    std::string_view value() const noexcept { return {value_, size_}; }
    std::size_t size() const noexcept { return size_; }
    bool is_null() const noexcept { return is_null_ != False; }

  private:
    char value_[SizeN];
    unsigned long size_{};
    my_bool is_null_{};
};

template <DbType, std::size_t SizeN = 0>
struct DbField;

template <>
struct DbField<DbType::Null> {
    static constexpr enum_field_types Type = MYSQL_TYPE_NULL;
    static void bind(MYSQL_BIND& bind) noexcept
    {
        std::memset(&bind, 0, sizeof(bind));
        bind.buffer_type = MYSQL_TYPE_NULL;
    }
};

template <>
struct DbField<DbType::Int> : DbInt<MYSQL_TYPE_LONG, std::int32_t> {
    using DbInt::DbInt;
};

template <>
struct DbField<DbType::BigInt> : DbInt<MYSQL_TYPE_LONGLONG, std::int64_t> {
    using DbInt::DbInt;
};

template <std::size_t SizeN>
struct DbField<DbType::Char, SizeN> : DbString<MYSQL_TYPE_STRING, SizeN> {
    using DbString<MYSQL_TYPE_STRING, SizeN>::DbString;
};

template <std::size_t SizeN>
struct DbField<DbType::VarChar, SizeN> : DbString<MYSQL_TYPE_VAR_STRING, SizeN> {
    using DbString<MYSQL_TYPE_VAR_STRING, SizeN>::DbString;
};

namespace field {
using Null = DbField<DbType::Null>;
using Cost = DbField<DbType::BigInt>;
using Denom = DbField<DbType::Int>;
using Display = DbField<DbType::VarChar, 64>;
using Id32 = DbField<DbType::Int>;
using Id64 = DbField<DbType::BigInt>;
using JDay = DbField<DbType::Int>;
using LiqInd = DbField<DbType::Int>;
using Lots = DbField<DbType::BigInt>;
using MarketState = DbField<DbType::Int>;
using MatchId = DbField<DbType::BigInt>;
using Numer = DbField<DbType::Int>;
using PipDp = DbField<DbType::Int>;
using Ref = DbField<DbType::VarChar, 64>;
using Side = DbField<DbType::Int>;
using State = DbField<DbType::Int>;
using Symbol = DbField<DbType::Char, 16>;
using Ticks = DbField<DbType::BigInt>;
using Time = DbField<DbType::BigInt>;
} // namespace field

DbPtr open_db(const char* host, const char* user, const char* pass, const char* dbname,
              const char* unix_socket, unsigned long client_flag = 0);

inline DbPtr open_db(const char* host, const char* user, const char* pass, const char* dbname,
                     const char* unix_socket, const Config& config)
{
    // There are no MySQL-specific config settings for now.
    return open_db(host, user, pass, dbname, unix_socket, 0);
}

DbPtr open_db(const char* host, const char* user, const char* pass, const char* dbname,
              unsigned port, unsigned long client_flag = 0);

inline DbPtr open_db(const char* host, const char* user, const char* pass, const char* dbname,
                     unsigned port, const Config& config)
{
    // There are no MySQL-specific config settings for now.
    return open_db(host, user, pass, dbname, port, 0);
}

void query(MYSQL& db, std::string_view sql);

ResPtr store_result(MYSQL& db);

StmtPtr prepare(MYSQL& db, std::string_view sql);

void execute(MYSQL_STMT& stmt);

ResPtr result_metadata(MYSQL_STMT& stmt);

void bind_param(MYSQL_STMT& stmt, MYSQL_BIND* bind);

void bind_result(MYSQL_STMT& stmt, MYSQL_BIND* bind);

std::size_t affected_rows(MYSQL_STMT& stmt);

bool fetch(MYSQL_STMT& stmt);

} // namespace mysql
} // namespace db
} // namespace swirly

#endif // SWIRLY_DB_MYSQL_HXX
