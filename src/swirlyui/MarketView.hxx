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
#ifndef SWIRLYUI_MARKETVIEW_HXX
#define SWIRLYUI_MARKETVIEW_HXX

#include "Types.hxx"

#include <QWidget>

class QModelIndex;

namespace swirly {
namespace ui {

class Contr;
class ContrModel;
class Market;
class MarketForm;
class MarketModel;

class MarketView : public QWidget {
    Q_OBJECT

  public:
    MarketView(ContrModel& contrModel, MarketModel& model, QWidget* parent = nullptr,
               Qt::WindowFlags f = Qt::WindowFlags{});
    ~MarketView() noexcept override;

    void setFields(const QString& contrMnem, QDate settlDate, std::optional<Lots> lots,
                   std::optional<Ticks> ticks);

  signals:
    void createMarket(const Contr& contr, QDate settlDate);
    void createOrder(const Contr& contr, QDate settlDate, const QString& ref, Side side, Lots lots,
                     Ticks ticks);

  private slots:
    void slotClicked(const QModelIndex& index);

  private:
    MarketModel& model_;
    MarketForm* marketForm_{nullptr};
};

} // ui
} // swirly

#endif // SWIRLYUI_MARKETVIEW_HXX
