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
#include "TradeModel.hpp"

#include <algorithm>

using namespace std;

namespace swirly {
namespace ui {
using namespace exec;

TradeModel::TradeModel(QObject* parent) : QAbstractTableModel{parent}
{
  header_[column::MarketId] = tr("Market Id");
  header_[column::Contr] = tr("Contr");
  header_[column::SettlDate] = tr("Settl Date");
  header_[column::Id] = tr("Id");
  header_[column::OrderId] = tr("Order Id");
  header_[column::Accnt] = tr("Accnt");
  header_[column::Ref] = tr("Ref");
  header_[column::State] = tr("State");
  header_[column::Side] = tr("Side");
  header_[column::Lots] = tr("Lots");
  header_[column::Price] = tr("Price");
  header_[column::Resd] = tr("Resd");
  header_[column::Exec] = tr("Exec");
  header_[column::AvgPrice] = tr("Avg Price");
  header_[column::LastLots] = tr("Last Lots");
  header_[column::LastPrice] = tr("Last Price");
  header_[column::MinLots] = tr("Min Lots");
  header_[column::MatchId] = tr("Match Id");
  header_[column::LiqInd] = tr("Liq Ind");
  header_[column::Cpty] = tr("Cpty");
  header_[column::Created] = tr("Created");
}

TradeModel::~TradeModel() noexcept = default;

int TradeModel::rowCount(const QModelIndex& parent) const
{
  return rows_.size();
}

int TradeModel::columnCount(const QModelIndex& parent) const
{
  return column::Count;
}

QVariant TradeModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid()) {
    return QVariant{};
  }

  if (role == Qt::TextAlignmentRole) {
    return QVariant{Qt::AlignLeft | Qt::AlignVCenter};
  }

  if (role == Qt::UserRole) {
    return QVariant::fromValue(rows_.nth(index.row())->second);
  }

  QVariant var;
  if (role == Qt::DisplayRole) {
    const auto& trade = rows_.nth(index.row())->second;
    switch (index.column()) {
    case column::MarketId:
      var = trade.marketId();
      break;
    case column::Contr:
      var = trade.contr().mnem();
      break;
    case column::SettlDate:
      var = trade.settlDate();
      break;
    case column::Id:
      var = trade.id();
      break;
    case column::OrderId:
      var = trade.orderId();
      break;
    case column::Accnt:
      var = trade.accnt();
      break;
    case column::Ref:
      var = trade.ref();
      break;
    case column::State:
      var = enumString(trade.state());
      break;
    case column::Side:
      var = enumString(trade.side());
      break;
    case column::Lots:
      var = trade.lots();
      break;
    case column::Price:
      var = ticksToPriceString(trade.ticks(), trade.contr());
      break;
    case column::Resd:
      var = trade.resd();
      break;
    case column::Exec:
      var = trade.exec();
      break;
    case column::AvgPrice:
      var = ticksToAvgPriceString(trade.exec(), trade.cost(), trade.contr());
      break;
    case column::LastLots:
      var = trade.lastLots();
      break;
    case column::LastPrice:
      var = ticksToPriceString(trade.lastTicks(), trade.contr());
      break;
    case column::MinLots:
      var = trade.minLots();
      break;
    case column::MatchId:
      var = trade.matchId();
      break;
    case column::LiqInd:
      var = enumString(trade.liqInd());
      break;
    case column::Cpty:
      var = trade.cpty();
      break;
    case column::Created:
      var = trade.created();
      break;
    }
  }
  return var;
}

QVariant TradeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
    return QVariant{};
  }
  return header_[section];
}

void TradeModel::updateRow(const Exec& trade)
{
  const auto key = make_pair(trade.marketId(), trade.id());
  auto it = rows_.lower_bound(key);
  const int i = distance(rows_.begin(), it);

  const bool found{it != rows_.end() && !rows_.key_comp()(key, it->first)};
  if (found) {
    it->second = trade;
    emit dataChanged(index(i, 0), index(i, column::Count - 1));
  } else {
    // If not found then insert.
    beginInsertRows(QModelIndex{}, i, i);
    rows_.emplace_hint(it, key, trade);
    endInsertRows();
  }
}

} // ui
} // swirly
