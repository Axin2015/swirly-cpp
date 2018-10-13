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
#include "Random.hpp"

namespace swirly {
inline namespace fix {
using namespace std;

RandomBbo::~RandomBbo() = default;

pair<Ticks, Ticks> RandomBbo::operator()(Ticks open)
{
    if ((gen_() & 0x7) == 0) {
        offset_ += dist_(gen_) - 2;
    }
    const Ticks delta{offset_ + dist_(gen_) - 2};
    const Ticks spread{dist_(gen_) / 2 + 1};
    return {open + delta - spread, open + delta + spread};
}

} // namespace fix
} // namespace swirly
