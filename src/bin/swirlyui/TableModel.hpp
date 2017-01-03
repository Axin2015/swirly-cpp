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
#ifndef SWIRLYUI_TABLEMODEL_HPP
#define SWIRLYUI_TABLEMODEL_HPP

#include "Row.hpp"

#include <QAbstractTableModel>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/container/flat_map.hpp>
#pragma GCC diagnostic pop

namespace swirly {
namespace ui {

template <typename KeyT, typename ValueT, int CheckStateN, int ColumnCountN>
class TableModel : public QAbstractTableModel {
 public:
  TableModel(QObject* parent = nullptr) : QAbstractTableModel{parent} {}
  ~TableModel() noexcept override = default;

  int rowCount(const QModelIndex& parent) const override { return rows_.size(); }
  int columnCount(const QModelIndex& parent) const override { return ColumnCountN; }
  Qt::ItemFlags flags(const QModelIndex& index) const override
  {
    Qt::ItemFlags flags;
    if (index.isValid() && index.column() == CheckStateN) {
      flags = static_cast<Qt::ItemFlags>(Qt::ItemIsEnabled | Qt::ItemIsSelectable
                                         | Qt::ItemIsUserCheckable);
    } else {
      flags = QAbstractTableModel::flags(index);
    }
    return flags;
  }
  const ValueT& valueAt(int n) const { return rowAt(n).value(); }
  void reset()
  {
    beginResetModel();
    rows_.clear();
    endResetModel();
  }
  void sweep(std::uint64_t tag)
  {
    auto it = rows_.begin();
    while (it != rows_.end()) {
      if (it->second.tag() < tag) {
        const int row = distance(rows_.begin(), it);
        beginRemoveRows(QModelIndex{}, row, row);
        it = rows_.erase(it);
        endRemoveRows();
      } else {
        ++it;
      }
    }
  }
  void setChecked(bool checked = true)
  {
    if (!rows_.empty()) {
      for (auto& row : rows_) {
        row.second.setChecked(checked);
      }
      emit dataChanged(index(0, CheckStateN), index(rows_.size() - 1, CheckStateN));
    }
  }
  void toggleCheckState(int n)
  {
    auto& row = rowAt(n);
    row.setChecked(!row.checked());
    const auto i = index(n, CheckStateN);
    emit dataChanged(i, i);
  }

 protected:
  const Row<ValueT>& rowAt(int n) const { return rows_.nth(n)->second; }
  Row<ValueT>& rowAt(int n) { return rows_.nth(n)->second; }
  void removeRow(const KeyT& key)
  {
    auto it = rows_.find(key);
    if (it != rows_.end()) {
      const int row = distance(rows_.begin(), it);
      beginRemoveRows(QModelIndex{}, row, row);
      rows_.erase(it);
      endRemoveRows();
    }
  }
  void updateRow(const KeyT& key, uint64_t tag, const ValueT& val)
  {
    auto it = rows_.lower_bound(key);
    const int row = distance(rows_.begin(), it);

    const bool found{it != rows_.end() && !rows_.key_comp()(key, it->first)};
    if (found) {
      // Update tag to avoid removal during sweep phase.
      it->second.setTag(tag);
      if (isModified(it->second.value(), val)) {
        it->second.setValue(val);
        emit dataChanged(index(row, 0), index(row, ColumnCountN - 1));
      }
    } else {
      // If not found then insert.
      beginInsertRows(QModelIndex{}, row, row);
      rows_.emplace_hint(it, key, Row<ValueT>{tag, val});
      endInsertRows();
    }
  }

  boost::container::flat_map<KeyT, Row<ValueT>> rows_;
};

} // ui
} // swirly

#endif // SWIRLYUI_TABLEMODEL_HPP
