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
#include "Model.hpp"

#include <swirly/elm/Asset.hpp>
#include <swirly/elm/Contr.hpp>
#include <swirly/elm/Exec.hpp>
#include <swirly/elm/Market.hpp>
#include <swirly/elm/Market.hpp>

#include <swirly/elm/Order.hpp>
#include <swirly/elm/Posn.hpp>

#include <swirly/ash/Conf.hpp>

using namespace std;

namespace swirly {
namespace sqlite {
namespace {

constexpr auto SelectAssetSql = //
  "SELECT id, mnem, display, type_id FROM asset_t"_sv;

constexpr auto SelectContrSql = //
  "SELECT id, mnem, display, asset, ccy, tick_numer, tick_denom, lot_numer, lot_denom,"
  " pip_dp, min_lots, max_lots FROM contr_v"_sv;

constexpr auto SelectMarketSql = //
  "SELECT id, contr, settl_day, state, last_lots, last_ticks, last_time, max_id" //
  " FROM market_v"_sv;

constexpr auto SelectAccntSql = //
  "SELECT mnem FROM accnt_t WHERE modified > ?"_sv;

constexpr auto SelectOrderSql = //
  "SELECT accnt, market_id, contr, settl_day, id, ref, state_id, side_id, lots, ticks, resd," //
  " exec, cost, last_lots, last_ticks, min_lots, created, modified" //
  " FROM order_t WHERE resd > 0;"_sv;

constexpr auto SelectExecSql = //
  "SELECT market_id, contr, settl_day, id, ref, order_id, state_id, side_id, lots, ticks," //
  " resd, exec, cost, last_lots, last_ticks, min_lots, match_id, liqInd_id, cpty, created" //
  " FROM exec_t WHERE accnt = ? ORDER BY seq_id DESC LIMIT ?;"_sv;

constexpr auto SelectTradeSql = //
  "SELECT accnt, market_id, contr, settl_day, id, ref, order_id, side_id, lots, ticks, resd," //
  " exec, cost, last_lots, last_ticks, min_lots, match_id, liqInd_id, cpty, created" //
  " FROM exec_t WHERE state_id = 4 AND archive IS NULL;"_sv;

constexpr auto SelectPosnSql = //
  "SELECT accnt, contr, settl_day, side_id, lots, cost FROM posn_v;"_sv;

} // anonymous

Model::Model(const Conf& conf)
  : db_{openDb(conf.get("sqlite_model", "swirly.db"), SQLITE_OPEN_READONLY, conf)}
{
}

Model::~Model() noexcept = default;

Model::Model(Model&&) = default;

Model& Model::operator=(Model&&) = default;

void Model::doReadAsset(const ModelCallback<AssetPtr>& cb) const
{
  enum {
    Id, //
    Mnem, //
    Display, //
    TypeId //
  };

  StmtPtr stmt{prepare(*db_, SelectAssetSql)};
  while (step(*stmt)) {
    cb(Asset::make(column<Id32>(*stmt, Id), //
                   column<string_view>(*stmt, Mnem), //
                   column<string_view>(*stmt, Display), //
                   column<AssetType>(*stmt, TypeId)));
  }
}

void Model::doReadContr(const ModelCallback<ContrPtr>& cb) const
{
  enum {
    Id, //
    Mnem, //
    Display, //
    Asset, //
    Ccy, //
    TickNumer, //
    TickDenom, //
    LotNumer, //
    LotDenom, //
    PipDp, //
    MinLots, //
    MaxLots //
  };

  StmtPtr stmt{prepare(*db_, SelectContrSql)};
  while (step(*stmt)) {
    cb(Contr::make(column<Id32>(*stmt, Id), //
                   column<string_view>(*stmt, Mnem), //
                   column<string_view>(*stmt, Display), //
                   column<string_view>(*stmt, Asset), //
                   column<string_view>(*stmt, Ccy), //
                   column<int>(*stmt, LotNumer), //
                   column<int>(*stmt, LotDenom), //
                   column<int>(*stmt, TickNumer), //
                   column<int>(*stmt, TickDenom), //
                   column<int>(*stmt, PipDp), //
                   column<Lots>(*stmt, MinLots), //
                   column<Lots>(*stmt, MaxLots)));
  }
}

void Model::doReadMarket(const ModelCallback<MarketPtr>& cb) const
{
  enum { //
    Id, //
    Contr, //
    SettlDay, //
    State, //
    LastLots, //
    LastTicks, //
    LastTime, //
    MaxId //
  };

  StmtPtr stmt{prepare(*db_, SelectMarketSql)};
  while (step(*stmt)) {
    cb(Market::make(column<Id64>(*stmt, Id), //
                    column<string_view>(*stmt, Contr), //
                    column<JDay>(*stmt, SettlDay), //
                    column<MarketState>(*stmt, State), //
                    column<Lots>(*stmt, LastLots), //
                    column<Ticks>(*stmt, LastTicks), //
                    column<Millis>(*stmt, LastTime), //
                    column<Id64>(*stmt, MaxId)));
  }
}

void Model::doReadAccnt(Millis now, const ModelCallback<string_view>& cb) const
{
  using namespace enumops;

  enum { //
    Mnem //
  };

  StmtPtr stmt{prepare(*db_, SelectAccntSql)};
  ScopedBind bind{*stmt};
  // One week ago.
  bind(now - 604800000_ms);
  while (step(*stmt)) {
    cb(column<string_view>(*stmt, Mnem));
  }
}

void Model::doReadOrder(const ModelCallback<OrderPtr>& cb) const
{
  enum { //
    Accnt, //
    MarketId, //
    Contr, //
    SettlDay, //
    Id, //
    Ref, //
    State, //
    Side, //
    Lots, //
    Ticks, //
    Resd, //
    Exec, //
    Cost, //
    LastLots, //
    LastTicks, //
    MinLots, //
    Created, //
    Modified //
  };

  StmtPtr stmt{prepare(*db_, SelectOrderSql)};
  while (step(*stmt)) {
    cb(Order::make(column<string_view>(*stmt, Accnt), //
                   column<Id64>(*stmt, MarketId), //
                   column<string_view>(*stmt, Contr), //
                   column<JDay>(*stmt, SettlDay), //
                   column<Id64>(*stmt, Id), //
                   column<string_view>(*stmt, Ref), //
                   column<swirly::State>(*stmt, State), //
                   column<swirly::Side>(*stmt, Side), //
                   column<swirly::Lots>(*stmt, Lots), //
                   column<swirly::Ticks>(*stmt, Ticks), //
                   column<swirly::Lots>(*stmt, Resd), //
                   column<swirly::Lots>(*stmt, Exec), //
                   column<swirly::Cost>(*stmt, Cost), //
                   column<swirly::Lots>(*stmt, LastLots), //
                   column<swirly::Ticks>(*stmt, LastTicks), //
                   column<swirly::Lots>(*stmt, MinLots), //
                   column<Millis>(*stmt, Created), //
                   column<Millis>(*stmt, Modified)));
  }
}

void Model::doReadExec(string_view accnt, size_t limit, const ModelCallback<ExecPtr>& cb) const
{
  enum { //
    MarketId, //
    Contr, //
    SettlDay, //
    Id, //
    Ref, //
    OrderId, //
    State, //
    Side, //
    Lots, //
    Ticks, //
    Resd, //
    Exec, //
    Cost, //
    LastLots, //
    LastTicks, //
    MinLots, //
    MatchId, //
    LiqInd, //
    Cpty, //
    Created //
  };

  StmtPtr stmt{prepare(*db_, SelectExecSql)};
  ScopedBind bind{*stmt};
  bind(accnt);
  bind(limit);
  while (step(*stmt)) {
    cb(Exec::make(accnt, //
                  column<Id64>(*stmt, MarketId), //
                  column<string_view>(*stmt, Contr), //
                  column<JDay>(*stmt, SettlDay), //
                  column<Id64>(*stmt, Id), //
                  column<string_view>(*stmt, Ref), //
                  column<Id64>(*stmt, OrderId), //
                  column<swirly::State>(*stmt, State), //
                  column<swirly::Side>(*stmt, Side), //
                  column<swirly::Lots>(*stmt, Lots), //
                  column<swirly::Ticks>(*stmt, Ticks), //
                  column<swirly::Lots>(*stmt, Resd), //
                  column<swirly::Lots>(*stmt, Exec), //
                  column<swirly::Cost>(*stmt, Cost), //
                  column<swirly::Lots>(*stmt, LastLots), //
                  column<swirly::Ticks>(*stmt, LastTicks), //
                  column<swirly::Lots>(*stmt, MinLots), //
                  column<Id64>(*stmt, MatchId), //
                  column<swirly::LiqInd>(*stmt, LiqInd), //
                  column<string_view>(*stmt, Cpty), //
                  column<Millis>(*stmt, Created)));
  }
}

void Model::doReadTrade(const ModelCallback<ExecPtr>& cb) const
{
  enum { //
    Accnt, //
    MarketId, //
    Contr, //
    SettlDay, //
    Id, //
    Ref, //
    OrderId, //
    Side, //
    Lots, //
    Ticks, //
    Resd, //
    Exec, //
    Cost, //
    LastLots, //
    LastTicks, //
    MinLots, //
    MatchId, //
    LiqInd, //
    Cpty, //
    Created //
  };

  StmtPtr stmt{prepare(*db_, SelectTradeSql)};
  while (step(*stmt)) {
    cb(Exec::make(column<string_view>(*stmt, Accnt), //
                  column<Id64>(*stmt, MarketId), //
                  column<string_view>(*stmt, Contr), //
                  column<JDay>(*stmt, SettlDay), //
                  column<Id64>(*stmt, Id), //
                  column<string_view>(*stmt, Ref), //
                  column<Id64>(*stmt, OrderId), //
                  State::Trade, //
                  column<swirly::Side>(*stmt, Side), //
                  column<swirly::Lots>(*stmt, Lots), //
                  column<swirly::Ticks>(*stmt, Ticks), //
                  column<swirly::Lots>(*stmt, Resd), //
                  column<swirly::Lots>(*stmt, Exec), //
                  column<swirly::Cost>(*stmt, Cost), //
                  column<swirly::Lots>(*stmt, LastLots), //
                  column<swirly::Ticks>(*stmt, LastTicks), //
                  column<swirly::Lots>(*stmt, MinLots), //
                  column<Id64>(*stmt, MatchId), //
                  column<swirly::LiqInd>(*stmt, LiqInd), //
                  column<string_view>(*stmt, Cpty), //
                  column<Millis>(*stmt, Created)));
  }
}

void Model::doReadPosn(JDay busDay, const ModelCallback<PosnPtr>& cb) const
{
  enum { //
    Accnt, //
    Contr, //
    SettlDay, //
    Side, //
    Lots, //
    Cost //
  };

  PosnSet ps;
  PosnSet::Iterator it;

  StmtPtr stmt{prepare(*db_, SelectPosnSql)};
  while (step(*stmt)) {
    const auto accnt = column<string_view>(*stmt, Accnt);
    const auto contr = column<string_view>(*stmt, Contr);
    auto settlDay = column<JDay>(*stmt, SettlDay);

    // FIXME: review when end of day is implemented.
    if (settlDay != 0_jd && settlDay <= busDay) {
      settlDay = 0_jd;
    }

    bool found;
    tie(it, found) = ps.findHint(accnt, contr, settlDay);
    if (!found) {
      it = ps.insertHint(it, Posn::make(accnt, contr, settlDay));
    }

    const auto side = column<swirly::Side>(*stmt, Side);
    const auto lots = column<swirly::Lots>(*stmt, Lots);
    const auto cost = column<swirly::Cost>(*stmt, Cost);
    if (side == swirly::Side::Buy) {
      it->addBuy(lots, cost);
    } else {
      it->addSell(lots, cost);
    }
  }

  for (it = ps.begin(); it != ps.end();) {
    cb(ps.remove(it++));
  }
}

} // sqlite

unique_ptr<Model> makeModel(const Conf& conf)
{
  return make_unique<sqlite::Model>(conf);
}

} // swirly
