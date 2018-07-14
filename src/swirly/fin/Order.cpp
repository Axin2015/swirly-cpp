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
#include "Order.hpp"

#include <swirly/util/Date.hpp>
#include <swirly/util/Stream.hpp>

namespace swirly {
inline namespace fin {
using namespace std;

static_assert(sizeof(Order) <= 6 * 64, "no greater than specified cache-lines");

Order::~Order() = default;

Order::Order(Order&&) = default;

void Order::to_dsv(ostream& os, char delim) const
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
    osj << id_;
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
    osj << created_ //
        << modified_;
}

void Order::to_json(ostream& os) const
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
    os << ",\"id\":" << id_ //
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
    os << ",\"created\":" << created_   //
       << ",\"modified\":" << modified_ //
       << '}';
}

OrderRefSet::~OrderRefSet()
{
    set_.clear_and_dispose([](const Order* ptr) { ptr->release(); });
}

OrderRefSet::OrderRefSet(OrderRefSet&&) = default;

OrderRefSet& OrderRefSet::operator=(OrderRefSet&&) = default;

OrderRefSet::Iterator OrderRefSet::insert(const ValuePtr& value) noexcept
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

OrderRefSet::Iterator OrderRefSet::insert_hint(ConstIterator hint, const ValuePtr& value) noexcept
{
    auto it = set_.insert(hint, *value);
    // Take ownership.
    value->add_ref();
    return it;
}

OrderRefSet::Iterator OrderRefSet::insert_or_replace(const ValuePtr& value) noexcept
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

OrderList::~OrderList()
{
    list_.clear_and_dispose([](const Order* ptr) { ptr->release(); });
}

OrderList::OrderList(OrderList&&) = default;

OrderList& OrderList::operator=(OrderList&&) = default;

OrderList::Iterator OrderList::insert_back(const OrderPtr& value) noexcept
{
    list_.push_back(*value);
    // Take ownership.
    value->add_ref();
    return List::s_iterator_to(*value);
}

OrderList::Iterator OrderList::insert_before(const OrderPtr& value, const Order& next) noexcept
{
    auto it = list_.insert(List::s_iterator_to(next), *value);
    // Take ownership.
    value->add_ref();
    return it;
}

OrderList::ValuePtr OrderList::remove(const Order& ref) noexcept
{
    ValuePtr value;
    list_.erase_and_dispose(List::s_iterator_to(ref), [&value](Order* ptr) {
        value = ValuePtr{ptr, false};
    });
    return value;
}

} // namespace fin
} // namespace swirly
