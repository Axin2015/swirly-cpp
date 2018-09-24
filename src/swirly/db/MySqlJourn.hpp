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
#ifndef SWIRLY_DB_MYSQLJOURN_HPP
#define SWIRLY_DB_MYSQLJOURN_HPP

#include <swirly/db/Types.hpp>

#include <swirly/fin/Journ.hpp>
#include <swirly/fin/MsgHandler.hpp>

namespace swirly {
inline namespace db {

class SWIRLY_API MySqlJourn
: public Journ
, BasicMsgHandler<MySqlJourn> {
    // Crtp.
    friend struct BasicMsgHandler<MySqlJourn>;

  public:
    explicit MySqlJourn(const mysql::DbPtr& db);
    explicit MySqlJourn(const Config& config);
    ~MySqlJourn() override;

    // Copy.
    MySqlJourn(const MySqlJourn&) = delete;
    MySqlJourn& operator=(const MySqlJourn&) = delete;

    // Move.
    MySqlJourn(MySqlJourn&&);
    MySqlJourn& operator=(MySqlJourn&&);

  protected:
    void do_begin() override;

    void do_commit() override;

    void do_rollback() noexcept override;

    void do_write(const Msg& msg) override;

  private:
    void on_create_market(WallTime now, const CreateMarket& body);

    void on_update_market(WallTime now, const UpdateMarket& body);

    void on_create_exec(WallTime now, const CreateExec& body);

    void on_archive_trade(WallTime now, const ArchiveTrade& body);

    mysql::DbPtr db_;
    mysql::StmtPtr insert_market_stmt_;
    mysql::StmtPtr update_market_stmt_;
    mysql::StmtPtr insert_exec_stmt_;
    mysql::StmtPtr update_exec_stmt_;
};

} // namespace db
} // namespace swirly

#endif // SWIRLY_DB_MYSQLJOURN_HPP
