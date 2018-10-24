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
#ifndef SWIRLYUI_CLIENT_HXX
#define SWIRLYUI_CLIENT_HXX

#include "AssetModel.hxx"
#include "ExecModel.hxx"
#include "InstrModel.hxx"
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
    ~Client() override = default;

    const AssetModel& asset_model() const noexcept { return asset_model_; }
    const InstrModel& instr_model() const noexcept { return instr_model_; }
    const MarketModel& market_model() const noexcept { return market_model_; }
    const OrderModel& order_model() const noexcept { return order_model_; }
    const ExecModel& exec_model() const noexcept { return exec_model_; }
    const TradeModel& trade_model() const noexcept { return trade_model_; }
    const PosnModel& posn_model() const noexcept { return posn_model_; }

    AssetModel& asset_model() noexcept { return asset_model_; }
    InstrModel& instr_model() noexcept { return instr_model_; }
    MarketModel& market_model() noexcept { return market_model_; }
    OrderModel& order_model() noexcept { return order_model_; }
    ExecModel& exec_model() noexcept { return exec_model_; }
    TradeModel& trade_model() noexcept { return trade_model_; }
    PosnModel& posn_model() noexcept { return posn_model_; }

    virtual void create_market(const Instr& instr, QDate settl_date) = 0;
    virtual void create_order(const Instr& instr, QDate settl_date, const QString& ref, Side side,
                              Lots lots, Ticks ticks)
        = 0;
    virtual void cancel_orders(const OrderKeys& keys) = 0;

  signals:
    void ref_data_complete();
    void service_error(const QString& error);

  protected:
    void reset();

  private slots:

  private:
    AssetModel asset_model_;
    InstrModel instr_model_;
    MarketModel market_model_;
    OrderModel order_model_;
    ExecModel exec_model_;
    TradeModel trade_model_;
    PosnModel posn_model_;
};

} // namespace ui
} // namespace swirly

#endif // SWIRLYUI_CLIENT_HXX
