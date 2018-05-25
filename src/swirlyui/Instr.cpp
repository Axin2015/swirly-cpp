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

Instr::Impl::Impl(const QString& symbol, const QString& display, const QString& baseAsset,
                  const QString& termCcy, int lotNumer, int lotDenom, int tickNumer, int tickDenom,
                  int pipDp, Lots minLots, Lots maxLots)
: symbol{symbol}
, display{display}
, baseAsset{baseAsset}
, termCcy{termCcy}
, lotNumer{lotNumer}
, lotDenom{lotDenom}
, qtyInc{fractToReal(lotNumer, lotDenom)}
, tickNumer{tickNumer}
, tickDenom{tickDenom}
, priceInc{fractToReal(tickNumer, tickDenom)}
, pipDp{pipDp}
, qtyDp{realToDp(qtyInc)}
, priceDp{realToDp(priceInc)}
, minLots{minLots}
, maxLots{maxLots}
{
}

Instr Instr::fromJson(const QJsonObject& obj)
{
    using swirly::ui::fromJson;
    return Instr(fromJson<QString>(obj["symbol"]), fromJson<QString>(obj["display"]),
                 fromJson<QString>(obj["base_asset"]), fromJson<QString>(obj["term_ccy"]),
                 fromJson<int>(obj["lot_numer"]), fromJson<int>(obj["lot_denom"]),
                 fromJson<int>(obj["tick_numer"]), fromJson<int>(obj["tick_denom"]),
                 fromJson<int>(obj["pip_dp"]), fromJson<Lots>(obj["min_lots"]),
                 fromJson<Lots>(obj["max_lots"]));
}

shared_ptr<const Instr::Impl> Instr::empty()
{
    static auto impl = make_shared<const Impl>();
    return impl;
}

QDebug operator<<(QDebug debug, const Instr& instr)
{
    debug.nospace() << "Instr{symbol=" << instr.symbol()  //
                    << ",display=" << instr.display()     //
                    << ",baseAsset=" << instr.baseAsset() //
                    << ",termCcy=" << instr.termCcy()     //
                    << ",lotNumer=" << instr.lotNumer()   //
                    << ",lotDenom=" << instr.lotDenom()   //
                    << ",tickNumer=" << instr.tickNumer() //
                    << ",tickDenom=" << instr.tickDenom() //
                    << ",pipDp=" << instr.pipDp()         //
                    << ",minLots=" << instr.minLots()     //
                    << ",maxLots=" << instr.maxLots()     //
                    << '}';
    return debug;
}

} // namespace ui
} // namespace swirly
