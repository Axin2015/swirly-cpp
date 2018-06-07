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
#include "Posn.hxx"

#include <QJsonObject>

namespace swirly {
namespace ui {

Posn Posn::from_json(const Instr& instr, const QJsonObject& obj)
{
    using swirly::ui::from_json;
    return Posn{from_json<QString>(obj["accnt"]),
                from_json<Id64>(obj["market_id"]),
                instr,
                from_json<QDate>(obj["settl_date"]),
                from_json<Lots>(obj["buy_lots"]),
                from_json<Cost>(obj["buy_cost"]),
                from_json<Lots>(obj["sell_lots"]),
                from_json<Cost>(obj["sell_cost"])};
}

QDebug operator<<(QDebug debug, const Posn& posn)
{
    debug.nospace() << "Posn{accnt=" << posn.accnt()       //
                    << ",market_id=" << posn.market_id()   //
                    << ",instr=" << posn.instr().symbol()  //
                    << ",settl_date=" << posn.settl_date() //
                    << ",buy_lots=" << posn.buy_lots()     //
                    << ",buy_cost=" << posn.buy_cost()     //
                    << ",sell_lots=" << posn.sell_lots()   //
                    << ",sell_cost=" << posn.sell_cost()   //
                    << '}';
    return debug;
}

} // namespace ui
} // namespace swirly
