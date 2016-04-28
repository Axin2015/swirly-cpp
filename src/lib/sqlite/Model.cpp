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

#include <swirly/elm/Factory.hpp>

#include <iostream>

using namespace std;

namespace swirly {
namespace sqlite {
namespace {

constexpr auto SelectAssetSql = //
  "SELECT mnem, display, type_id FROM asset_t"_sv;

constexpr auto SelectTraderSql = //
  "SELECT mnem, display, email FROM trader_t"_sv;

constexpr auto SelectContrSql = //
  "SELECT mnem, display, asset, ccy, tick_numer, tick_denom, lot_numer, lot_denom, pip_dp," //
  " min_lots, max_lots FROM contr_v"_sv;

constexpr auto SelectMarketSql = //
  "SELECT mnem, display, contr, settl_day, expiry_day, state, last_lots, last_ticks, last_time," //
  " max_order_id, max_exec_id FROM market_v"_sv;

} // anonymous

Model::Model(const char* connString) : db_{openDb(connString)}
{
}

Model::~Model() noexcept = default;

Model::Model(Model&&) = default;

Model& Model::operator=(Model&&) = default;

AssetSet Model::doReadAsset(const Factory& factory) const
{
  enum {
    Mnem, //
    Display, //
    TypeId //
  };

  AssetSet s;

  StmtPtr stmt{prepare(*db_, SelectAssetSql)};
  while (step(*stmt)) {
    s.insert(factory.newAsset(column<string_view>(*stmt, Mnem), //
                              column<string_view>(*stmt, Display), //
                              column<AssetType>(*stmt, TypeId)));
  }
  return s;
}

ContrSet Model::doReadContr(const Factory& factory) const
{
  enum {
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

  ContrSet s;

  StmtPtr stmt{prepare(*db_, SelectContrSql)};
  while (step(*stmt)) {
    s.insert(factory.newContr(column<string_view>(*stmt, Mnem), //
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
  return s;
}

MarketSet Model::doReadMarket(const Factory& factory) const
{
  enum { //
    Mnem, //
    Display, //
    Contr, //
    SettlDay, //
    ExpiryDay, //
    State, //
    LastLots, //
    LastTicks, //
    LastTime, //
    MaxOrderId, //
    MaxExecId //
  };

  MarketSet s;

  StmtPtr stmt{prepare(*db_, SelectMarketSql)};
  while (step(*stmt)) {
    s.insert(factory.newMarket(column<string_view>(*stmt, Mnem), //
                               column<string_view>(*stmt, Display), //
                               column<string_view>(*stmt, Contr), //
                               column<Jday>(*stmt, SettlDay), //
                               column<Jday>(*stmt, ExpiryDay), //
                               column<MarketState>(*stmt, State), //
                               column<Lots>(*stmt, LastLots), //
                               column<Ticks>(*stmt, LastTicks), //
                               column<Millis>(*stmt, LastTime), //
                               column<Iden>(*stmt, MaxOrderId), //
                               column<Iden>(*stmt, MaxExecId)));
  }
  return s;
}

TraderSet Model::doReadTrader(const Factory& factory) const
{
  enum { //
    Mnem, //
    Display, //
    Email //
  };

  TraderSet s;

  StmtPtr stmt{prepare(*db_, SelectTraderSql)};
  while (step(*stmt)) {
    s.insert(factory.newTrader(column<string_view>(*stmt, Mnem), //
                               column<string_view>(*stmt, Display), //
                               column<string_view>(*stmt, Email)));
  }
  return s;
}

} // sqlite

SWIRLY_API std::unique_ptr<Model> makeModel(const char* connString)
{
  return make_unique<sqlite::Model>(connString);
}

} // swirly
