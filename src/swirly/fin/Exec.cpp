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

void Exec::to_dsv(ostream& os, char delim) const
{
    OStreamJoiner osj{os, delim};
    osj << accnt_     //
        << market_id_ //
        << instr_;
    if (settl_day_ != 0_jd) {
        osj << jd_to_iso(settl_day_);
    } else {
        osj << ""sv;
    }
    osj << id_ //
        << order_id_;
    if (!ref_.empty()) {
        osj << ref_;
    } else {
        osj << ""sv;
    }
    osj << state_     //
        << side_      //
        << lots_      //
        << ticks_     //
        << resd_lots_ //
        << exec_lots_ //
        << exec_cost_;
    if (last_lots_ != 0_lts) {
        osj << last_lots_ //
            << last_ticks_;
    } else {
        osj << ""sv
            << ""sv;
    }
    if (min_lots_ != 0_lts) {
        osj << min_lots_;
    } else {
        osj << ""sv;
    }
    if (match_id_ != 0_id64) {
        osj << match_id_;
    } else {
        osj << ""sv;
    }
    if (state_ == State::Trade) {
        osj << posn_lots_ << posn_cost_;
    } else {
        osj << ""sv
            << ""sv;
    }
    if (liq_ind_ != LiqInd::None) {
        osj << liq_ind_;
    } else {
        osj << ""sv;
    }
    osj << cpty_ //
        << created_;
}

void Exec::to_json(ostream& os) const
{
    os << "{\"accnt\":\""sv << accnt_         //
       << "\",\"market_id\":"sv << market_id_ //
       << ",\"instr\":\""sv << instr_         //
       << "\",\"settl_date\":"sv;
    if (settl_day_ != 0_jd) {
        os << jd_to_iso(settl_day_);
    } else {
        os << "null"sv;
    }
    os << ",\"id\":"sv << id_             //
       << ",\"order_id\":"sv << order_id_ //
       << ",\"ref\":"sv;
    if (!ref_.empty()) {
        os << '"' << ref_ << '"';
    } else {
        os << "null"sv;
    }
    os << ",\"state\":\""sv << state_       //
       << "\",\"side\":\""sv << side_       //
       << "\",\"lots\":"sv << lots_         //
       << ",\"ticks\":"sv << ticks_         //
       << ",\"resd_lots\":"sv << resd_lots_ //
       << ",\"exec_lots\":"sv << exec_lots_ //
       << ",\"exec_cost\":"sv << exec_cost_;
    if (last_lots_ != 0_lts) {
        os << ",\"last_lots\":"sv << last_lots_ //
           << ",\"last_ticks\":"sv << last_ticks_;
    } else {
        os << ",\"last_lots\":null,\"last_ticks\":null"sv;
    }
    os << ",\"min_lots\":"sv;
    if (min_lots_ != 0_lts) {
        os << min_lots_;
    } else {
        os << "null"sv;
    }
    os << ",\"match_id\":"sv;
    if (match_id_ != 0_id64) {
        os << match_id_;
    } else {
        os << "null"sv;
    }
    if (state_ == State::Trade) {
        os << ",\"posn_lots\":"sv << posn_lots_ //
           << ",\"posn_cost\":"sv << posn_cost_;
    } else {
        os << ",\"posn_lots\":null,\"posn_cost\":null"sv;
    }
    os << ",\"liq_ind\":"sv;
    if (liq_ind_ != LiqInd::None) {
        os << '"' << liq_ind_ << '"';
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
    return make(cpty_, market_id_, instr_, settl_day_, id, order_id_, +ref_, state_,
                swirly::opposite(side_), lots_, ticks_, resd_lots_, exec_lots_, exec_cost_,
                last_lots_, last_ticks_, min_lots_, match_id_, posn_lots_, posn_cost_,
                swirly::opposite(liq_ind_), accnt_, created_);
}

void Exec::trade(Lots sum_lots, Cost sum_cost, Lots last_lots, Ticks last_ticks, Id64 match_id,
                 LiqInd liq_ind, Symbol cpty) noexcept
{
    state_ = State::Trade;
    resd_lots_ -= sum_lots;
    exec_lots_ += sum_lots;
    exec_cost_ += sum_cost;
    last_lots_ = last_lots;
    last_ticks_ = last_ticks;
    match_id_ = match_id;
    liq_ind_ = liq_ind;
    cpty_ = cpty;
}

} // namespace fin
} // namespace swirly
