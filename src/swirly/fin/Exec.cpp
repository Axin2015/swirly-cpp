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
#include "Exec.hpp"

#include <swirly/util/Date.hpp>
#include <swirly/util/Stream.hpp>

namespace swirly {
inline namespace fin {
using namespace std;

static_assert(sizeof(Exec) <= 5 * 64, "no greater than specified cache-lines");

Exec::~Exec() = default;

Exec::Exec(Exec&&) = default;

void Exec::toDsv(ostream& os, char sep) const
{
    OStreamJoiner osj{os, sep};
    osj << accnt_    //
        << marketId_ //
        << instr_;
    if (settlDay_ != 0_jd) {
        osj << jdToIso(settlDay_);
    } else {
        osj << "";
    }
    osj << id_ //
        << orderId_;
    if (!ref_.empty()) {
        osj << ref_;
    } else {
        osj << "";
    }
    osj << state_    //
        << side_     //
        << lots_     //
        << ticks_    //
        << resdLots_ //
        << execLots_ //
        << execCost_;
    if (lastLots_ != 0_lts) {
        osj << lastLots_ //
            << lastTicks_;
    } else {
        osj << ""
            << "";
    }
    if (minLots_ != 0_lts) {
        osj << minLots_;
    } else {
        osj << "";
    }
    if (matchId_ != 0_id64) {
        osj << matchId_;
    } else {
        osj << "";
    }
    if (liqInd_ != LiqInd::None) {
        osj << liqInd_;
    } else {
        osj << "";
    }
    if (!cpty_.empty()) {
        osj << cpty_;
    } else {
        osj << "";
    }
    osj << created_;
}

void Exec::toJson(ostream& os) const
{
    os << "{\"accnt\":\"" << accnt_       //
       << "\",\"marketId\":" << marketId_ //
       << ",\"instr\":\"" << instr_       //
       << "\",\"settlDate\":";
    if (settlDay_ != 0_jd) {
        os << jdToIso(settlDay_);
    } else {
        os << "null";
    }
    os << ",\"id\":" << id_           //
       << ",\"orderId\":" << orderId_ //
       << ",\"ref\":";
    if (!ref_.empty()) {
        os << '"' << ref_ << '"';
    } else {
        os << "null";
    }
    os << ",\"state\":\"" << state_     //
       << "\",\"side\":\"" << side_     //
       << "\",\"lots\":" << lots_       //
       << ",\"ticks\":" << ticks_       //
       << ",\"resdLots\":" << resdLots_ //
       << ",\"execLots\":" << execLots_ //
       << ",\"execCost\":" << execCost_;
    if (lastLots_ != 0_lts) {
        os << ",\"lastLots\":" << lastLots_ //
           << ",\"lastTicks\":" << lastTicks_;
    } else {
        os << ",\"lastLots\":null,\"lastTicks\":null";
    }
    os << ",\"minLots\":";
    if (minLots_ != 0_lts) {
        os << minLots_;
    } else {
        os << "null";
    }
    os << ",\"matchId\":";
    if (matchId_ != 0_id64) {
        os << matchId_;
    } else {
        os << "null";
    }
    os << ",\"liqInd\":";
    if (liqInd_ != LiqInd::None) {
        os << '"' << liqInd_ << '"';
    } else {
        os << "null";
    }
    os << ",\"cpty\":";
    if (!cpty_.empty()) {
        os << '"' << cpty_ << '"';
    } else {
        os << "null";
    }
    os << ",\"created\":" << created_ //
       << '}';
}

ExecPtr Exec::opposite(Id64 id) const
{
    assert(!cpty_.empty());
    return make(cpty_, marketId_, instr_, settlDay_, id, orderId_, +ref_, state_,
                swirly::opposite(side_), lots_, ticks_, resdLots_, execLots_, execCost_, lastLots_,
                lastTicks_, minLots_, matchId_, swirly::opposite(liqInd_), accnt_, created_);
}

void Exec::trade(Lots sumLots, Cost sumCost, Lots lastLots, Ticks lastTicks, Id64 matchId,
                 LiqInd liqInd, Symbol cpty) noexcept
{
    state_ = State::Trade;
    resdLots_ -= sumLots;
    execLots_ += sumLots;
    execCost_ += sumCost;
    lastLots_ = lastLots;
    lastTicks_ = lastTicks;
    matchId_ = matchId;
    liqInd_ = liqInd;
    cpty_ = cpty;
}

} // namespace fin
} // namespace swirly
