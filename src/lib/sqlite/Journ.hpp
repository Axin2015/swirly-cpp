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
#ifndef SWIRLY_SQLITE_JOURN_HPP
#define SWIRLY_SQLITE_JOURN_HPP

#include "Utility.hpp"

#include <swirly/fin/Journ.hpp>
#include <swirly/fin/MsgHandler.hpp>
#include <swirly/fin/Transaction.hpp>

namespace swirly {
namespace sqlite {

class Journ : public Transactional, public swirly::Journ, BasicMsgHandler<Journ> {
  // Crtp.
  friend struct BasicMsgHandler<Journ>;

 public:
  explicit Journ(const Conf& conf);
  ~Journ() noexcept override;

  // Copy.
  Journ(const Journ&) = delete;
  Journ& operator=(const Journ&) = delete;

  // Move.
  Journ(Journ&&);
  Journ& operator=(Journ&&);

 protected:
  void doBegin() override;

  void doCommit() override;

  void doRollback() override;

  void doUpdate(const Msg& msg) override;

 private:
  void onReset();

  void onCreateMarket(const CreateMarketBody& body);

  void onUpdateMarket(const UpdateMarketBody& body);

  void onCreateExec(const CreateExecBody& body);

  void onArchiveTrade(const ArchiveTradeBody& body);

  DbPtr db_;
  StmtPtr beginStmt_;
  StmtPtr commitStmt_;
  StmtPtr rollbackStmt_;
  StmtPtr insertMarketStmt_;
  StmtPtr updateMarketStmt_;
  StmtPtr insertExecStmt_;
  StmtPtr updateExecStmt_;
};

} // sqlite
} // swirly

#endif // SWIRLY_SQLITE_JOURN_HPP
