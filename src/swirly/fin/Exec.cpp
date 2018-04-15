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

void Exec::toDsv(ostream& os, char delim) const
{
    OStreamJoiner osj{os, delim};
    osj << accnt_    //
        << marketId_ //
        << instr_;
    if (settlDay_ != 0_jd) {
        osj << jdToIso(settlDay_);
    } else {
        osj << ""sv;
    }
    osj << id_ //
        << orderId_;
    if (!ref_.empty()) {
        osj << ref_;
    } else {
        osj << ""sv;
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
        osj << ""sv
            << ""sv;
    }
    if (minLots_ != 0_lts) {
        osj << minLots_;
    } else {
        osj << ""sv;
    }
    if (matchId_ != 0_id64) {
        osj << matchId_;
    } else {
        osj << ""sv;
    }
    if (liqInd_ != LiqInd::None) {
        osj << liqInd_;
    } else {
        osj << ""sv;
    }
    if (!cpty_.empty()) {
        osj << cpty_;
    } else {
        osj << ""sv;
    }
    osj << created_;
}

void Exec::toJson(ostream& os) const
{
    os << "{\"accnt\":\""sv << accnt_       //
       << "\",\"marketId\":"sv << marketId_ //
       << ",\"instr\":\""sv << instr_       //
       << "\",\"settlDate\":"sv;
    if (settlDay_ != 0_jd) {
        os << jdToIso(settlDay_);
    } else {
        os << "null"sv;
    }
    os << ",\"id\":"sv << id_           //
       << ",\"orderId\":"sv << orderId_ //
       << ",\"ref\":"sv;
    if (!ref_.empty()) {
        os << '"' << ref_ << '"';
    } else {
        os << "null"sv;
    }
    os << ",\"state\":\""sv << state_     //
       << "\",\"side\":\""sv << side_     //
       << "\",\"lots\":"sv << lots_       //
       << ",\"ticks\":"sv << ticks_       //
       << ",\"resdLots\":"sv << resdLots_ //
       << ",\"execLots\":"sv << execLots_ //
       << ",\"execCost\":"sv << execCost_;
    if (lastLots_ != 0_lts) {
        os << ",\"lastLots\":"sv << lastLots_ //
           << ",\"lastTicks\":"sv << lastTicks_;
    } else {
        os << ",\"lastLots\":null,\"lastTicks\":null"sv;
    }
    os << ",\"minLots\":"sv;
    if (minLots_ != 0_lts) {
        os << minLots_;
    } else {
        os << "null"sv;
    }
    os << ",\"matchId\":"sv;
    if (matchId_ != 0_id64) {
        os << matchId_;
    } else {
        os << "null"sv;
    }
    os << ",\"liqInd\":"sv;
    if (liqInd_ != LiqInd::None) {
        os << '"' << liqInd_ << '"';
    } else {
        os << "null"sv;
    }
    os << ",\"cpty\":"sv;
    if (!cpty_.empty()) {
        os << '"' << cpty_ << '"';
    } else {
        os << "null"sv;
    }
    os << ",\"created\":"sv << created_ //
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
