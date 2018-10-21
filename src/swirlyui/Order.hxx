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
#ifndef SWIRLYUI_ORDER_HXX
#define SWIRLYUI_ORDER_HXX

#include "Instr.hxx"

namespace swirly {
namespace ui {
namespace order {

enum class Column : int { //
    CheckState,           //
    Created,              //
    Modified,             //
    Accnt,                //
    MarketId,             //
    Instr,                //
    SettlDate,            //
    Id,                   //
    Ref,                  //
    State,                //
    Side,                 //
    Lots,                 //
    Price,                //
    ResdLots,             //
    ExecLots,             //
    AvgPrice,             //
    LastLots,             //
    LastPrice,            //
    MinLots
};
constexpr int ColumnCount{unbox(Column::MinLots) + 1};

} // namespace order

class Order {
  public:
    Order(const QDateTime& created, const QDateTime& modified, const QString& accnt, Id64 market_id,
          const Instr& instr, QDate settl_date, Id64 id, const QString& ref, State state, Side side,
          Lots lots, Ticks ticks, Lots resd_lots, Lots exec_lots, Cost exec_cost, Lots last_lots,
          Ticks last_ticks, Lots min_lots);
    Order() = default;
    ~Order() = default;

    static Order from_json(const Instr& instr, const QJsonObject& obj);

    OrderKey key() const noexcept { return {market_id_, id_}; }
    const QDateTime& created() const noexcept { return created_; }
    const QDateTime& modified() const noexcept { return modified_; }
    const QString& accnt() const noexcept { return accnt_; }
    Id64 market_id() const noexcept { return market_id_; }
    const Instr& instr() const noexcept { return instr_; }
    QDate settl_date() const noexcept { return settl_date_; }
    Id64 id() const noexcept { return id_; }
    const QString& ref() const noexcept { return ref_; }
    State state() const noexcept { return state_; }
    Side side() const noexcept { return side_; }
    Lots lots() const noexcept { return lots_; }
    Ticks ticks() const noexcept { return ticks_; }
    Lots resd_lots() const noexcept { return resd_lots_; }
    Lots exec_lots() const noexcept { return exec_lots_; }
    Cost exec_cost() const noexcept { return exec_cost_; }
    Lots last_lots() const noexcept { return last_lots_; }
    Ticks last_ticks() const noexcept { return last_ticks_; }
    Lots min_lots() const noexcept { return min_lots_; }
    bool done() const noexcept { return resd_lots_ == 0_lts; }

  private:
    QDateTime created_{};
    QDateTime modified_{};
    QString accnt_{};
    Id64 market_id_{};
    Instr instr_{};
    QDate settl_date_{};
    Id64 id_{};
    QString ref_{};
    State state_{};
    Side side_{};
    Lots lots_{};
    Ticks ticks_{};
    Lots resd_lots_{};
    Lots exec_lots_{};
    Cost exec_cost_{};
    Lots last_lots_{};
    Ticks last_ticks_{};
    Lots min_lots_{};
};

QDebug operator<<(QDebug debug, const Order& order);

inline bool is_modified(const Order& prev, const Order& next) noexcept
{
    return prev.modified() != next.modified()   //
        || prev.state() != next.state()         //
        || prev.lots() != next.lots()           //
        || prev.resd_lots() != next.resd_lots() //
        || prev.exec_lots() != next.exec_lots() //
        || prev.exec_cost() != next.exec_cost() //
        || prev.last_lots() != next.last_lots() //
        || prev.last_ticks() != next.last_ticks();
}
} // namespace ui
} // namespace swirly

Q_DECLARE_METATYPE(swirly::ui::Order)

#endif // SWIRLYUI_ORDER_HXX
