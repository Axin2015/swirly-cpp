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
#include "Posn.hpp"

#include <swirly/util/Date.hpp>
#include <swirly/util/Stream.hpp>
#include <swirly/util/Symbol.hpp>

namespace swirly {
inline namespace fin {
using namespace std;

static_assert(sizeof(Posn) <= 2 * 64, "no greater than specified cache-lines");

Posn::~Posn() = default;

Posn::Posn(Posn&&) = default;

void Posn::to_dsv(ostream& os, char delim) const
{
    OStreamJoiner osj{os, delim};
    osj << accnt_     //
        << market_id_ //
        << instr_;
    if (settl_day_ != 0_jd) {
        osj << jd_to_iso(settl_day_);
    } else {
        osj << "";
    }
    if (buy_lots_ != 0_lts) {
        osj << buy_lots_ //
            << buy_cost_;
    } else {
        osj << '0' << '0';
    }
    if (sell_lots_ != 0_lts) {
        osj << sell_lots_ //
            << sell_cost_;
    } else {
        osj << '0' << '0';
    }
}

void Posn::to_json(ostream& os) const
{
    os << "{\"accnt\":\"" << accnt_         //
       << "\",\"market_id\":" << market_id_ //
       << ",\"instr\":\"" << instr_         //
       << "\",\"settl_date\":";
    if (settl_day_ != 0_jd) {
        os << jd_to_iso(settl_day_);
    } else {
        os << "null";
    }
    if (buy_lots_ != 0_lts) {
        os << ",\"buy_lots\":" << buy_lots_ //
           << ",\"buy_cost\":" << buy_cost_;
    } else {
        os << ",\"buy_lots\":0,\"buy_cost\":0";
    }
    if (sell_lots_ != 0_lts) {
        os << ",\"sell_lots\":" << sell_lots_ //
           << ",\"sell_cost\":" << sell_cost_;
    } else {
        os << ",\"sell_lots\":0,\"sell_cost\":0";
    }
    os << ",\"open_cost\":" << open_cost_ << '}';
}

void Posn::add_buy(Lots lots, Ticks ticks) noexcept
{
    //   -2  -1   0   1
    //    +-------+---+
    //    |-->    |   |     open_lots < 0
    //    |------>|   |     open_lots == 0
    //    |---------->|     open_lots > 0 && open_lots < lots
    //    |       |   |-->  open_lots >= lots

    const auto cost = swirly::cost(lots, ticks);
    const auto open_lots = this->open_lots() + lots;
    buy_lots_ += lots;
    buy_cost_ += cost;
    if (open_lots < 0_lts || open_lots >= lots) {
        // Either short position was partially closed.
        // Or long position was extended.
        open_cost_ += cost;
    } else if (open_lots > 0_lts) {
        assert(open_lots < lots);
        // Short position was fully closed and long position opened.
        open_cost_ = swirly::cost(open_lots, ticks);
    } else {
        assert(open_lots == 0_lts);
        // Short position was fully closed.
        open_cost_ = 0_cst;
    }
}

void Posn::add_sell(Lots lots, Ticks ticks) noexcept
{
    //   -1   0   1   2
    //    +-------+---+
    //    |   |    <--|     open_lots > 0
    //    |   |<------|     open_lots == 0
    //    |<----------|     open_lots < 0 && open_lots > -lots
    // <--|   |       |     open_lots <= -lots

    const auto cost = swirly::cost(lots, ticks);
    const auto open_lots = this->open_lots() - lots;
    sell_lots_ += lots;
    sell_cost_ += cost;
    if (open_lots > 0_lts || open_lots <= -lots) {
        // Either long position was partially closed.
        // Or short position was extended.
        open_cost_ -= cost;
    } else if (open_lots < 0_lts) {
        assert(open_lots > -lots);
        // Long position was fully closed and short position opened.
        open_cost_ = swirly::cost(open_lots, ticks);
    } else {
        assert(open_lots == 0_lts);
        // Long position was fully closed.
        open_cost_ = 0_cst;
    }
}

PosnSet::~PosnSet()
{
    set_.clear_and_dispose([](const Posn* ptr) { ptr->release(); });
}

PosnSet::PosnSet(PosnSet&&) = default;

PosnSet& PosnSet::operator=(PosnSet&&) = default;

PosnSet::Iterator PosnSet::insert(const ValuePtr& value) noexcept
{
    auto [it, inserted] = set_.insert(*value);
    if (inserted) {
        // Take ownership if inserted.
        value->add_ref();
    }
    return it;
}

PosnSet::Iterator PosnSet::insert_hint(ConstIterator hint, const ValuePtr& value) noexcept
{
    auto it = set_.insert(hint, *value);
    // Take ownership.
    value->add_ref();
    return it;
}

PosnSet::Iterator PosnSet::insert_or_replace(const ValuePtr& value) noexcept
{
    auto [it, inserted] = set_.insert(*value);
    if (!inserted) {
        // Replace if exists.
        ValuePtr prev{&*it, false};
        set_.replace_node(it, *value);
        it = Set::s_iterator_to(*value);
    }
    // Take ownership.
    value->add_ref();
    return it;
}

} // namespace fin
} // namespace swirly
