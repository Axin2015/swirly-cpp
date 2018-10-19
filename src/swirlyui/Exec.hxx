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
#ifndef SWIRLYUI_EXEC_HXX
#define SWIRLYUI_EXEC_HXX

#include "Product.hxx"

namespace swirly {
namespace ui {
namespace exec {

enum class Column : int { //
    CheckState,           //
    Created,              //
    Accnt,                //
    MarketId,             //
    Product,              //
    SettlDate,            //
    Id,                   //
    OrderId,              //
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
    MinLots,              //
    MatchId,              //
    LiqInd,               //
    Cpty
};
constexpr int ColumnCount{unbox(Column::Cpty) + 1};

} // namespace exec

class Exec {
  public:
    Exec(const QDateTime& created, const QString& accnt, Id64 market_id, const Product& product,
         QDate settl_date, Id64 id, Id64 order_id, const QString& ref, State state, Side side,
         Lots lots, Ticks ticks, Lots resd_lots, Lots exec_lots, Cost exec_cost, Lots last_lots,
         Ticks last_ticks, Lots min_lots, Id64 match_id, LiqInd liq_ind, const QString& cpty);
    Exec() = default;
    ~Exec() = default;

    static Exec from_json(const Product& product, const QJsonObject& obj);

    ExecKey key() const noexcept { return {market_id_, id_}; }
    const QDateTime& created() const noexcept { return created_; }
    const QString& accnt() const noexcept { return accnt_; }
    Id64 market_id() const noexcept { return market_id_; }
    const Product& product() const noexcept { return product_; }
    QDate settl_date() const noexcept { return settl_date_; }
    Id64 id() const noexcept { return id_; }
    Id64 order_id() const noexcept { return order_id_; }
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
    Id64 match_id() const noexcept { return match_id_; }
    LiqInd liq_ind() const noexcept { return liq_ind_; }
    const QString& cpty() const noexcept { return cpty_; }

  private:
    QDateTime created_{};
    QString accnt_{};
    Id64 market_id_{};
    Product product_{};
    QDate settl_date_{};
    Id64 id_{};
    Id64 order_id_{};
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
    Id64 match_id_{};
    LiqInd liq_ind_{};
    QString cpty_{};
};

QDebug operator<<(QDebug debug, const Exec& exec);

inline bool is_modified(const Exec& prev, const Exec& next) noexcept
{
    // Immutable.
    return false;
}

} // namespace ui
} // namespace swirly

Q_DECLARE_METATYPE(swirly::ui::Exec)

#endif // SWIRLYUI_EXEC_HXX
