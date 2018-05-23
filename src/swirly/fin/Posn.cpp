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

void Posn::toDsv(ostream& os, char delim) const
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
    if (buyLots_ != 0_lts) {
        osj << buyLots_ //
            << buyCost_;
    } else {
        osj << '0' << '0';
    }
    if (sellLots_ != 0_lts) {
        osj << sellLots_ //
            << sellCost_;
    } else {
        osj << '0' << '0';
    }
}

void Posn::toJson(ostream& os) const
{
    os << "{\"accnt\":\""sv << accnt_        //
       << "\",\"market_id\":"sv << marketId_ //
       << ",\"instr\":\""sv << instr_        //
       << "\",\"settl_date\":"sv;
    if (settlDay_ != 0_jd) {
        os << jdToIso(settlDay_);
    } else {
        os << "null"sv;
    }
    if (buyLots_ != 0_lts) {
        os << ",\"buy_lots\":"sv << buyLots_ //
           << ",\"buy_cost\":"sv << buyCost_;
    } else {
        os << ",\"buy_lots\":0,\"buy_cost\":0"sv;
    }
    if (sellLots_ != 0_lts) {
        os << ",\"sell_lots\":"sv << sellLots_ //
           << ",\"sell_cost\":"sv << sellCost_;
    } else {
        os << ",\"sell_lots\":0,\"sell_cost\":0"sv;
    }
    os << '}';
}

void Posn::addBuy(Lots lots, Ticks ticks) noexcept
{
    //   -2  -1   0   1
    //    +-------+---+
    //    |-->    |   |     netLots < 0
    //    |------>|   |     netLots == 0
    //    |---------->|     netLots > 0 && netLots < lots
    //    |       |   |-->  netLots >= lots

    const auto cost = swirly::cost(lots, ticks);
    const auto netLots = this->netLots() + lots;
    buyLots_ += lots;
    buyCost_ += cost;
    if (netLots < 0_lts || netLots >= lots) {
        // Either short position was partially closed.
        // Or long position was extended.
        netCost_ += cost;
    } else if (netLots > 0_lts) {
        assert(netLots < lots);
        // Short position was fully closed and long position opened.
        netCost_ = swirly::cost(netLots, ticks);
    } else {
        assert(netLots == 0_lts);
        // Short position was fully closed.
        netCost_ = 0_cst;
    }
}

void Posn::addSell(Lots lots, Ticks ticks) noexcept
{
    //   -1   0   1   2
    //    +-------+---+
    //    |   |    <--|     netLots > 0
    //    |   |<------|     netLots == 0
    //    |<----------|     netLots < 0 && netLots > -lots
    // <--|   |       |     netLots <= -lots

    const auto cost = swirly::cost(lots, ticks);
    const auto netLots = this->netLots() - lots;
    sellLots_ += lots;
    sellCost_ += cost;
    if (netLots > 0_lts || netLots <= -lots) {
        // Either long position was partially closed.
        // Or short position was extended.
        netCost_ -= cost;
    } else if (netLots < 0_lts) {
        assert(netLots > -lots);
        // Long position was fully closed and short position opened.
        netCost_ = swirly::cost(netLots, ticks);
    } else {
        assert(netLots == 0_lts);
        // Long position was fully closed.
        netCost_ = 0_cst;
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
        value->addRef();
    }
    return it;
}

PosnSet::Iterator PosnSet::insertHint(ConstIterator hint, const ValuePtr& value) noexcept
{
    auto it = set_.insert(hint, *value);
    // Take ownership.
    value->addRef();
    return it;
}

PosnSet::Iterator PosnSet::insertOrReplace(const ValuePtr& value) noexcept
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
    value->addRef();
    return it;
}

} // namespace fin
} // namespace swirly
