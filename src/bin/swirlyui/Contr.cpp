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
#include "Contr.hpp"

#include "Conv.hpp"
#include "Json.hpp"

#include <QDebug>
#include <QJsonObject>

namespace swirly {
namespace ui {

Contr::Contr(const QString& mnem, const QString& display, const QString& asset, const QString& ccy,
             int lotNumer, int lotDenom, int tickNumer, int tickDenom, int pipDp, Lots minLots,
             Lots maxLots)
  : mnem_{mnem},
    display_{display},
    asset_{asset},
    ccy_{ccy},
    lotNumer_{lotNumer},
    lotDenom_{lotDenom},
    qtyInc_{fractToReal(lotNumer, lotDenom)},
    tickNumer_{tickNumer},
    tickDenom_{tickDenom},
    priceInc_{fractToReal(tickNumer, tickDenom)},
    pipDp_{pipDp},
    qtyDp_{realToDp(qtyInc_)},
    priceDp_{realToDp(priceInc_)},
    minLots_{minLots},
    maxLots_{maxLots}
{
}

Contr Contr::fromJson(const QJsonObject& obj)
{
  using swirly::ui::fromJson;
  return Contr{fromJson<QString>(obj["mnem"]),  fromJson<QString>(obj["display"]),
               fromJson<QString>(obj["asset"]), fromJson<QString>(obj["ccy"]),
               fromJson<int>(obj["lotNumer"]),  fromJson<int>(obj["lotDenom"]),
               fromJson<int>(obj["tickNumer"]), fromJson<int>(obj["tickDenom"]),
               fromJson<int>(obj["pipDp"]),     fromJson<qint64>(obj["minLots"]),
               fromJson<qint64>(obj["maxLots"])};
}

QDebug operator<<(QDebug debug, const Contr& contr)
{
  debug.nospace() << "Contr{mnem=" << contr.mnem() //
                  << ",display=" << contr.display() //
                  << ",asset=" << contr.asset() //
                  << ",ccy=" << contr.ccy() //
                  << ",lotNumer=" << contr.lotNumer() //
                  << ",lotDenom=" << contr.lotDenom() //
                  << ",tickNumer=" << contr.tickNumer() //
                  << ",tickDenom=" << contr.tickDenom() //
                  << ",pipDp=" << contr.pipDp() //
                  << ",minLots=" << contr.minLots() //
                  << ",maxLots=" << contr.maxLots() //
                  << '}';
  return debug;
}

} // ui
} // swirly
