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
#include <swirly/elm/Quote.hpp>

#include <swirly/elm/Order.hpp>

namespace swirly {

Quote::Quote(const StringView& trader, const StringView& market, const StringView& contr,
             Jd settlDay, Iden id, const StringView& ref, const OrderPtr& order, Side side,
             Lots lots, Ticks ticks, Millis created, Millis expiry) noexcept
:   Request{trader, market, contr, settlDay, id, ref, side, lots, created},
    order_{order},
    ticks_{ticks},
    expiry_{expiry}
{
}

Quote::~Quote() noexcept = default;

Quote::Quote(Quote&&) = default;

} // swirly
