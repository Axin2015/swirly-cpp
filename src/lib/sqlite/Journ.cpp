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
#include "Journ.hpp"

#include <swirly/elm/Exec.hpp>

#include <swirly/ash/Conf.hpp>

using namespace std;

namespace swirly {
namespace sqlite {
namespace {

constexpr auto BeginSql = "BEGIN TRANSACTION"_sv;
constexpr auto CommitSql = "COMMIT TRANSACTION"_sv;
constexpr auto RollbackSql = "ROLLBACK TRANSACTION"_sv;

constexpr auto InsertMarketSql = //
  "INSERT INTO market_t (mnem, display, contr, settl_day, expiry_day, state)" //
  " VALUES (?, ?, ?, ?, ?, ?)"_sv;

constexpr auto UpdateMarketSql = //
  "UPDATE Market_t SET display = ?2, state = ?3" //
  " WHERE mnem = ?1"_sv;

constexpr auto InsertExecSql = //
  "INSERT INTO exec_t (accnt, market, contr, settl_day, id, ref, order_id," //
  " state_id, side_id, lots, ticks, resd, exec, cost, last_lots, last_ticks," //
  " min_lots, match_id, liqInd_id, cpty, created)" //
  " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"_sv;

constexpr auto UpdateExecSql = //
  "UPDATE exec_t SET archive = ?3" //
  " WHERE market = ?1 AND id = ?2"_sv;

} // anonymous

Journ::Journ(const Conf& conf)
  : db_{openDb(conf.get("sqlite_journ", "swirly.db"), SQLITE_OPEN_READWRITE, conf)},
    beginStmt_{prepare(*db_, BeginSql)},
    commitStmt_{prepare(*db_, CommitSql)},
    rollbackStmt_{prepare(*db_, RollbackSql)},
    insertMarketStmt_{prepare(*db_, InsertMarketSql)},
    updateMarketStmt_{prepare(*db_, UpdateMarketSql)},
    insertExecStmt_{prepare(*db_, InsertExecSql)},
    updateExecStmt_{prepare(*db_, UpdateExecSql)}
{
}

Journ::~Journ() noexcept = default;

Journ::Journ(Journ&&) = default;

Journ& Journ::operator=(Journ&&) = default;

void Journ::doBegin()
{
  stepOnce(*beginStmt_);
}

void Journ::doCommit()
{
  stepOnce(*commitStmt_);
}

void Journ::doRollback()
{
  stepOnce(*rollbackStmt_);
}

void Journ::doUpdate(const Msg& msg)
{
  dispatch(msg);
}

void Journ::reset()
{
  Transactional::reset();
}

void Journ::createMarket(const CreateMarketBody& body)
{
  auto& stmt = *insertMarketStmt_;

  ScopedBind bind{stmt};
  bind(toStringView(body.mnem));
  bind(toStringView(body.display));
  bind(toStringView(body.contr));
  bind(body.settlDay, MaybeNull);
  bind(body.expiryDay, MaybeNull);
  bind(body.state);

  stepOnce(stmt);
}

void Journ::updateMarket(const UpdateMarketBody& body)
{
  auto& stmt = *updateMarketStmt_;

  ScopedBind bind{stmt};
  bind(toStringView(body.mnem));
  bind(toStringView(body.display));
  bind(body.state);

  stepOnce(stmt);
}

void Journ::createExec(const CreateExecBody& body)
{
  Transaction trans{*this, body.more};
  if (failed()) {
    return;
  }
  auto& stmt = *insertExecStmt_;

  ScopedBind bind{stmt};
  bind(toStringView(body.accnt));
  bind(toStringView(body.market));
  bind(toStringView(body.contr));
  bind(body.settlDay, MaybeNull);
  bind(body.id);
  bind(toStringView(body.ref), MaybeNull);
  bind(body.orderId, MaybeNull);
  bind(body.state);
  bind(body.side);
  bind(body.lots);
  bind(body.ticks);
  bind(body.resd);
  bind(body.exec);
  bind(body.cost);
  if (body.lastLots > 0_lts) {
    bind(body.lastLots);
    bind(body.lastTicks);
  } else {
    bind(nullptr);
    bind(nullptr);
  }
  bind(body.minLots);
  bind(body.matchId, MaybeNull);
  bind(body.liqInd, MaybeNull);
  bind(toStringView(body.cpty), MaybeNull);
  bind(body.created); // Created.

  stepOnce(stmt);
  trans.commit();
}

void Journ::archiveTrade(const ArchiveTradeBody& body)
{
  Transaction trans{*this, body.more};
  if (failed()) {
    return;
  }
  auto& stmt = *updateExecStmt_;

  for (size_t i{0}; i < MaxIds; ++i) {
    const auto id = body.ids[i];
    if (id == 0_id) {
      break;
    }
    ScopedBind bind{stmt};
    bind(toStringView(body.market));
    bind(id);
    bind(body.modified);

    stepOnce(stmt);
  }
  trans.commit();
}

} // sqlite

std::unique_ptr<Journ> makeJourn(const Conf& conf)
{
  return make_unique<sqlite::Journ>(conf);
}

} // swirly
