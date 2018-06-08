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
        osj << ""sv;
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
    os << "{\"accnt\":\""sv << accnt_         //
       << "\",\"market_id\":"sv << market_id_ //
       << ",\"instr\":\""sv << instr_         //
       << "\",\"settl_date\":"sv;
    if (settl_day_ != 0_jd) {
        os << jd_to_iso(settl_day_);
    } else {
        os << "null"sv;
    }
    if (buy_lots_ != 0_lts) {
        os << ",\"buy_lots\":"sv << buy_lots_ //
           << ",\"buy_cost\":"sv << buy_cost_;
    } else {
        os << ",\"buy_lots\":0,\"buy_cost\":0"sv;
    }
    if (sell_lots_ != 0_lts) {
        os << ",\"sell_lots\":"sv << sell_lots_ //
           << ",\"sell_cost\":"sv << sell_cost_;
    } else {
        os << ",\"sell_lots\":0,\"sell_cost\":0"sv;
    }
    os << '}';
}

void Posn::add_buy(Lots lots, Ticks ticks) noexcept
{
    //   -2  -1   0   1
    //    +-------+---+
    //    |-->    |   |     net_lots < 0
    //    |------>|   |     net_lots == 0
    //    |---------->|     net_lots > 0 && net_lots < lots
    //    |       |   |-->  net_lots >= lots

    const auto cost = swirly::cost(lots, ticks);
    const auto net_lots = this->net_lots() + lots;
    buy_lots_ += lots;
    buy_cost_ += cost;
    if (net_lots < 0_lts || net_lots >= lots) {
        // Either short position was partially closed.
        // Or long position was extended.
        net_cost_ += cost;
    } else if (net_lots > 0_lts) {
        assert(net_lots < lots);
        // Short position was fully closed and long position opened.
        net_cost_ = swirly::cost(net_lots, ticks);
    } else {
        assert(net_lots == 0_lts);
        // Short position was fully closed.
        net_cost_ = 0_cst;
    }
}

void Posn::add_sell(Lots lots, Ticks ticks) noexcept
{
    //   -1   0   1   2
    //    +-------+---+
    //    |   |    <--|     net_lots > 0
    //    |   |<------|     net_lots == 0
    //    |<----------|     net_lots < 0 && net_lots > -lots
    // <--|   |       |     net_lots <= -lots

    const auto cost = swirly::cost(lots, ticks);
    const auto net_lots = this->net_lots() - lots;
    sell_lots_ += lots;
    sell_cost_ += cost;
    if (net_lots > 0_lts || net_lots <= -lots) {
        // Either long position was partially closed.
        // Or short position was extended.
        net_cost_ -= cost;
    } else if (net_lots < 0_lts) {
        assert(net_lots > -lots);
        // Long position was fully closed and short position opened.
        net_cost_ = swirly::cost(net_lots, ticks);
    } else {
        assert(net_lots == 0_lts);
        // Long position was fully closed.
        net_cost_ = 0_cst;
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
    Iterator it;
    bool inserted;
    tie(it, inserted) = set_.insert(*value);
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
    Iterator it;
    bool inserted;
    tie(it, inserted) = set_.insert(*value);
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
