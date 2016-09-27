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
#ifndef SWIRLYUI_ORDERMODEL_HPP
#define SWIRLYUI_ORDERMODEL_HPP

#include "Order.hpp"

#include <QAbstractTableModel>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <boost/container/flat_map.hpp>
#pragma GCC diagnostic pop

namespace swirly {
namespace ui {

class OrderModel : public QAbstractTableModel {
 public:
  OrderModel(QObject* parent = nullptr);
  ~OrderModel() noexcept = default;

  int rowCount(const QModelIndex& parent) const override;

  int columnCount(const QModelIndex& parent) const override;

  QVariant data(const QModelIndex& index, int role) const override;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  void updateRow(const Order& order);

 private:
  enum { Columns = 18 };
  QVariant header_[Columns];
  using Key = std::pair<Id64, Id64>;
  boost::container::flat_map<Key, Order, std::less<Key>> rows_;
};

} // ui
} // swirly

#endif // SWIRLYUI_ORDERMODEL_HPP
