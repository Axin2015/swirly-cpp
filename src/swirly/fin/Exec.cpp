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
    osj << created_   //
        << accnt_     //
        << market_id_ //
        << product_;
    if (settl_day_ != 0_jd) {
        osj << jd_to_iso(settl_day_);
    } else {
        osj << "";
    }
    osj << id_ //
        << order_id_;
    if (!ref_.empty()) {
        osj << ref_;
    } else {
        osj << "";
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
        osj << ""
            << "";
    }
    if (min_lots_ != 0_lts) {
        osj << min_lots_;
    } else {
        osj << "";
    }
    if (match_id_ != 0_id64) {
        osj << match_id_;
    } else {
        osj << "";
    }
    if (state_ == State::Trade) {
        osj << posn_lots_ << posn_cost_;
    } else {
        osj << ""
            << "";
    }
    if (liq_ind_ != LiqInd::None) {
        osj << liq_ind_;
    } else {
        osj << "";
    }
    osj << cpty_;
}

void Exec::to_json(ostream& os) const
{
    os << "{\"created\":" << created_       //
       << ",\"accnt\":\"" << accnt_         //
       << "\",\"market_id\":" << market_id_ //
       << ",\"product\":\"" << product_     //
       << "\",\"settl_date\":";
    if (settl_day_ != 0_jd) {
        os << jd_to_iso(settl_day_);
    } else {
        os << "null";
    }
    os << ",\"id\":" << id_             //
       << ",\"order_id\":" << order_id_ //
       << ",\"ref\":";
    if (!ref_.empty()) {
        os << '"' << ref_ << '"';
    } else {
        os << "null";
    }
    os << ",\"state\":\"" << state_       //
       << "\",\"side\":\"" << side_       //
       << "\",\"lots\":" << lots_         //
       << ",\"ticks\":" << ticks_         //
       << ",\"resd_lots\":" << resd_lots_ //
       << ",\"exec_lots\":" << exec_lots_ //
       << ",\"exec_cost\":" << exec_cost_;
    if (last_lots_ != 0_lts) {
        os << ",\"last_lots\":" << last_lots_ //
           << ",\"last_ticks\":" << last_ticks_;
    } else {
        os << ",\"last_lots\":null,\"last_ticks\":null";
    }
    os << ",\"min_lots\":";
    if (min_lots_ != 0_lts) {
        os << min_lots_;
    } else {
        os << "null";
    }
    os << ",\"match_id\":";
    if (match_id_ != 0_id64) {
        os << match_id_;
    } else {
        os << "null";
    }
    if (state_ == State::Trade) {
        os << ",\"posn_lots\":" << posn_lots_ //
           << ",\"posn_cost\":" << posn_cost_;
    } else {
        os << ",\"posn_lots\":null,\"posn_cost\":null";
    }
    os << ",\"liq_ind\":";
    if (liq_ind_ != LiqInd::None) {
        os << '"' << liq_ind_ << '"';
    } else {
        os << "null";
    }
    os << ",\"cpty\":";
    if (!cpty_.empty()) {
        os << '"' << cpty_ << '"';
    } else {
        os << "null";
    }
    os << '}';
}

ExecPtr Exec::opposite(Id64 id) const
{
    assert(!cpty_.empty());
    return make(created_, cpty_, market_id_, product_, settl_day_, id, order_id_, +ref_, state_,
                swirly::opposite(side_), lots_, ticks_, resd_lots_, exec_lots_, exec_cost_,
                last_lots_, last_ticks_, min_lots_, match_id_, posn_lots_, posn_cost_,
                swirly::opposite(liq_ind_), accnt_);
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
