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
#include "AssetModel.hpp"

#include <algorithm>

using namespace std;

namespace swirly {
namespace ui {
using namespace asset;

AssetModel::AssetModel(QObject* parent) : QAbstractTableModel{parent}
{
  header_[column::Mnem] = tr("Mnem");
  header_[column::Display] = tr("Display");
  header_[column::Type] = tr("Type");
}

AssetModel::~AssetModel() noexcept = default;

int AssetModel::rowCount(const QModelIndex& parent) const
{
  return rows_.size();
}

int AssetModel::columnCount(const QModelIndex& parent) const
{
  return column::Count;
}

QVariant AssetModel::data(const QModelIndex& index, int role) const
{
  QVariant var{};
  if (!index.isValid()) {
    // No-op.
  } else if (role == Qt::DisplayRole) {
    const auto& asset = rows_.nth(index.row())->second;
    switch (index.column()) {
    case column::Mnem:
      var = asset.mnem();
      break;
    case column::Display:
      var = asset.display();
      break;
    case column::Type:
      var = enumString(asset.type());
      break;
    }
  } else if (role == Qt::TextAlignmentRole) {
    var = QVariant{Qt::AlignLeft | Qt::AlignVCenter};
  } else if (role == Qt::UserRole) {
    var = QVariant::fromValue(rows_.nth(index.row())->second);
  }
  return var;
}

QVariant AssetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
    return QVariant{};
  }
  return header_[section];
}

void AssetModel::updateRow(const Asset& asset)
{
  auto it = rows_.lower_bound(asset.mnem());
  const int i = distance(rows_.begin(), it);

  const bool found{it != rows_.end() && !rows_.key_comp()(asset.mnem(), it->first)};
  if (found) {
    it->second = asset;
    emit dataChanged(index(i, 0), index(i, column::Count - 1));
  } else {
    // If not found then insert.
    beginInsertRows(QModelIndex{}, i, i);
    rows_.emplace_hint(it, asset.mnem(), asset);
    endInsertRows();
  }
}

} // ui
} // swirly
