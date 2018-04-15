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
#include "Market.hpp"

#include <swirly/util/Date.hpp>
#include <swirly/util/Stream.hpp>

#include <cassert>

namespace swirly {
inline namespace fin {
using namespace std;

static_assert(sizeof(Market) <= 4 * 64, "no greater than specified cache-lines");

namespace {
template <typename FnT>
void toJsonLevels(LevelSet::ConstIterator it, LevelSet::ConstIterator end, ostream& os, FnT fn)
{
    for (size_t i{0}; i < MaxLevels; ++i) {
        if (i > 0) {
            os << ',';
        }
        if (it != end) {
            os << fn(*it);
            ++it;
        } else {
            os << "null"sv;
        }
    }
}
} // namespace

Market::~Market() = default;

Market::Market(Market&&) = default;

void Market::toDsv(ostream& os, char delim) const
{
    OStreamJoiner osj{os, delim};
    osj << id_ //
        << instr_;
    if (settlDay_ != 0_jd) {
        osj << jdToIso(settlDay_);
    } else {
        osj << ""sv;
    }
    osj << state_;
    if (lastLots_ != 0_lts) {
        osj << lastLots_  //
            << lastTicks_ //
            << lastTime_;
    } else {
        osj << ""sv
            << ""sv
            << ""sv;
    }
    osj << maxId_;
}

void Market::toJson(ostream& os) const
{
    os << "{\"id\":"sv << id_         //
       << ",\"instr\":\""sv << instr_ //
       << "\",\"settlDate\":"sv;
    if (settlDay_ != 0_jd) {
        os << jdToIso(settlDay_);
    } else {
        os << "null"sv;
    }
    os << ",\"state\":"sv << state_;
    if (lastLots_ != 0_lts) {
        os << ",\"lastLots\":"sv << lastLots_   //
           << ",\"lastTicks\":"sv << lastTicks_ //
           << ",\"lastTime\":"sv << lastTime_;
    } else {
        os << ",\"lastLots\":null,\"lastTicks\":null,\"lastTime\":null"sv;
    }

    const auto& bidLevels = bidSide_.levels();
    os << ",\"bidTicks\":["sv;
    toJsonLevels(bidLevels.begin(), bidLevels.end(), os,
                 [](const auto& level) { return level.ticks(); });
    os << "],\"bidLots\":["sv;
    toJsonLevels(bidLevels.begin(), bidLevels.end(), os,
                 [](const auto& level) { return level.lots(); });
    os << "],\"bidCount\":["sv;
    toJsonLevels(bidLevels.begin(), bidLevels.end(), os,
                 [](const auto& level) { return level.count(); });

    const auto& offerLevels = offerSide_.levels();
    os << "],\"offerTicks\":["sv;
    toJsonLevels(offerLevels.begin(), offerLevels.end(), os,
                 [](const auto& level) { return level.ticks(); });
    os << "],\"offerLots\":["sv;
    toJsonLevels(offerLevels.begin(), offerLevels.end(), os,
                 [](const auto& level) { return level.lots(); });
    os << "],\"offerCount\":["sv;
    toJsonLevels(offerLevels.begin(), offerLevels.end(), os,
                 [](const auto& level) { return level.count(); });
    os << "]}"sv;
}

} // namespace fin
} // namespace swirly
