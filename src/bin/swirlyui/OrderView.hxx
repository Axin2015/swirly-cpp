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
#ifndef SWIRLYUI_ORDERVIEW_HXX
#define SWIRLYUI_ORDERVIEW_HXX

#include "Types.hxx"

#include <QWidget>

class QModelIndex;

namespace swirly {
namespace ui {

class Order;
class OrderModel;

class OrderView : public QWidget {
    Q_OBJECT

  public:
    explicit OrderView(OrderModel& model, QWidget* parent = nullptr,
                       Qt::WindowFlags f = Qt::WindowFlags{});
    ~OrderView() noexcept override;

  signals:
    void cancelOrders(const OrderKeys& keys);
    void setFields(const QString& contrMnem, QDate settlDate, std::optional<Lots> lots,
                   std::optional<Ticks> ticks);

  private slots:
    void slotClicked(const QModelIndex& index);

  private:
    OrderModel& model_;
};

} // ui
} // swirly

#endif // SWIRLYUI_ORDERVIEW_HXX
