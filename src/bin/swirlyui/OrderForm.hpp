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
#ifndef SWIRLYUI_ORDERFORM_HPP
#define SWIRLYUI_ORDERFORM_HPP

#include "Types.hpp"

#include <QWidget>

namespace swirly {
namespace ui {

class OrderModel;

class OrderForm : public QWidget {
    Q_OBJECT

  public:
    OrderForm(OrderModel& orderModel, QWidget* parent = nullptr,
              Qt::WindowFlags f = Qt::WindowFlags{});
    ~OrderForm() noexcept override;

  signals:
    void cancelOrders(const OrderKeys& keys);

  private slots:
    void slotSelectAllClicked();
    void slotSelectNoneClicked();
    void slotCancelOrdersClicked();

  private:
    OrderModel& orderModel_;
};

} // ui
} // swirly

#endif // SWIRLYUI_ORDERFORM_HPP
