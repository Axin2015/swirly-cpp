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
#ifndef SWIRLYUI_MAINWINDOW_HXX
#define SWIRLYUI_MAINWINDOW_HXX

#include "HttpClient.hxx"

#include <QMainWindow>

namespace swirly {
namespace ui {

class AssetView;
class ProductView;
class MarketView;

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow();
    ~MainWindow() override;

  protected:
    void closeEvent(QCloseEvent* event) override;

  private slots:
    void slot_ref_data_complete();
    void slot_service_error(const QString& error);
    void slot_create_market(const Product& product, QDate settl_date);
    void slot_create_order(const Product& product, QDate settl_date, const QString& ref, Side side,
                           Lots lots, Ticks ticks);
    void slot_cancel_orders(const OrderKeys& keys);
    void slot_about();

  private:
    void create_actions();
    void create_status_bar();
    void read_settings();
    void write_settings();
    bool can_close();

    HttpClient client_;
    AssetView* asset_view_{nullptr};
    ProductView* product_view_{nullptr};
    MarketView* market_view_{nullptr};
};

} // namespace ui
} // namespace swirly

#endif // SWIRLYUI_MAINWINDOW_HXX
