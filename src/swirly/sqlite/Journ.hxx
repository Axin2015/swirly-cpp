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
#ifndef SWIRLY_SQLITE_JOURN_HXX
#define SWIRLY_SQLITE_JOURN_HXX

#include "Utility.hxx"

#include <swirly/fin/Journ.hpp>
#include <swirly/fin/MsgHandler.hpp>
#include <swirly/fin/Transaction.hpp>

namespace swirly {

class SqlJourn
: public Transactional
, public Journ
, BasicMsgHandler<SqlJourn> {
    // Crtp.
    friend struct BasicMsgHandler<SqlJourn>;

  public:
    explicit SqlJourn(const Config& config);
    ~SqlJourn() noexcept override;

    // Copy.
    SqlJourn(const SqlJourn&) = delete;
    SqlJourn& operator=(const SqlJourn&) = delete;

    // Move.
    SqlJourn(SqlJourn&&);
    SqlJourn& operator=(SqlJourn&&);

  protected:
    void doBegin() override;

    void doCommit() override;

    void doRollback() override;

    void doUpdate(const Msg& msg) override;

  private:
    void onReset();

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

} // namespace swirly

#endif // SWIRLY_SQLITE_JOURN_HXX
