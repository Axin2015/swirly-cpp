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
  header_[column::MarketId] = tr("MarketId");
  header_[column::Contr] = tr("Contr");
  header_[column::SettlDate] = tr("SettlDate");
  header_[column::Id] = tr("Id");
  header_[column::Accnt] = tr("Accnt");
  header_[column::Ref] = tr("Ref");
  header_[column::State] = tr("State");
  header_[column::Side] = tr("Side");
  header_[column::Lots] = tr("Lots");
  header_[column::Ticks] = tr("Ticks");
  header_[column::Resd] = tr("Resd");
  header_[column::Exec] = tr("Exec");
  header_[column::Cost] = tr("Cost");
  header_[column::LastLots] = tr("LastLots");
  header_[column::LastTicks] = tr("LastTicks");
  header_[column::MinLots] = tr("MinLots");
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
    const auto& order = rows_.nth(index.row())->second;
    switch (index.column()) {
    case column::MarketId:
      var = order.marketId();
      break;
    case column::Contr:
      var = order.contr();
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
    case column::Ticks:
      var = order.ticks();
      break;
    case column::Resd:
      var = order.resd();
      break;
    case column::Exec:
      var = order.exec();
      break;
    case column::Cost:
      var = order.cost();
      break;
    case column::LastLots:
      var = order.lastLots();
      break;
    case column::LastTicks:
      var = order.lastTicks();
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
