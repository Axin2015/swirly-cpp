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

constexpr auto InsertMarketSql = //
  "INSERT INTO market_t (mnem, display, contr, settl_day, expiry_day, state)" //
  " VALUES (?, ?, ?, ?, ?, ?)"_sv;

constexpr auto UpdateMarketSql = //
  "UPDATE Market_t SET display = ?2, state = ?3" //
  " WHERE mnem = ?1"_sv;

constexpr auto InsertTraderSql = //
  "INSERT INTO trader_t (mnem, display, email)" //
  " VALUES (?, ?, ?)"_sv;

constexpr auto UpdateTraderSql = //
  "UPDATE trader_t SET display = ?2" //
  " WHERE mnem = ?1"_sv;

constexpr auto InsertExecSql = //
  "INSERT INTO exec_t (trader, market, contr, settl_day, id, ref, order_id," //
  " state_id, side_id, lots, ticks, resd, exec, cost, last_lots, last_ticks," //
  " min_lots, match_id, role_id, cpty, archive, created, modified)" //
  " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"_sv;

constexpr auto UpdateOrderSql = //
  "UPDATE order_t SET archive = 1, modified = ?3" //
  " WHERE market = ?1 AND id = ?2"_sv;

constexpr auto UpdateExecSql = //
  "UPDATE exec_t SET archive = 1, modified = ?3" //
  " WHERE market = ?1 AND id = ?2"_sv;

} // anonymous

Journ::Journ(const Conf& conf)
  : db_{openDb(conf.get("sqlite_journ", "swirly.db"))},
    ctx_{*db_},
    insertMarketStmt_{prepare(*db_, InsertMarketSql)},
    updateMarketStmt_{prepare(*db_, UpdateMarketSql)},
    insertTraderStmt_{prepare(*db_, InsertTraderSql)},
    updateTraderStmt_{prepare(*db_, UpdateTraderSql)},
    insertExecStmt_{prepare(*db_, InsertExecSql)},
    updateOrderStmt_{prepare(*db_, UpdateOrderSql)},
    updateExecStmt_{prepare(*db_, UpdateExecSql)}
{
}

Journ::~Journ() noexcept = default;

Journ::Journ(Journ&&) = default;

Journ& Journ::operator=(Journ&&) = default;

void Journ::doCreateMarket(string_view mnem, string_view display, string_view contr, Jday settlDay,
                           Jday expiryDay, MarketState state)
{
  auto& stmt = *insertMarketStmt_;

  ScopedBind bind{stmt};
  bind(mnem);
  bind(display);
  bind(contr);
  bind(settlDay, MaybeNull);
  bind(expiryDay, MaybeNull);
  bind(state);

  stepOnce(stmt);
}

void Journ::doUpdateMarket(string_view mnem, string_view display, MarketState state)
{
  auto& stmt = *updateMarketStmt_;

  ScopedBind bind{stmt};
  bind(mnem);
  bind(display);
  bind(state);

  stepOnce(stmt);
}

void Journ::doCreateTrader(string_view mnem, string_view display, string_view email)
{
  auto& stmt = *insertTraderStmt_;

  ScopedBind bind{stmt};
  bind(mnem);
  bind(display);
  bind(email);

  stepOnce(stmt);
}

void Journ::doUpdateTrader(string_view mnem, string_view display)
{
  auto& stmt = *updateTraderStmt_;

  ScopedBind bind{stmt};
  bind(mnem);
  bind(display);

  stepOnce(stmt);
}

void Journ::doCreateExec(const Exec& exec)
{
  auto& stmt = *insertExecStmt_;

  ScopedBind bind{stmt};
  bind(exec.trader());
  bind(exec.market());
  bind(exec.contr());
  bind(exec.settlDay(), MaybeNull);
  bind(exec.id());
  bind(exec.ref(), MaybeNull);
  bind(exec.orderId(), MaybeNull);
  bind(exec.state());
  bind(exec.side());
  bind(exec.lots());
  bind(exec.ticks());
  bind(exec.resd());
  bind(exec.exec());
  bind(exec.cost());
  if (exec.lastLots() > 0_lts) {
    bind(exec.lastLots());
    bind(exec.lastTicks());
  } else {
    bind(nullptr);
    bind(nullptr);
  }
  bind(exec.minLots());
  bind(exec.matchId(), MaybeNull);
  bind(exec.role(), MaybeNull);
  bind(exec.cpty(), MaybeNull);
  bind(0); // Archive.
  bind(exec.created()); // Created.
  bind(exec.created()); // Modified.

  stepOnce(stmt);
}

void Journ::doCreateExec(string_view market, ArrayView<ConstExecPtr> execs)
{
  // N.B. the market parameter is unused in the SQLite implementation.
  doCreateExec(execs);
}

void Journ::doCreateExec(ArrayView<ConstExecPtr> execs)
{
  if (execs.size() == 1) {
    doCreateExec(*execs.front());
  } else {
    ScopedTrans trans{ctx_};
    for (const auto& exec : execs) {
      doCreateExec(*exec);
    }
    trans.commit();
  }
}

void Journ::doArchiveOrder(string_view market, Iden id, Millis modified)
{
  auto& stmt = *updateOrderStmt_;

  ScopedBind bind{stmt};
  bind(market);
  bind(id);
  bind(modified);

  stepOnce(stmt);
}

void Journ::doArchiveOrder(string_view market, ArrayView<Iden> ids, Millis modified)
{
  if (ids.size() == 1) {
    doArchiveOrder(market, ids.front(), modified);
  } else {
    ScopedTrans trans{ctx_};
    for (const auto id : ids) {
      doArchiveOrder(market, id, modified);
    }
    trans.commit();
  }
}

void Journ::doArchiveTrade(string_view market, Iden id, Millis modified)
{
  auto& stmt = *updateExecStmt_;

  ScopedBind bind{stmt};
  bind(market);
  bind(id);
  bind(modified);

  stepOnce(stmt);
}

void Journ::doArchiveTrade(string_view market, ArrayView<Iden> ids, Millis modified)
{
  if (ids.size() == 1) {
    doArchiveTrade(market, ids.front(), modified);
  } else {
    ScopedTrans trans{ctx_};
    for (const auto id : ids) {
      doArchiveTrade(market, id, modified);
    }
    trans.commit();
  }
}

} // sqlite

SWIRLY_API std::unique_ptr<Journ> makeJourn(const Conf& conf)
{
  return make_unique<sqlite::Journ>(conf);
}

} // swirly
