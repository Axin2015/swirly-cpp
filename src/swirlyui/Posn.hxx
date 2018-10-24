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
#ifndef SWIRLYUI_POSN_HXX
#define SWIRLYUI_POSN_HXX

#include "Market.hxx"

namespace swirly {
namespace ui {
namespace posn {

enum class Column : int { //
    CheckState,           //
    Accnt,                //
    MarketId,             //
    Instr,                //
    SettlDate,            //
    BuyLots,              //
    BuyAvgPrice,          //
    SellLots,             //
    SellAvgPrice
};
constexpr int ColumnCount{unbox(Column::SellAvgPrice) + 1};

} // namespace posn

class Posn {
  public:
    Posn(const QString& accnt, const Market& market, Lots buy_lots, Cost buy_cost, Lots sell_lots,
         Cost sell_cost, Cost open_cost)
    : accnt_{accnt}
    , market_{market}
    , buy_lots_{buy_lots}
    , buy_cost_{buy_cost}
    , sell_lots_{sell_lots}
    , sell_cost_{sell_cost}
    , open_cost_{open_cost}
    {
    }
    Posn() = default;
    ~Posn() = default;

    static Posn from_json(const Market& market, const QJsonObject& obj);

    const QString& accnt() const noexcept { return accnt_; }
    const Market& market() const noexcept { return market_; }
    const Instr& instr() const noexcept { return market_.instr(); }
    QDate settl_date() const noexcept { return market_.settl_date(); }
    Lots buy_lots() const noexcept { return buy_lots_; }
    Cost buy_cost() const noexcept { return buy_cost_; }
    Lots sell_lots() const noexcept { return sell_lots_; }
    Cost sell_cost() const noexcept { return sell_cost_; }
    Cost open_cost() const noexcept { return open_cost_; }

  private:
    QString accnt_{};
    Market market_{};
    Lots buy_lots_{};
    Cost buy_cost_{};
    Lots sell_lots_{};
    Cost sell_cost_{};
    Cost open_cost_{};
};

QDebug operator<<(QDebug debug, const Posn& posn);

inline bool is_modified(const Posn& prev, const Posn& next) noexcept
{
    return prev.buy_lots() != next.buy_lots()   //
        || prev.buy_cost() != next.buy_cost()   //
        || prev.sell_lots() != next.sell_lots() //
        || prev.sell_cost() != next.sell_cost() //
        || prev.open_cost() != next.open_cost();
}

} // namespace ui
} // namespace swirly

Q_DECLARE_METATYPE(swirly::ui::Posn)

#endif // SWIRLYUI_POSN_HXX
