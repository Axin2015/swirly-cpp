/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
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
#include <swirly/elm/Order.hpp>

namespace swirly {

Order::~Order() noexcept = default;

Order::Order(Order&&) = default;

OrderList::~OrderList() noexcept
{
    list_.clear_and_dispose([](Order* ptr) { ptr->release(); });
}

OrderList::OrderList(OrderList&&) = default;

OrderList& OrderList::operator =(OrderList&&) = default;

OrderList::Iterator OrderList::insertBack(const OrderPtr& value) noexcept
{
    list_.push_back(*value);
    // Take ownership.
    value->addRef();
    return List::s_iterator_to(*value);
}

OrderList::Iterator OrderList::insertBefore(const OrderPtr& value, const Order& next) noexcept
{
    auto it = list_.insert(List::s_iterator_to(next), *value);
    // Take ownership.
    value->addRef();
    return it;
}

void OrderList::remove(const Order& order) noexcept
{
    list_.erase_and_dispose(List::s_iterator_to(order),
                            [](Order* ptr) { ptr->release(); });
}

} // swirly
