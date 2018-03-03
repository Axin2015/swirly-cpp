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
#include "Order.hxx"

#include <QJsonObject>

namespace swirly {
namespace ui {

Order::Order(const QString& accnt, Id64 marketId, const Instr& instr, QDate settlDate, Id64 id,
             const QString& ref, State state, Side side, Lots lots, Ticks ticks, Lots resdLots,
             Lots execLots, Cost execCost, Lots lastLots, Ticks lastTicks, Lots minLots,
             const QDateTime& created, const QDateTime& modified)
: accnt_{accnt}
, marketId_{marketId}
, instr_{instr}
, settlDate_{settlDate}
, id_{id}
, ref_{ref}
, state_{state}
, side_{side}
, lots_{lots}
, ticks_{ticks}
, resdLots_{resdLots}
, execLots_{execLots}
, execCost_{execCost}
, lastLots_{lastLots}
, lastTicks_{lastTicks}
, minLots_{minLots}
, created_{created}
, modified_{modified}
{
}

Order Order::fromJson(const Instr& instr, const QJsonObject& obj)
{
    using swirly::ui::fromJson;
    return Order{fromJson<QString>(obj["accnt"]),
                 fromJson<Id64>(obj["marketId"]),
                 instr,
                 fromJson<QDate>(obj["settlDate"]),
                 fromJson<Id64>(obj["id"]),
                 fromJson<QString>(obj["ref"]),
                 fromJson<State>(obj["state"]),
                 fromJson<Side>(obj["side"]),
                 fromJson<Lots>(obj["lots"]),
                 fromJson<Ticks>(obj["ticks"]),
                 fromJson<Lots>(obj["resdLots"]),
                 fromJson<Lots>(obj["execLots"]),
                 fromJson<Cost>(obj["execCost"]),
                 fromJson<Lots>(obj["lastLots"]),
                 fromJson<Ticks>(obj["lastTicks"]),
                 fromJson<Lots>(obj["minLots"]),
                 fromJson<QDateTime>(obj["created"]),
                 fromJson<QDateTime>(obj["modified"])};
}

QDebug operator<<(QDebug debug, const Order& order)
{
    debug.nospace() << "Order{accnt=" << order.accnt()    //
                    << ",marketId=" << order.marketId()   //
                    << ",instr=" << order.instr()         //
                    << ",settlDate=" << order.settlDate() //
                    << ",id=" << order.id()               //
                    << ",ref=" << order.ref()             //
                    << ",state=" << order.state()         //
                    << ",side=" << order.side()           //
                    << ",lots=" << order.lots()           //
                    << ",ticks=" << order.ticks()         //
                    << ",resdLots=" << order.resdLots()   //
                    << ",execLots=" << order.execLots()   //
                    << ",execCost=" << order.execCost()   //
                    << ",lastLots=" << order.lastLots()   //
                    << ",lastTicks=" << order.lastTicks() //
                    << ",minLots=" << order.minLots()     //
                    << ",created=" << order.created()     //
                    << ",modified=" << order.modified()   //
                    << '}';
    return debug;
}

} // namespace ui
} // namespace swirly
