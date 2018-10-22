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
#include "Instr.hxx"

#include <QJsonObject>

namespace swirly {
namespace ui {
using namespace std;

Instr::Impl::Impl(Id32 id, const QString& symbol, const QString& display, const QString& base_asset,
                  const QString& term_ccy, int lot_numer, int lot_denom, int tick_numer,
                  int tick_denom, int pip_dp, Lots min_lots, Lots max_lots)
: id{id}
, symbol{symbol}
, display{display}
, base_asset{base_asset}
, term_ccy{term_ccy}
, lot_numer{lot_numer}
, lot_denom{lot_denom}
, qty_inc{fract_to_real(lot_numer, lot_denom)}
, tick_numer{tick_numer}
, tick_denom{tick_denom}
, price_inc{fract_to_real(tick_numer, tick_denom)}
, pip_dp{pip_dp}
, qty_dp{real_to_dp(qty_inc)}
, price_dp{real_to_dp(price_inc)}
, min_lots{min_lots}
, max_lots{max_lots}
{
}

Instr Instr::from_json(const QJsonObject& obj)
{
    using swirly::ui::from_json;
    return Instr(from_json<Id32>(obj["id"]), from_json<QString>(obj["symbol"]),
                 from_json<QString>(obj["display"]), from_json<QString>(obj["base_asset"]),
                 from_json<QString>(obj["term_ccy"]), from_json<int>(obj["lot_numer"]),
                 from_json<int>(obj["lot_denom"]), from_json<int>(obj["tick_numer"]),
                 from_json<int>(obj["tick_denom"]), from_json<int>(obj["pip_dp"]),
                 from_json<Lots>(obj["min_lots"]), from_json<Lots>(obj["max_lots"]));
}

shared_ptr<const Instr::Impl> Instr::empty()
{
    static auto impl = make_shared<const Impl>();
    return impl;
}

QDebug operator<<(QDebug debug, const Instr& instr)
{
    debug.nospace() << "Instr{symbol=" << instr.symbol()    //
                    << ",display=" << instr.display()       //
                    << ",base_asset=" << instr.base_asset() //
                    << ",term_ccy=" << instr.term_ccy()     //
                    << ",lot_numer=" << instr.lot_numer()   //
                    << ",lot_denom=" << instr.lot_denom()   //
                    << ",tick_numer=" << instr.tick_numer() //
                    << ",tick_denom=" << instr.tick_denom() //
                    << ",pip_dp=" << instr.pip_dp()         //
                    << ",min_lots=" << instr.min_lots()     //
                    << ",max_lots=" << instr.max_lots()     //
                    << '}';
    return debug;
}

} // namespace ui
} // namespace swirly
