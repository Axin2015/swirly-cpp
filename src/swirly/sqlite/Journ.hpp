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
#ifndef SWIRLY_SQLITE_JOURN_HPP
#define SWIRLY_SQLITE_JOURN_HPP

#include <swirly/sqlite/Types.hpp>

#include <swirly/fin/Journ.hpp>
#include <swirly/fin/MsgHandler.hpp>
#include <swirly/fin/Transaction.hpp>

namespace swirly {
inline namespace util {
class Config;
} // namespace util
inline namespace sqlite {

class SWIRLY_API SqlJourn
: public Journ
, BasicMsgHandler<SqlJourn> {
    using Transaction = BasicTransaction<SqlJourn>;
    // Crtp.
    friend struct BasicMsgHandler<SqlJourn>;
    friend class BasicTransaction<SqlJourn>;

  public:
    explicit SqlJourn(const Config& config);
    ~SqlJourn() override;

    // Copy.
    SqlJourn(const SqlJourn&) = delete;
    SqlJourn& operator=(const SqlJourn&) = delete;

    // Move.
    SqlJourn(SqlJourn&&);
    SqlJourn& operator=(SqlJourn&&);

  protected:
    void doWrite(const Msg& msg) override;

  private:
    void begin();

    void commit();

    void rollback() noexcept;

    void onCreateMarket(const CreateMarket& body);

    void onUpdateMarket(const UpdateMarket& body);

    void onCreateExec(const CreateExec& body);

    void onArchiveTrade(const ArchiveTrade& body);

    sqlite::DbPtr db_;
    sqlite::StmtPtr beginStmt_;
    sqlite::StmtPtr commitStmt_;
    sqlite::StmtPtr rollbackStmt_;
    sqlite::StmtPtr insertMarketStmt_;
    sqlite::StmtPtr updateMarketStmt_;
    sqlite::StmtPtr insertExecStmt_;
    sqlite::StmtPtr updateExecStmt_;
};

} // namespace sqlite
} // namespace swirly

#endif // SWIRLY_SQLITE_JOURN_HPP
