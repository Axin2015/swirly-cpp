/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#include "MainWindow.hpp"

#include "AssetView.hpp"
#include "ContrView.hpp"
#include "ExecView.hpp"
#include "MarketView.hpp"
#include "OrderView.hpp"
#include "PosnView.hpp"
#include "TradeView.hpp"

#include <QtWidgets>

namespace swirly {
namespace ui {

MainWindow::MainWindow() : splitter_{new QSplitter{Qt::Vertical}}
{
  auto* const topTabs{new QTabWidget{}};
  topTabs->addTab(new AssetView{client_.assetModel()}, tr("Asset"));
  topTabs->addTab(new ContrView{client_.contrModel()}, tr("Contr"));
  topTabs->addTab(new MarketView{client_.marketModel()}, tr("Market"));
  topTabs->setCurrentIndex(2);

  auto* const bottomTabs{new QTabWidget{}};
  bottomTabs->addTab(new OrderView{client_.orderModel()}, tr("Order"));
  bottomTabs->addTab(new ExecView{client_.execModel()}, tr("Exec"));
  bottomTabs->addTab(new TradeView{client_.tradeModel()}, tr("Trade"));
  bottomTabs->addTab(new PosnView{client_.posnModel()}, tr("Posn"));

  splitter_->addWidget(topTabs);
  splitter_->addWidget(bottomTabs);

  setCentralWidget(splitter_);

  connect(&client_, &HttpClient::refDataComplete, this, &MainWindow::slotRefDataComplete);
  connect(&client_, &HttpClient::serviceError, this, &MainWindow::slotServiceError);

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
}

void MainWindow::slotServiceError(const QString& error)
{
  qDebug() << "slotServiceError:" << error;
}

void MainWindow::slotAbout()
{
  qDebug() << "slotFinished";
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
    move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
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

} // ui
} // swirly
