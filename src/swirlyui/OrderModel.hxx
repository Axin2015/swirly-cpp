/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
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
#ifndef SWIRLYUI_ORDERMODEL_HXX
#define SWIRLYUI_ORDERMODEL_HXX

#include "Order.hxx"
#include "TableModel.hxx"

namespace swirly {
namespace ui {

class OrderModel
: public TableModel<OrderKey, Order, unbox(order::Column::CheckState), order::ColumnCount> {
  public:
    OrderModel(QObject* parent = nullptr);
    ~OrderModel() override;

    QVariant data(const QModelIndex& index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    OrderKeys checked() const;

    void remove_row(const Order& order) { TableModel::remove_row(order.key()); }
    void update_row(std::uint64_t tag, const Order& order)
    {
        TableModel::update_row(order.key(), tag, order);
    }

  private:
    QVariant header_[order::ColumnCount];
};

} // namespace ui
} // namespace swirly

#endif // SWIRLYUI_ORDERMODEL_HXX
