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
#include "Asset.hpp"

#include <swirly/util/Stream.hpp>

namespace swirly {
inline namespace fin {
using namespace std;

static_assert(sizeof(Asset) <= 3 * 64, "no greater than specified cache-lines");

Asset::~Asset() = default;

Asset::Asset(const Asset&) = default;

Asset::Asset(Asset&&) = default;

void Asset::toDsv(ostream& os, char sep) const
{
    OStreamJoiner osj{os, sep};
    osj << id_      //
        << symbol_  //
        << display_ //
        << static_cast<int>(type_);
}

void Asset::toJson(ostream& os) const
{
    os << "{\"symbol\":\"" << symbol_     //
       << "\",\"display\":\"" << display_ //
       << "\",\"type\":\"" << type_       //
       << "\"}";
}

} // namespace fin
} // namespace swirly
