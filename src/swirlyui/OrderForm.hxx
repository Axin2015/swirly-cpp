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
#ifndef SWIRLYUI_ORDERFORM_HXX
#define SWIRLYUI_ORDERFORM_HXX

#include "Types.hxx"

#include <QWidget>

namespace swirly {
namespace ui {

class OrderModel;

class OrderForm : public QWidget {
    Q_OBJECT

  public:
    OrderForm(OrderModel& order_model, QWidget* parent = nullptr,
              Qt::WindowFlags f = Qt::WindowFlags{});
    ~OrderForm() override;

  signals:
    void cancel_orders(const OrderKeys& keys);

  private slots:
    void slot_select_all_clicked();
    void slot_select_none_clicked();
    void slot_cancel_orders_clicked();

  private:
    OrderModel& order_model_;
};

} // namespace ui
} // namespace swirly

#endif // SWIRLYUI_ORDERFORM_HXX
