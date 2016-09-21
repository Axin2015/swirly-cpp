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
#ifndef SWIRLYUI_MAINWINDOW_HPP
#define SWIRLYUI_MAINWINDOW_HPP

#include "AssetModel.hpp"
#include "ContrModel.hpp"
#include "HttpClient.hpp"

#include <QMainWindow>

namespace swirly {
namespace ui {

class ContrView;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow();
  ~MainWindow() noexcept;

 protected:
  void closeEvent(QCloseEvent* event) override;

 private slots:
  void slotNotifyAsset(const Asset& asset);
  void slotNotifyContr(const Contr& contr);
  void slotNotifyError(const QString& error);
  void slotAbout();

 private:
  void createActions();
  void createStatusBar();
  void readSettings();
  void writeSettings();
  bool canClose();

  HttpClient client_;
  AssetModel assetModel_;
  ContrModel contrModel_;
  ContrView* const contrView_;
};

} // ui
} // swirly

#endif // SWIRLYUI_MAINWINDOW_HPP