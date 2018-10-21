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
#ifndef SWIRLYUI_MARKET_HXX
#define SWIRLYUI_MARKET_HXX

#include "Instr.hxx"
#include "Level.hxx"

namespace swirly {
namespace ui {
namespace market {

enum class Column : int { //
    CheckState,           //
    Id,                   //
    Instr,                //
    SettlDate,            //
    State,                //
    BidCount,             //
    BidLots,              //
    BidPrice,             //
    LastLots,             //
    LastPrice,            //
    LastTime,             //
    OfferPrice,           //
    OfferLots,            //
    OfferCount
};
constexpr int ColumnCount{unbox(Column::OfferCount) + 1};

} // namespace market

class Market {
  public:
    using Levels = std::array<Level, MaxLevels>;

    Market(Id64 id, const Instr& instr, QDate settl_date, MarketState state,
           const QDateTime& last_time, Lots last_lots, Ticks last_ticks)
    : id_{id}
    , instr_{instr}
    , settl_date_{settl_date}
    , state_{state}
    , last_time_{last_time}
    , last_lots_{last_lots}
    , last_ticks_{last_ticks}
    {
    }
    Market() = default;
    ~Market() = default;

    static Market from_json(const Instr& instr, const QJsonObject& obj);

    Id64 id() const noexcept { return id_; }
    const Instr& instr() const noexcept { return instr_; }
    QDate settl_date() const noexcept { return settl_date_; }
    MarketState state() const noexcept { return state_; }
    const QDateTime& last_time() const noexcept { return last_time_; }
    Lots last_lots() const noexcept { return last_lots_; }
    Ticks last_ticks() const noexcept { return last_ticks_; }
    const Levels& bids() const noexcept { return bids_; }
    const Levels& offers() const noexcept { return offers_; }
    const Level& best_bid() const noexcept { return bids_.front(); }
    const Level& best_offer() const noexcept { return offers_.front(); }

  private:
    Id64 id_{};
    Instr instr_{};
    QDate settl_date_{};
    MarketState state_{};
    QDateTime last_time_{};
    Lots last_lots_{};
    Ticks last_ticks_{};
    Levels bids_{};
    Levels offers_{};
};

QDebug operator<<(QDebug debug, const Market& market);

inline bool is_modified(const Market& prev, const Market& next) noexcept
{
    // Assume market-data has changed.
    return true;
}

} // namespace ui
} // namespace swirly

Q_DECLARE_METATYPE(swirly::ui::Market)

#endif // SWIRLYUI_MARKET_HXX
