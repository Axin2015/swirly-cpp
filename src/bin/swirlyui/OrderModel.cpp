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
#include "OrderModel.hpp"

#include <algorithm>

using namespace std;

namespace swirly {
namespace ui {
using namespace order;

OrderModel::OrderModel(QObject* parent) : QAbstractTableModel{parent}
{
  header_[column::MarketId] = tr("Market Id");
  header_[column::Contr] = tr("Contr");
  header_[column::SettlDate] = tr("Settl Date");
  header_[column::Id] = tr("Id");
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
  header_[column::Created] = tr("Created");
  header_[column::Modified] = tr("Modified");
}

OrderModel::~OrderModel() noexcept = default;

int OrderModel::rowCount(const QModelIndex& parent) const
{
  return rows_.size();
}

int OrderModel::columnCount(const QModelIndex& parent) const
{
  return column::Count;
}

QVariant OrderModel::data(const QModelIndex& index, int role) const
{
  QVariant var{};
  if (!index.isValid()) {
    // No-op.
  } else if (role == Qt::DisplayRole) {
    const auto& order = rows_.nth(index.row())->second;
    switch (index.column()) {
    case column::MarketId:
      var = order.marketId();
      break;
    case column::Contr:
      var = order.contr().mnem();
      break;
    case column::SettlDate:
      var = order.settlDate();
      break;
    case column::Id:
      var = order.id();
      break;
    case column::Accnt:
      var = order.accnt();
      break;
    case column::Ref:
      var = order.ref();
      break;
    case column::State:
      var = enumString(order.state());
      break;
    case column::Side:
      var = enumString(order.side());
      break;
    case column::Lots:
      var = order.lots();
      break;
    case column::Price:
      var = ticksToPriceString(order.ticks(), order.contr());
      break;
    case column::Resd:
      var = order.resd();
      break;
    case column::Exec:
      var = order.exec();
      break;
    case column::AvgPrice:
      var = ticksToAvgPriceString(order.exec(), order.cost(), order.contr());
      break;
    case column::LastLots:
      var = order.lastLots();
      break;
    case column::LastPrice:
      var = ticksToPriceString(order.lastTicks(), order.contr());
      break;
    case column::MinLots:
      var = order.minLots();
      break;
    case column::Created:
      var = order.created();
      break;
    case column::Modified:
      var = order.modified();
      break;
    }
  } else if (role == Qt::TextAlignmentRole) {
    var = QVariant{Qt::AlignLeft | Qt::AlignVCenter};
  } else if (role == Qt::UserRole) {
    var = QVariant::fromValue(rows_.nth(index.row())->second);
  }
  return var;
}

QVariant OrderModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
    return QVariant{};
  }
  return header_[section];
}

void OrderModel::updateRow(const Order& order)
{
  const auto key = make_pair(order.marketId(), order.id());
  auto it = rows_.lower_bound(key);
  const int i = distance(rows_.begin(), it);

  const bool found{it != rows_.end() && !rows_.key_comp()(key, it->first)};
  if (found) {
    it->second = order;
    emit dataChanged(index(i, 0), index(i, column::Count - 1));
  } else {
    // If not found then insert.
    beginInsertRows(QModelIndex{}, i, i);
    rows_.emplace_hint(it, key, order);
    endInsertRows();
  }
}

} // ui
} // swirly
