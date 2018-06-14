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
#include "DbCtx.hpp"

#include "Exception.hpp"

#include "Sqlite.hxx"
#include "SqliteJourn.hpp"
#include "SqliteModel.hpp"

#if SWIRLY_HAVE_MYSQL
#include "MySql.hxx"
#include "MySqlJourn.hpp"
#include "MySqlModel.hpp"
#endif

#include <swirly/util/Config.hpp>
#include <swirly/util/Log.hpp>

namespace swirly {
inline namespace db {
namespace {
struct SqliteImpl : DbCtx::Impl {
    explicit SqliteImpl(const sqlite::DbPtr& db)
    : model_{db}
    , journ_{db}
    {
    }
    ~SqliteImpl() override = default;
    Model& do_model() override { return model_; }
    Journ& do_journ() override { return journ_; }

  private:
    SqliteModel model_;
    SqliteJourn journ_;
};

#if SWIRLY_HAVE_MYSQL
struct MySqlImpl : DbCtx::Impl {
    explicit MySqlImpl(const mysql::DbPtr& db)
    : model_{db}
    , journ_{db}
    {
    }
    ~MySqlImpl() override = default;
    Model& do_model() override { return model_; }
    Journ& do_journ() override { return journ_; }

  private:
    MySqlModel model_;
    MySqlJourn journ_;
};
#endif

std::unique_ptr<DbCtx::Impl> make_impl(const Config& config)
{
    const char* const db_type{config.get("db_type", "sqlite")};
    SWIRLY_INFO << "db_type:    "sv << db_type;

    std::unique_ptr<DbCtx::Impl> impl;
    if (std::strcmp(db_type, "sqlite") == 0) {

        const char* const db_name{config.get("db_name", "swirly.db")};
        SWIRLY_INFO << "db_name:    "sv << db_name;
        impl
            = std::make_unique<SqliteImpl>(sqlite::open_db(db_name, SQLITE_OPEN_READWRITE, config));

#if SWIRLY_HAVE_MYSQL
    } else if (std::strcmp(db_type, "mariadb") == 0 || std::strcmp(db_type, "mysql") == 0) {

        const char* const db_host{config.get("db_host", "localhost")};
        const char* const db_user{config.get("db_user", "swirlyuser")};
        const char* const db_pass{config.get("db_pass", "")};
        const char* const db_name{config.get("db_name", "swirlydb")};
        const auto db_port{config.get<unsigned>("db_port", 3306)};

        SWIRLY_INFO << "db_host:    "sv << db_host;
        SWIRLY_INFO << "db_user:    "sv << db_user;
        SWIRLY_INFO << "db_name:    "sv << db_name;
        SWIRLY_INFO << "db_port:    "sv << db_port;

        impl = std::make_unique<MySqlImpl>(
            mysql::open_db(db_host, db_user, db_pass, db_name, db_port, config));
#endif
    } else {
        throw DbException{err_msg() << "db_type '"sv << db_type << "' not supported"sv};
    }
    return impl;
}
} // namespace

DbCtx::Impl::~Impl() = default;

DbCtx::DbCtx(const Config& config)
: impl_{make_impl(config)}
{
}

DbCtx::~DbCtx() = default;

} // namespace db
} // namespace swirly
