/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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

using namespace std;

namespace swirly {
namespace ui {
using namespace contr;

ContrModel::ContrModel(QObject* parent) : TableModel{parent}
{
  header_[unbox(Column::CheckState)] = tr("");
  header_[unbox(Column::Mnem)] = tr("Mnem");
  header_[unbox(Column::Display)] = tr("Display");
  header_[unbox(Column::Asset)] = tr("Asset");
  header_[unbox(Column::Ccy)] = tr("Ccy");
  header_[unbox(Column::LotNumer)] = tr("Lot Numer");
  header_[unbox(Column::LotDenom)] = tr("Lot Denom");
  header_[unbox(Column::TickNumer)] = tr("Tick Numer");
  header_[unbox(Column::TickDenom)] = tr("Tick Denom");
  header_[unbox(Column::PipDp)] = tr("Pip Dp");
  header_[unbox(Column::MinLots)] = tr("Min Lots");
  header_[unbox(Column::MaxLots)] = tr("Max Lots");
}

ContrModel::~ContrModel() noexcept = default;

QVariant ContrModel::data(const QModelIndex& index, int role) const
{
  QVariant var{};
  if (!index.isValid()) {
    // No-op.
  } else if (role == Qt::CheckStateRole) {
    const auto& row = rowAt(index.row());
    switch (box<Column>(index.column())) {
    case Column::CheckState:
      var = row.checked() ? Qt::Checked : Qt::Unchecked;
      break;
    default:
      break;
    }
  } else if (role == Qt::DisplayRole) {
    const auto& contr = valueAt(index.row());
    switch (box<Column>(index.column())) {
    case Column::CheckState:
      break;
    case Column::Mnem:
      var = contr.mnem();
      break;
    case Column::Display:
      var = contr.display();
      break;
    case Column::Asset:
      var = contr.asset();
      break;
    case Column::Ccy:
      var = contr.ccy();
      break;
    case Column::LotNumer:
      var = contr.lotNumer();
      break;
    case Column::LotDenom:
      var = contr.lotDenom();
      break;
    case Column::TickNumer:
      var = contr.tickNumer();
      break;
    case Column::TickDenom:
      var = contr.tickDenom();
      break;
    case Column::PipDp:
      var = contr.pipDp();
      break;
    case Column::MinLots:
      var = toVariant(contr.minLots());
      break;
    case Column::MaxLots:
      var = toVariant(contr.maxLots());
      break;
    }
  } else if (role == Qt::TextAlignmentRole) {
    switch (box<Column>(index.column())) {
    case Column::CheckState:
      break;
    case Column::Mnem:
    case Column::Display:
    case Column::Asset:
    case Column::Ccy:
      var = QVariant{Qt::AlignLeft | Qt::AlignVCenter};
      break;
    case Column::LotNumer:
    case Column::LotDenom:
    case Column::TickNumer:
    case Column::TickDenom:
    case Column::PipDp:
    case Column::MinLots:
    case Column::MaxLots:
      var = QVariant{Qt::AlignRight | Qt::AlignVCenter};
      break;
    }
  } else if (role == Qt::UserRole) {
    var = QVariant::fromValue(valueAt(index.row()));
  }
  return var;
}

QVariant ContrModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  QVariant var{};
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    var = header_[section];
  }
  return var;
}

Contr ContrModel::find(const QString& mnem) const
{
  Contr contr;
  auto it = rows_.find(mnem);
  if (it != rows_.end()) {
    contr = it->second.value();
  }
  return contr;
}

int ContrModel::indexOf(const QString& mnem) const
{
  int i{-1};
  auto it = rows_.find(mnem);
  if (it != rows_.end()) {
    i = distance(rows_.begin(), it);
  }
  return i;
}

} // ui
} // swirly
