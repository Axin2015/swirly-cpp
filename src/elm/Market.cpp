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
#include <swirly/elm/Market.hpp>

#include <cassert>

namespace swirly {

Market::Market(const char* mnem, const char* display, const char* contr,
               Jd settlDay, Jd expiryDay, int state) noexcept
:   Rec{RecType::MARKET, mnem, display},
    contr_{contr},
    settlDay_{settlDay},
    expiryDay_{expiryDay},
    state_{state}
{
    assert((settlDay == 0) == (expiryDay == 0));
}

Market::~Market() noexcept = default;

} // swirly
