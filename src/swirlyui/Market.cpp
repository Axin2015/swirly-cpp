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
#include "Market.hxx"

#include <QJsonArray>
#include <QJsonObject>

namespace swirly {
namespace ui {
using namespace std;
namespace {

void to_levels(const QJsonArray& ticks, const QJsonArray& resd, const QJsonArray& count,
               Market::Levels& levels)
{
    for (size_t i{0}; i < MaxLevels; ++i) {
        levels[i]
            = {from_json<Ticks>(ticks[i]), from_json<Lots>(resd[i]), from_json<int>(count[i])};
    }
}

} // namespace

Market Market::from_json(const Instr& instr, const QJsonObject& obj)
{
    using swirly::ui::from_json;

    Market market{from_json<Id64>(obj["id"]),
                  instr,
                  from_json<QDate>(obj["settl_date"]),
                  from_json<MarketState>(obj["state"]),
                  from_json<QDateTime>(obj["last_time"]),
                  from_json<Lots>(obj["last_lots"]),
                  from_json<Ticks>(obj["last_ticks"])};

    const auto bid_ticks = obj["bid_ticks"].toArray();
    const auto bid_lots = obj["bid_lots"].toArray();
    const auto bid_count = obj["bid_count"].toArray();
    to_levels(bid_ticks, bid_lots, bid_count, market.bids_);

    const auto offer_ticks = obj["offer_ticks"].toArray();
    const auto offer_lots = obj["offer_lots"].toArray();
    const auto offer_count = obj["offer_count"].toArray();
    to_levels(offer_ticks, offer_lots, offer_count, market.offers_);

    return market;
}

QDebug operator<<(QDebug debug, const Market& market)
{
    debug.nospace() << "Market{id=" << market.id()               //
                    << ",instr=" << market.instr().symbol()      //
                    << ",settl_date=" << market.settl_date()     //
                    << ",state=" << market.state()               //
                    << ",last_time=" << market.last_time()       //
                    << ",last_lots=" << market.last_lots()       //
                    << ",last_ticks=" << market.last_ticks()     //
                    << ",best_bid=" << market.bids().front()     //
                    << ",best_offer=" << market.offers().front() //
                    << '}';
    return debug;
}

} // namespace ui
} // namespace swirly
