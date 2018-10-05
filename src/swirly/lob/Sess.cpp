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
#include "Sess.hpp"

namespace swirly {
inline namespace lob {
using namespace std;

static_assert(sizeof(Sess) <= 6 * 64, "no greater than specified cache-lines");

Sess::~Sess() = default;

Sess::Sess(Sess&&) = default;

PosnPtr Sess::posn(Id64 market_id, Symbol instr, JDay settl_day)
{
    auto [it, found] = posns_.find_hint(market_id);
    if (!found) {
        it = posns_.insert_hint(it, Posn::make(accnt_, market_id, instr, settl_day));
    }
    return &*it;
}

} // namespace lob
} // namespace swirly
