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
#include <swirly/elm/Market.hpp>

#include <swirly/ash/Date.hpp>

#include <cassert>

using namespace std;

namespace swirly {

Market::Market(Mnem mnem, string_view display, Mnem contr, JDay settlDay,
               MarketState state) noexcept
  : mnem_{mnem}, display_{display}, contr_{contr}, settlDay_{settlDay}, state_{state}
{
}

Market::~Market() noexcept = default;

Market::Market(const Market&) = default;

Market::Market(Market&&) = default;

void Market::toJson(ostream& os) const
{
  os << "{\"mnem\":\"" << mnem_ //
     << "\",\"display\":\"" << display_ //
     << "\",\"contr\":\"" << contr_ //
     << "\",\"settlDate\":";

  if (settlDay_ != 0_jd) {
    os << jdToIso(settlDay_);
  } else {
    os << "null";
  }

  os << ",\"state\":" << state_ //
     << '}';
}

} // swirly
