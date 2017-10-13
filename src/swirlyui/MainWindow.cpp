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
#include "MainWindow.hxx"

#include "AssetView.hxx"
#include "ExecView.hxx"
#include "InstrView.hxx"
#include "MarketView.hxx"
#include "OrderView.hxx"
#include "PosnView.hxx"
#include "TradeView.hxx"

#include <QtWidgets>

#include <memory>

using namespace std;

namespace swirly {
namespace ui {

MainWindow::MainWindow()
{
    connect(&client_, &HttpClient::refDataComplete, this, &MainWindow::slotRefDataComplete);
    connect(&client_, &HttpClient::serviceError, this, &MainWindow::slotServiceError);

    auto marketView = make_unique<MarketView>(client_.instrModel(), client_.marketModel());
    connect(marketView.get(), &MarketView::createMarket, this, &MainWindow::slotCreateMarket);
    connect(marketView.get(), &MarketView::createOrder, this, &MainWindow::slotCreateOrder);

    auto orderView = make_unique<OrderView>(client_.orderModel());
    connect(orderView.get(), &OrderView::cancelOrders, this, &MainWindow::slotCancelOrders);
    connect(orderView.get(), &OrderView::setFields, marketView.get(), &MarketView::setFields);

    auto execView = make_unique<ExecView>(client_.execModel());
    connect(execView.get(), &ExecView::setFields, marketView.get(), &MarketView::setFields);

    auto tradeView = make_unique<TradeView>(client_.tradeModel());
    connect(tradeView.get(), &TradeView::setFields, marketView.get(), &MarketView::setFields);

    auto posnView = make_unique<PosnView>(client_.posnModel());
    connect(posnView.get(), &PosnView::setFields, marketView.get(), &MarketView::setFields);

    auto topTabs = make_unique<QTabWidget>();
    topTabs->addTab(assetView_ = new AssetView{client_.assetModel()}, tr("Asset"));
    topTabs->addTab(instrView_ = new InstrView{client_.instrModel()}, tr("Instr"));
    topTabs->addTab(marketView_ = marketView.release(), tr("Market"));
    topTabs->setCurrentIndex(2);

    auto bottomTabs = make_unique<QTabWidget>();
    bottomTabs->addTab(orderView.release(), tr("Order"));
    bottomTabs->addTab(execView.release(), tr("Exec"));
    bottomTabs->addTab(tradeView.release(), tr("Trade"));
    bottomTabs->addTab(posnView.release(), tr("Posn"));

    auto splitter = make_unique<QSplitter>(Qt::Vertical);
    splitter->addWidget(topTabs.release());
    splitter->addWidget(bottomTabs.release());

    setCentralWidget(splitter.release());

    createActions();
    createStatusBar();

    readSettings();

    setUnifiedTitleAndToolBarOnMac(true);
}

MainWindow::~MainWindow() noexcept = default;

void MainWindow::closeEvent(QCloseEvent* event)
{
    qDebug() << "closeEvent";
    if (canClose()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::slotRefDataComplete()
{
    qDebug() << "slotRefDataComplete";
    assetView_->resizeColumnsToContents();
    instrView_->resizeColumnsToContents();
}

void MainWindow::slotServiceError(const QString& error)
{
    qDebug().nospace() << "slotServiceError: " << error;
}

void MainWindow::slotCreateMarket(const Instr& instr, QDate settlDate)
{
    qDebug().nospace() << "slotCreateMarket: instr=" << instr //
                       << ",settlDate=" << settlDate;
    client_.createMarket(instr, settlDate);
}

void MainWindow::slotCreateOrder(const Instr& instr, QDate settlDate, const QString& ref, Side side,
                                 Lots lots, Ticks ticks)
{
    qDebug().nospace() << "slotCreateOrder: instr=" << instr //
                       << ",settlDate=" << settlDate //
                       << ",ref=" << ref //
                       << ",side=" << side //
                       << ",lots=" << lots //
                       << ",ticks=" << ticks;
    client_.createOrder(instr, settlDate, ref, side, lots, ticks);
}

void MainWindow::slotCancelOrders(const OrderKeys& keys)
{
    qDebug() << "slotCancelOrders";
    client_.cancelOrders(keys);
}

void MainWindow::slotAbout()
{
    qDebug() << "slotAbout";
    QMessageBox::about(this, tr("About Swiry UI"), tr("The Swirly UI."));
}

void MainWindow::createActions()
{
    auto* const fileMenu = menuBar()->addMenu(tr("&File"));

    const auto exitIcon = QIcon::fromTheme("application-exit");

    auto* const exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    auto* const helpMenu = menuBar()->addMenu(tr("&Help"));

    auto* const aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::slotAbout);
    aboutAct->setStatusTip(tr("Show the application's About box"));

    auto* const aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    const QSettings settings{QCoreApplication::organizationName(),
                             QCoreApplication::applicationName()};
    const auto geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const auto availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings{QCoreApplication::organizationName(), QCoreApplication::applicationName()};
    settings.setValue("geometry", saveGeometry());
}

bool MainWindow::canClose()
{
    return true;
}

} // namespace ui
} // namespace swirly
