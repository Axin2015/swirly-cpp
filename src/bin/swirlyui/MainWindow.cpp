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

#include "ContrView.hpp"

#include <QtWidgets>

namespace swirly {
namespace ui {

MainWindow::~MainWindow() noexcept = default;

MainWindow::MainWindow() : contrView_{new ContrView{contrModel_}}
{
  setCentralWidget(contrView_);

  connect(&client_, &HttpClient::notifyAsset, this, &MainWindow::slotNotifyAsset);
  connect(&client_, &HttpClient::notifyContr, this, &MainWindow::slotNotifyContr);
  connect(&client_, &HttpClient::notifyError, this, &MainWindow::slotNotifyError);

  createActions();
  createStatusBar();

  readSettings();

  setUnifiedTitleAndToolBarOnMac(true);
}

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

void MainWindow::slotNotifyAsset(const Asset& asset)
{
  qDebug() << "slotNotifyAsset";
  assetModel_.insertRow(asset);
}

void MainWindow::slotNotifyContr(const Contr& contr)
{
  qDebug() << "slotNotifyContr";
  contrModel_.insertRow(contr);
}

void MainWindow::slotNotifyError(const QString& error)
{
  qDebug() << "slotNotifyError:" << error;
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
