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
#include <swirly/elm/Factory.hpp>
#include <swirly/elm/Market.hpp>
#include <swirly/elm/Trader.hpp>

#include <swirly/ash/Conf.hpp>

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

Model::Model(const Conf& conf)
  : db_{openDb(conf.get("sqlite_model", "swirly.db"), SQLITE_OPEN_READONLY, conf)}
{
}

Model::~Model() noexcept = default;

Model::Model(Model&&) = default;

Model& Model::operator=(Model&&) = default;

void Model::doReadAsset(const Factory& factory, const ModelCallback<AssetPtr>& cb) const
{
  enum {
    Mnem, //
    Display, //
    TypeId //
  };

  StmtPtr stmt{prepare(*db_, SelectAssetSql)};
  while (step(*stmt)) {
    cb(factory.newAsset(column<string_view>(*stmt, Mnem), //
                        column<string_view>(*stmt, Display), //
                        column<AssetType>(*stmt, TypeId)));
  }
}

void Model::doReadContr(const Factory& factory, const ModelCallback<ContrPtr>& cb) const
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

  StmtPtr stmt{prepare(*db_, SelectContrSql)};
  while (step(*stmt)) {
    cb(factory.newContr(column<string_view>(*stmt, Mnem), //
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

void Model::doReadMarket(const Factory& factory, const ModelCallback<MarketPtr>& cb) const
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

  StmtPtr stmt{prepare(*db_, SelectMarketSql)};
  while (step(*stmt)) {
    cb(factory.newMarket(column<string_view>(*stmt, Mnem), //
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
}

void Model::doReadTrader(const Factory& factory, const ModelCallback<TraderPtr>& cb) const
{
  enum { //
    Mnem, //
    Display, //
    Email //
  };

  StmtPtr stmt{prepare(*db_, SelectTraderSql)};
  while (step(*stmt)) {
    cb(factory.newTrader(column<string_view>(*stmt, Mnem), //
                         column<string_view>(*stmt, Display), //
                         column<string_view>(*stmt, Email)));
  }
}

} // sqlite

SWIRLY_API std::unique_ptr<Model> makeModel(const Conf& conf)
{
  return make_unique<sqlite::Model>(conf);
}

} // swirly
