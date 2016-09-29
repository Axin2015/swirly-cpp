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
#include "Exec.hpp"

#include "Json.hpp"

#include <QDebug>
#include <QJsonObject>

namespace swirly {
namespace ui {

Exec::Exec(Id64 marketId, const QString& contr, QDate settlDate, Id64 id, Id64 orderId,
           const QString& accnt, const QString& ref, State state, Side side, Lots lots, Ticks ticks,
           Lots resd, Lots exec, Cost cost, Lots lastLots, Ticks lastTicks, Lots minLots,
           Id64 matchId, LiqInd liqInd, const QString& cpty, const QDateTime& created)
  : marketId_{marketId},
    contr_{contr},
    settlDate_{settlDate},
    id_{id},
    orderId_{orderId},
    accnt_{accnt},
    ref_{ref},
    state_{state},
    side_{side},
    lots_{lots},
    ticks_{ticks},
    resd_{resd},
    exec_{exec},
    cost_{cost},
    lastLots_{lastLots},
    lastTicks_{lastTicks},
    minLots_{minLots},
    matchId_{matchId},
    liqInd_{liqInd},
    cpty_{cpty},
    created_{created}
{
}

Exec Exec::fromJson(const QJsonObject& obj)
{
  using swirly::ui::fromJson;
  return Exec{fromJson<Id64>(obj["marketId"]),    fromJson<QString>(obj["contr"]),
              fromJson<QDate>(obj["settlDate"]),  fromJson<Id64>(obj["id"]),
              fromJson<Id64>(obj["orderId"]),     fromJson<QString>(obj["accnt"]),
              fromJson<QString>(obj["ref"]),      fromJson<State>(obj["state"]),
              fromJson<Side>(obj["side"]),        fromJson<Lots>(obj["lots"]),
              fromJson<Ticks>(obj["ticks"]),      fromJson<Lots>(obj["resd"]),
              fromJson<Lots>(obj["exec"]),        fromJson<Cost>(obj["cost"]),
              fromJson<Lots>(obj["lastLots"]),    fromJson<Ticks>(obj["lastTicks"]),
              fromJson<Lots>(obj["minLots"]),     fromJson<Id64>(obj["matchId"]),
              fromJson<LiqInd>(obj["liqInd"]),    fromJson<QString>(obj["cpty"]),
              fromJson<QDateTime>(obj["created"])};
}

QDebug operator<<(QDebug debug, const Exec& exec)
{
  debug.nospace() << "Exec{marketId=" << exec.marketId() //
                  << ",contr=" << exec.contr() //
                  << ",settlDate=" << exec.settlDate() //
                  << ",id=" << exec.id() //
                  << ",orderId=" << exec.orderId() //
                  << ",accnt=" << exec.accnt() //
                  << ",ref=" << exec.ref() //
                  << ",state=" << exec.state() //
                  << ",side=" << exec.side() //
                  << ",lots=" << exec.lots() //
                  << ",ticks=" << exec.ticks() //
                  << ",resd=" << exec.resd() //
                  << ",exec=" << exec.exec() //
                  << ",cost=" << exec.cost() //
                  << ",lastLots=" << exec.lastLots() //
                  << ",lastTicks=" << exec.lastTicks() //
                  << ",minLots=" << exec.minLots() //
                  << ",matchId=" << exec.matchId() //
                  << ",liqInd=" << exec.liqInd() //
                  << ",cpty=" << exec.cpty() //
                  << ",created=" << exec.created() //
                  << '}';
  return debug;
}

} // ui
} // swirly
