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
#ifndef SWIRLYUI_MARKETFORM_HXX
#define SWIRLYUI_MARKETFORM_HXX

#include "Types.hxx"

#include <QDoubleValidator>
#include <QIntValidator>
#include <QWidget>

class QComboBox;
class QDateEdit;
class QLineEdit;

namespace swirly {
namespace ui {

class Product;
class ProductModel;

class MarketForm : public QWidget {
    Q_OBJECT

  public:
    MarketForm(ProductModel& product_model, QWidget* parent = nullptr,
               Qt::WindowFlags f = Qt::WindowFlags{});
    ~MarketForm() override;

    void set_fields(const QString& product_symbol, QDate settl_date, std::optional<Lots> lots,
                    std::optional<Ticks> ticks);

  signals:
    void create_market(const Product& product, QDate settl_date);
    void create_order(const Product& product, QDate settl_date, const QString& ref, Side side,
                      Lots lots, Ticks ticks);

  private slots:
    void slot_product_changed(int index);
    void slot_create_clicked();
    void slot_buy_or_sell_clicked(Side side);

  private:
    ProductModel& product_model_;
    QComboBox* product_combo_box_{nullptr};
    QDateEdit* settl_date_edit_{nullptr};
    QLineEdit* lots_edit_{nullptr};
    QLineEdit* price_edit_{nullptr};
    QIntValidator lots_validator_;
    QDoubleValidator price_validator_;
};

} // namespace ui
} // namespace swirly

#endif // SWIRLYUI_MARKETFORM_HXX
