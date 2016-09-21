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
#include "Market.hpp"

#include "Json.hpp"

#include <QDebug>
#include <QJsonObject>

namespace swirly {
namespace ui {

Market Market::fromJson(const QJsonObject& obj)
{
  using swirly::ui::fromJson;
  return Market{fromJson<QString>(obj["contr"]),     fromJson<QDate>(obj["settlDate"]),
                fromJson<MarketState>(obj["state"]), fromJson<Lots>(obj["lastLots"]),
                fromJson<Ticks>(obj["lastTicks"]),   fromJson<QDateTime>(obj["lastTime"])};
}

QDebug operator<<(QDebug debug, const Market& market)
{
  debug.nospace() << "contr=" << market.contr() //
                  << ",settlDate=" << market.settlDate() //
                  << ",state=" << market.state() //
                  << ",lastLots=" << market.lastLots() //
                  << ",lastTicks=" << market.lastTicks() //
                  << ",lastTime=" << market.lastTime();
  return debug;
}

} // ui
} // swirly
