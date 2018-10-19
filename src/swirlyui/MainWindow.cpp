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
#include "MainWindow.hxx"

#include "AssetView.hxx"
#include "ExecView.hxx"
#include "MarketView.hxx"
#include "OrderView.hxx"
#include "PosnView.hxx"
#include "ProductView.hxx"
#include "TradeView.hxx"

#include <QtWidgets>

#include <memory>

namespace swirly {
namespace ui {
using namespace std;

MainWindow::MainWindow()
{
    connect(&client_, &HttpClient::ref_data_complete, this, &MainWindow::slot_ref_data_complete);
    connect(&client_, &HttpClient::service_error, this, &MainWindow::slot_service_error);

    auto market_view = make_unique<MarketView>(client_.product_model(), client_.market_model());
    connect(market_view.get(), &MarketView::create_market, this, &MainWindow::slot_create_market);
    connect(market_view.get(), &MarketView::create_order, this, &MainWindow::slot_create_order);

    auto order_view = make_unique<OrderView>(client_.order_model());
    connect(order_view.get(), &OrderView::cancel_orders, this, &MainWindow::slot_cancel_orders);
    connect(order_view.get(), &OrderView::set_fields, market_view.get(), &MarketView::set_fields);

    auto exec_view = make_unique<ExecView>(client_.exec_model());
    connect(exec_view.get(), &ExecView::set_fields, market_view.get(), &MarketView::set_fields);

    auto trade_view = make_unique<TradeView>(client_.trade_model());
    connect(trade_view.get(), &TradeView::set_fields, market_view.get(), &MarketView::set_fields);

    auto posn_view = make_unique<PosnView>(client_.posn_model());
    connect(posn_view.get(), &PosnView::set_fields, market_view.get(), &MarketView::set_fields);

    auto top_tabs = make_unique<QTabWidget>();
    top_tabs->addTab(asset_view_ = new AssetView{client_.asset_model()}, tr("Asset"));
    top_tabs->addTab(product_view_ = new ProductView{client_.product_model()}, tr("Product"));
    top_tabs->addTab(market_view_ = market_view.release(), tr("Market"));
    top_tabs->setCurrentIndex(2);

    auto bottom_tabs = make_unique<QTabWidget>();
    bottom_tabs->addTab(order_view.release(), tr("Order"));
    bottom_tabs->addTab(exec_view.release(), tr("Exec"));
    bottom_tabs->addTab(trade_view.release(), tr("Trade"));
    bottom_tabs->addTab(posn_view.release(), tr("Posn"));

    auto splitter = make_unique<QSplitter>(Qt::Vertical);
    splitter->addWidget(top_tabs.release());
    splitter->addWidget(bottom_tabs.release());

    setCentralWidget(splitter.release());

    create_actions();
    create_status_bar();

    read_settings();

    setUnifiedTitleAndToolBarOnMac(true);
}

MainWindow::~MainWindow() = default;

void MainWindow::closeEvent(QCloseEvent* event)
{
    qDebug() << "closeEvent";
    if (can_close()) {
        write_settings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::slot_ref_data_complete()
{
    qDebug() << "slot_ref_data_complete";
    asset_view_->resize_columns_to_contents();
    product_view_->resize_columns_to_contents();
}

void MainWindow::slot_service_error(const QString& error)
{
    qDebug().nospace() << "slot_service_error: " << error;
}

void MainWindow::slot_create_market(const Product& product, QDate settl_date)
{
    qDebug().nospace() << "slot_create_market: product=" << product //
                       << ",settl_date=" << settl_date;
    client_.create_market(product, settl_date);
}

void MainWindow::slot_create_order(const Product& product, QDate settl_date, const QString& ref,
                                   Side side, Lots lots, Ticks ticks)
{
    qDebug().nospace() << "slot_create_order: product=" << product //
                       << ",settl_date=" << settl_date             //
                       << ",ref=" << ref                           //
                       << ",side=" << side                         //
                       << ",lots=" << lots                         //
                       << ",ticks=" << ticks;
    client_.create_order(product, settl_date, ref, side, lots, ticks);
}

void MainWindow::slot_cancel_orders(const OrderKeys& keys)
{
    qDebug() << "slot_cancel_orders";
    client_.cancel_orders(keys);
}

void MainWindow::slot_about()
{
    qDebug() << "slot_about";
    QMessageBox::about(this, tr("About Swiry UI"), tr("The Swirly UI."));
}

void MainWindow::create_actions()
{
    auto* const file_menu = menuBar()->addMenu(tr("&File"));

    const auto exit_icon = QIcon::fromTheme("application-exit");

    auto* const exit_act = file_menu->addAction(exit_icon, tr("E&xit"), this, &QWidget::close);
    exit_act->setShortcuts(QKeySequence::Quit);
    exit_act->setStatusTip(tr("Exit the application"));

    auto* const help_menu = menuBar()->addMenu(tr("&Help"));

    auto* const about_act = help_menu->addAction(tr("&About"), this, &MainWindow::slot_about);
    about_act->setStatusTip(tr("Show the application's About box"));

    auto* const about_qt_act = help_menu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    about_qt_act->setStatusTip(tr("Show the Qt library's About box"));
}

void MainWindow::create_status_bar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::read_settings()
{
    const QSettings settings{QCoreApplication::organizationName(),
                             QCoreApplication::applicationName()};
    const auto geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const auto available_geometry = QApplication::desktop()->availableGeometry(this);
        resize(available_geometry.width() / 3, available_geometry.height() / 2);
        move((available_geometry.width() - width()) / 2,
             (available_geometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::write_settings()
{
    QSettings settings{QCoreApplication::organizationName(), QCoreApplication::applicationName()};
    settings.setValue("geometry", saveGeometry());
}

bool MainWindow::can_close()
{
    return true;
}

} // namespace ui
} // namespace swirly
