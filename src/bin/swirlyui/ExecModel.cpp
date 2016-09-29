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
#include "ExecModel.hpp"

#include <algorithm>

using namespace std;

namespace swirly {
namespace ui {
using namespace exec;

ExecModel::ExecModel(QObject* parent) : QAbstractTableModel{parent}
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
  header_[column::Ticks] = tr("Ticks");
  header_[column::Resd] = tr("Resd");
  header_[column::Exec] = tr("Exec");
  header_[column::Cost] = tr("Cost");
  header_[column::LastLots] = tr("Last Lots");
  header_[column::LastTicks] = tr("Last Ticks");
  header_[column::MinLots] = tr("Min Lots");
  header_[column::MatchId] = tr("Match Id");
  header_[column::LiqInd] = tr("Liq Ind");
  header_[column::Cpty] = tr("Cpty");
  header_[column::Created] = tr("Created");
}

ExecModel::~ExecModel() noexcept = default;

int ExecModel::rowCount(const QModelIndex& parent) const
{
  return rows_.size();
}

int ExecModel::columnCount(const QModelIndex& parent) const
{
  return column::Count;
}

QVariant ExecModel::data(const QModelIndex& index, int role) const
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
    const auto& exec = rows_.nth(index.row())->second;
    switch (index.column()) {
    case column::MarketId:
      var = exec.marketId();
      break;
    case column::Contr:
      var = exec.contr().mnem();
      break;
    case column::SettlDate:
      var = exec.settlDate();
      break;
    case column::Id:
      var = exec.id();
      break;
    case column::OrderId:
      var = exec.orderId();
      break;
    case column::Accnt:
      var = exec.accnt();
      break;
    case column::Ref:
      var = exec.ref();
      break;
    case column::State:
      var = enumString(exec.state());
      break;
    case column::Side:
      var = enumString(exec.side());
      break;
    case column::Lots:
      var = exec.lots();
      break;
    case column::Ticks:
      var = exec.ticks();
      break;
    case column::Resd:
      var = exec.resd();
      break;
    case column::Exec:
      var = exec.exec();
      break;
    case column::Cost:
      var = exec.cost();
      break;
    case column::LastLots:
      var = exec.lastLots();
      break;
    case column::LastTicks:
      var = exec.lastTicks();
      break;
    case column::MinLots:
      var = exec.minLots();
      break;
    case column::MatchId:
      var = exec.matchId();
      break;
    case column::LiqInd:
      var = enumString(exec.liqInd());
      break;
    case column::Cpty:
      var = exec.cpty();
      break;
    case column::Created:
      var = exec.created();
      break;
    }
  }
  return var;
}

QVariant ExecModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
    return QVariant{};
  }
  return header_[section];
}

void ExecModel::updateRow(const Exec& exec)
{
  const auto key = make_pair(exec.marketId(), exec.id());
  auto it = rows_.lower_bound(key);
  const int i = distance(rows_.begin(), it);

  const bool found{it != rows_.end() && !rows_.key_comp()(key, it->first)};
  if (found) {
    it->second = exec;
    emit dataChanged(index(i, 0), index(i, column::Count - 1));
  } else {
    // If not found then insert.
    beginInsertRows(QModelIndex{}, i, i);
    rows_.emplace_hint(it, key, exec);
    endInsertRows();
  }
}

} // ui
} // swirly
