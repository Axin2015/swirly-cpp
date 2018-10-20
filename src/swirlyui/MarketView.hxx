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
#ifndef SWIRLYUI_MARKETVIEW_HXX
#define SWIRLYUI_MARKETVIEW_HXX

#include "Types.hxx"

#include <QWidget>

class QModelIndex;

namespace swirly {
namespace ui {

class Product;
class ProductModel;
class Market;
class MarketForm;
class MarketModel;

class MarketView : public QWidget {
    Q_OBJECT

  public:
    MarketView(ProductModel& product_model, MarketModel& model, QWidget* parent = nullptr,
               Qt::WindowFlags f = Qt::WindowFlags{});
    ~MarketView() override;

    void set_fields(const QString& product_symbol, QDate settl_date, std::optional<Lots> lots,
                    std::optional<Ticks> ticks);

  signals:
    void create_market(const Product& product, QDate settl_date);
    void create_order(const Product& product, QDate settl_date, const QString& ref, Side side,
                      Lots lots, Ticks ticks);

  private slots:
    void slot_clicked(const QModelIndex& index);

  private:
    MarketModel& model_;
    MarketForm* market_form_{nullptr};
};

} // namespace ui
} // namespace swirly

#endif // SWIRLYUI_MARKETVIEW_HXX
