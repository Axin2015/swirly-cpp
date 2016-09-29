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
#ifndef SWIRLYUI_CLIENT_HPP
#define SWIRLYUI_CLIENT_HPP

#include "AssetModel.hpp"
#include "ContrModel.hpp"
#include "ExecModel.hpp"
#include "MarketModel.hpp"
#include "OrderModel.hpp"
#include "PosnModel.hpp"
#include "TradeModel.hpp"

#include <QObject>

namespace swirly {
namespace ui {

class Client : public QObject {
  Q_OBJECT

 public:
  Client(QObject* parent = nullptr);
  ~Client() noexcept = default;

  AssetModel& assetModel() noexcept { return assetModel_; }
  ContrModel& contrModel() noexcept { return contrModel_; }
  MarketModel& marketModel() noexcept { return marketModel_; }
  OrderModel& orderModel() noexcept { return orderModel_; }
  ExecModel& execModel() noexcept { return execModel_; }
  TradeModel& tradeModel() noexcept { return tradeModel_; }
  PosnModel& posnModel() noexcept { return posnModel_; }

 protected:
 signals:
  void refDataComplete();
  void serviceError(const QString& error);

 private slots:

 private:
  AssetModel assetModel_;
  ContrModel contrModel_;
  MarketModel marketModel_;
  OrderModel orderModel_;
  ExecModel execModel_;
  TradeModel tradeModel_;
  PosnModel posnModel_;
};

} // ui
} // swirly

#endif // SWIRLYUI_CLIENT_HPP
