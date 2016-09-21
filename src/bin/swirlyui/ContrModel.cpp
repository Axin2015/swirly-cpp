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

#include "ContrModel.hpp"

#include <algorithm>

using namespace std;

namespace swirly {
namespace ui {
namespace {

enum { //
  Mnem, //
  Display, //
  Asset, //
  Ccy, //
  LotNumer, //
  LotDenom, //
  TickNumer, //
  TickDenom, //
  PipDp, //
  MinLots, //
  MaxLots //
};

} // anonymous

ContrModel::ContrModel(QObject* parent) : QAbstractTableModel{parent}
{
  header_[Mnem] = tr("Mnem");
  header_[Display] = tr("Display");
  header_[Asset] = tr("Asset");
  header_[Ccy] = tr("Ccy");
  header_[LotNumer] = tr("LotNumer");
  header_[LotDenom] = tr("LotDenom");
  header_[TickNumer] = tr("TickNumer");
  header_[TickDenom] = tr("TickDenom");
  header_[PipDp] = tr("PipDp");
  header_[MinLots] = tr("MinLots");
  header_[MaxLots] = tr("MaxLots");
}

int ContrModel::rowCount(const QModelIndex& parent) const
{
  return rows_.size();
}

int ContrModel::columnCount(const QModelIndex& parent) const
{
  return Columns;
}

QVariant ContrModel::data(const QModelIndex& index, int role) const
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
    const auto& contr = rows_.nth(index.row())->second;
    switch (index.column()) {
    case Mnem:
      var = contr.mnem();
      break;
    case Display:
      var = contr.display();
      break;
    case Asset:
      var = contr.asset();
      break;
    case Ccy:
      var = contr.ccy();
      break;
    case LotNumer:
      var = contr.lotNumer();
      break;
    case LotDenom:
      var = contr.lotDenom();
      break;
    case TickNumer:
      var = contr.tickNumer();
      break;
    case TickDenom:
      var = contr.tickDenom();
      break;
    case PipDp:
      var = contr.pipDp();
      break;
    case MinLots:
      var = contr.minLots();
      break;
    case MaxLots:
      var = contr.maxLots();
      break;
    }
  }
  return var;
}

QVariant ContrModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
    return QVariant{};
  }
  return header_[section];
}

void ContrModel::insertRow(const Contr& contr)
{
  auto it = rows_.lower_bound(contr.mnem());
  const bool found{it != rows_.end() && !rows_.key_comp()(contr.mnem(), it->first)};
  if (!found) {
    // If not found then insert.
    const int i = distance(rows_.begin(), it);
    beginInsertRows(QModelIndex{}, i, i);
    rows_.emplace_hint(it, contr.mnem(), contr);
    endInsertRows();
  }
}

} // ui
} // swirly
