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
#include <swirly/elm/MarketBook.hpp>

#include <swirly/ash/JulianDay.hpp>

#include <cassert>

using namespace std;

namespace swirly {
namespace {
template <typename FnT>
void toJsonLevels(LevelSet::ConstIterator it, LevelSet::ConstIterator end, ostream& os, FnT fn)
{
  enum { Levels = 3 };
  for (int i{0}; i < Levels; ++i) {
    if (i > 0) {
      os << ',';
    }
    if (it != end) {
      os << fn(*it);
      ++it;
    } else {
      os << "null";
    }
  }
}
} // anonymous

MarketBook::MarketBook(string_view mnem, string_view display, string_view contr, Jday settlDay,
                       Jday expiryDay, MarketState state, Lots lastLots, Ticks lastTicks,
                       Millis lastTime, Iden maxOrderId, Iden maxExecId) noexcept
  : Market{mnem, display, contr, settlDay, expiryDay, state},
    lastLots_{lastLots},
    lastTicks_{lastTicks},
    lastTime_{lastTime},
    maxOrderId_{maxOrderId},
    maxExecId_{maxExecId}
{
}

MarketBook::~MarketBook() noexcept = default;

void MarketBook::toJsonView(ostream& os) const
{
  os << "{\"market\":\"" << mnem_ //
     << "\",\"contr\":\"" << contr_ //
     << "\",\"settlDate\":";
  if (settlDay_ != 0_jd) {
    os << jdToIso(settlDay_);
  } else {
    os << "null";
  }
  if (lastLots_ != 0_lts) {
    os << ",\"lastLots\":" << lastLots_ //
       << ",\"lastTicks\":" << lastTicks_ //
       << ",\"lastTime\":" << lastTime_;
  } else {
    os << ",\"lastLots\":null,\"lastTicks\":null,\"lastTime\":null";
  }

  const auto& bidLevels = bidSide_.levels();
  os << ",\"bidTicks\":[";
  toJsonLevels(bidLevels.begin(), bidLevels.end(), os,
               [](const auto& level) { return level.ticks(); });
  os << "],\"bidResd\":[";
  toJsonLevels(bidLevels.begin(), bidLevels.end(), os,
               [](const auto& level) { return level.resd(); });
  os << "],\"bidCount\":[";
  toJsonLevels(bidLevels.begin(), bidLevels.end(), os,
               [](const auto& level) { return level.count(); });

  const auto& offerLevels = offerSide_.levels();
  os << "],\"offerTicks\":[";
  toJsonLevels(offerLevels.begin(), offerLevels.end(), os,
               [](const auto& level) { return level.ticks(); });
  os << "],\"offerResd\":[";
  toJsonLevels(offerLevels.begin(), offerLevels.end(), os,
               [](const auto& level) { return level.resd(); });
  os << "],\"offerCount\":[";
  toJsonLevels(offerLevels.begin(), offerLevels.end(), os,
               [](const auto& level) { return level.count(); });
  os << "]}";
}

} // swirly
