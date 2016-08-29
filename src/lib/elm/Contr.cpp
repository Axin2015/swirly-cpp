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
#include <swirly/elm/Contr.hpp>

#include <swirly/elm/Conv.hpp>

using namespace std;

namespace swirly {

Contr::Contr(Mnem mnem, string_view display, Mnem asset, Mnem ccy, int lotNumer, int lotDenom,
             int tickNumer, int tickDenom, int pipDp, Lots minLots, Lots maxLots) noexcept
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

Contr::~Contr() noexcept = default;

Contr::Contr(const Contr&) = default;

Contr::Contr(Contr&&) = default;

void Contr::toJson(ostream& os) const
{
  os << "{\"mnem\":\"" << mnem_ //
     << "\",\"display\":\"" << display_ //
     << "\",\"asset\":\"" << asset_ //
     << "\",\"ccy\":\"" << ccy_ //
     << "\",\"lotNumer\":" << lotNumer_ //
     << ",\"lotDenom\":" << lotDenom_ //
     << ",\"tickNumer\":" << tickNumer_ //
     << ",\"tickDenom\":" << tickDenom_ //
     << ",\"pipDp\":" << pipDp_ //
     << ",\"minLots\":" << minLots_ //
     << ",\"maxLots\":" << maxLots_ //
     << "}";
}

} // swirly
