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
void to_json_levels(LevelSet::ConstIterator it, LevelSet::ConstIterator end, ostream& os, FnT fn)
{
    for (size_t i{0}; i < MaxLevels; ++i) {
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
} // namespace

Market::~Market() = default;

Market::Market(Market&&) = default;

void Market::to_dsv(ostream& os, char delim) const
{
    OStreamJoiner osj{os, delim};
    osj << id_ //
        << instr_;
    if (settl_day_ != 0_jd) {
        osj << jd_to_iso(settl_day_);
    } else {
        osj << "";
    }
    osj << state_;
    if (last_lots_ != 0_lts) {
        osj << last_lots_  //
            << last_ticks_ //
            << last_time_;
    } else {
        osj << ""
            << ""
            << "";
    }
    osj << max_id_;
}

void Market::to_json(ostream& os) const
{
    os << "{\"id\":" << id_         //
       << ",\"instr\":\"" << instr_ //
       << "\",\"settl_date\":";
    if (settl_day_ != 0_jd) {
        os << jd_to_iso(settl_day_);
    } else {
        os << "null";
    }
    os << ",\"state\":" << state_;
    if (last_lots_ != 0_lts) {
        os << ",\"last_lots\":" << last_lots_   //
           << ",\"last_ticks\":" << last_ticks_ //
           << ",\"last_time\":" << last_time_;
    } else {
        os << ",\"last_lots\":null,\"last_ticks\":null,\"last_time\":null";
    }

    const auto& bid_levels = bid_side_.levels();
    os << ",\"bid_ticks\":[";
    to_json_levels(bid_levels.begin(), bid_levels.end(), os,
                   [](const auto& level) { return level.ticks(); });
    os << "],\"bid_lots\":[";
    to_json_levels(bid_levels.begin(), bid_levels.end(), os,
                   [](const auto& level) { return level.lots(); });
    os << "],\"bid_count\":[";
    to_json_levels(bid_levels.begin(), bid_levels.end(), os,
                   [](const auto& level) { return level.count(); });

    const auto& offer_levels = offer_side_.levels();
    os << "],\"offer_ticks\":[";
    to_json_levels(offer_levels.begin(), offer_levels.end(), os,
                   [](const auto& level) { return level.ticks(); });
    os << "],\"offer_lots\":[";
    to_json_levels(offer_levels.begin(), offer_levels.end(), os,
                   [](const auto& level) { return level.lots(); });
    os << "],\"offer_count\":[";
    to_json_levels(offer_levels.begin(), offer_levels.end(), os,
                   [](const auto& level) { return level.count(); });
    os << "]}";
}

} // namespace fin
} // namespace swirly
