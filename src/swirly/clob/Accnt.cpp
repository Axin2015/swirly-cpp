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
#include "Accnt.hpp"

namespace swirly {
inline namespace clob {
using namespace std;

static_assert(sizeof(Accnt) <= 6 * 64, "no greater than specified cache-lines");

Accnt::~Accnt() = default;

Accnt::Accnt(Accnt&&) = default;

PosnPtr Accnt::posn(Id64 marketId, Symbol instr, JDay settlDay)
{
    PosnSet::Iterator it;
    bool found;
    tie(it, found) = posns_.findHint(marketId);
    if (!found) {
        it = posns_.insertHint(it, Posn::make(symbol_, marketId, instr, settlDay));
    }
    return &*it;
}

} // namespace clob
} // namespace swirly
