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
#include "MySql.hxx"

#include "Exception.hpp"

#include <swirly/util/String.hpp>

#include <mysql.h>

namespace swirly {
inline namespace db {
namespace mysql {

DbPtr open_db(const char* host, const char* user, const char* pass, const char* dbname,
              const char* unix_socket, unsigned long client_flag)
{
    DbPtr db{mysql_init(nullptr), mysql_close};
    if (!db) {
        throw std::bad_alloc{};
    }
    if (!mysql_real_connect(db.get(), host, user, pass, dbname, 0, unix_socket, client_flag)) {
        throw DbException{err_msg() << "mysql_real_connect: "sv << mysql_error(db.get())};
    }
    return db;
}

DbPtr open_db(const char* host, const char* user, const char* pass, const char* dbname,
              unsigned int port, unsigned long client_flag)
{
    DbPtr db{mysql_init(nullptr), mysql_close};
    if (!db) {
        throw std::bad_alloc{};
    }
    mysql_protocol_type prot{MYSQL_PROTOCOL_TCP};
    mysql_options(db.get(), MYSQL_OPT_PROTOCOL, &prot);

    if (!mysql_real_connect(db.get(), host, user, pass, dbname, port, nullptr, client_flag)) {
        throw DbException{err_msg() << "mysql_real_connect: "sv << mysql_error(db.get())};
    }
    return db;
}

void query(MYSQL& db, std::string_view sql)
{
    if (mysql_real_query(&db, sql.data(), sql.size()) != 0) {
        throw DbException{err_msg() << "mysql_real_query: "sv << mysql_error(&db)};
    }
}

ResPtr store_result(MYSQL& db)
{
    ResPtr res{mysql_store_result(&db), mysql_free_result};
    if (!res) {
        throw DbException{err_msg() << "mysql_store_result: "sv << mysql_error(&db)};
    }
    return res;
}

StmtPtr prepare(MYSQL& db, std::string_view sql)
{
    StmtPtr stmt{mysql_stmt_init(&db), mysql_stmt_close};
    if (!stmt) {
        throw DbException{err_msg() << "mysql_stmt_init: "sv << mysql_error(&db)};
    }
    if (mysql_stmt_prepare(stmt.get(), sql.data(), sql.size()) != 0) {
        throw DbException{err_msg() << "mysql_stmt_prepare: "sv << mysql_stmt_error(stmt.get())};
    }
    return stmt;
}

void execute(MYSQL_STMT& stmt)
{
    if (mysql_stmt_execute(&stmt) != 0) {
        throw DbException{err_msg() << "mysql_stmt_execute: "sv << mysql_stmt_error(&stmt)};
    }
}
ResPtr result_metadata(MYSQL_STMT& stmt)
{
    ResPtr res{mysql_stmt_result_metadata(&stmt), mysql_free_result};
    if (!res) {
        throw DbException{err_msg() << "mysql_stmt_result_metadata: "sv << mysql_stmt_error(&stmt)};
    }
    return res;
}

void bind_param(MYSQL_STMT& stmt, MYSQL_BIND* bind)
{
    if (mysql_stmt_bind_param(&stmt, bind) != 0) {
        throw DbException{err_msg() << "mysql_stmt_bind_param: "sv << mysql_stmt_error(&stmt)};
    }
}

void bind_result(MYSQL_STMT& stmt, MYSQL_BIND* bind)
{
    if (mysql_stmt_bind_result(&stmt, bind) != 0) {
        throw DbException{err_msg() << "mysql_stmt_bind_result: "sv << mysql_stmt_error(&stmt)};
    }
}

std::size_t affected_rows(MYSQL_STMT& stmt)
{
    return mysql_stmt_affected_rows(&stmt);
}

bool fetch(MYSQL_STMT& stmt)
{
    switch (mysql_stmt_fetch(&stmt)) {
    case 0:
        break;
    case MYSQL_NO_DATA:
        return false;
    case MYSQL_DATA_TRUNCATED:
        throw DbException{err_msg() << "mysql_stmt_fetch: data truncated"sv};
    case 1:
    default:
        throw DbException{err_msg() << "mysql_stmt_fetch: "sv << mysql_stmt_error(&stmt)};
    }
    return true;
}

} // namespace mysql
} // namespace db
} // namespace swirly
