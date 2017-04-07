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
#include "Exec.hxx"

#include <QJsonObject>

namespace swirly {
namespace ui {

Exec::Exec(const QString& accnt, Id64 marketId, const Instr& instr, QDate settlDate, Id64 id,
           Id64 orderId, const QString& ref, State state, Side side, Lots lots, Ticks ticks,
           Lots resdLots, Lots execLots, Cost execCost, Lots lastLots, Ticks lastTicks,
           Lots minLots, Id64 matchId, LiqInd liqInd, const QString& cpty, const QDateTime& created)
    : accnt_{accnt},
      marketId_{marketId},
      instr_{instr},
      settlDate_{settlDate},
      id_{id},
      orderId_{orderId},
      ref_{ref},
      state_{state},
      side_{side},
      lots_{lots},
      ticks_{ticks},
      resdLots_{resdLots},
      execLots_{execLots},
      execCost_{execCost},
      lastLots_{lastLots},
      lastTicks_{lastTicks},
      minLots_{minLots},
      matchId_{matchId},
      liqInd_{liqInd},
      cpty_{cpty},
      created_{created}
{
}

Exec Exec::fromJson(const Instr& instr, const QJsonObject& obj)
{
    using swirly::ui::fromJson;
    return Exec{fromJson<QString>(obj["accnt"]),
                fromJson<Id64>(obj["marketId"]),
                instr,
                fromJson<QDate>(obj["settlDate"]),
                fromJson<Id64>(obj["id"]),
                fromJson<Id64>(obj["orderId"]),
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
                fromJson<Id64>(obj["matchId"]),
                fromJson<LiqInd>(obj["liqInd"]),
                fromJson<QString>(obj["cpty"]),
                fromJson<QDateTime>(obj["created"])};
}

QDebug operator<<(QDebug debug, const Exec& exec)
{
    debug.nospace() << "Exec{accnt=" << exec.accnt() //
                    << ",marketId=" << exec.marketId() //
                    << ",instr=" << exec.instr() //
                    << ",settlDate=" << exec.settlDate() //
                    << ",id=" << exec.id() //
                    << ",orderId=" << exec.orderId() //
                    << ",ref=" << exec.ref() //
                    << ",state=" << exec.state() //
                    << ",side=" << exec.side() //
                    << ",lots=" << exec.lots() //
                    << ",ticks=" << exec.ticks() //
                    << ",resdLots=" << exec.resdLots() //
                    << ",execLots=" << exec.execLots() //
                    << ",execCost=" << exec.execCost() //
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
