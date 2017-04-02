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
#ifndef SWIRLYUI_CLIENT_HXX
#define SWIRLYUI_CLIENT_HXX

#include "AssetModel.hxx"
#include "ContrModel.hxx"
#include "ExecModel.hxx"
#include "MarketModel.hxx"
#include "OrderModel.hxx"
#include "PosnModel.hxx"
#include "TradeModel.hxx"

#include <QObject>

namespace swirly {
namespace ui {

class Client : public QObject {
    Q_OBJECT

  public:
    Client(QObject* parent = nullptr);
    ~Client() noexcept = default;

    const AssetModel& assetModel() const noexcept { return assetModel_; }
    const ContrModel& contrModel() const noexcept { return contrModel_; }
    const MarketModel& marketModel() const noexcept { return marketModel_; }
    const OrderModel& orderModel() const noexcept { return orderModel_; }
    const ExecModel& execModel() const noexcept { return execModel_; }
    const TradeModel& tradeModel() const noexcept { return tradeModel_; }
    const PosnModel& posnModel() const noexcept { return posnModel_; }

    AssetModel& assetModel() noexcept { return assetModel_; }
    ContrModel& contrModel() noexcept { return contrModel_; }
    MarketModel& marketModel() noexcept { return marketModel_; }
    OrderModel& orderModel() noexcept { return orderModel_; }
    ExecModel& execModel() noexcept { return execModel_; }
    TradeModel& tradeModel() noexcept { return tradeModel_; }
    PosnModel& posnModel() noexcept { return posnModel_; }

    virtual void createMarket(const Contr& contr, QDate settlDate) = 0;
    virtual void createOrder(const Contr& contr, QDate settlDate, const QString& ref, Side side,
                             Lots lots, Ticks ticks)
        = 0;
    virtual void cancelOrders(const OrderKeys& keys) = 0;

  signals:
    void refDataComplete();
    void serviceError(const QString& error);

  protected:
    void reset();

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

#endif // SWIRLYUI_CLIENT_HXX
