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
#include <swirly/elm/Level.hpp>

#include <swirly/elm/Order.hpp>

namespace swirly {

Level::Level(const Order& firstOrder) noexcept
:   firstOrder_{&firstOrder},
    key_{detail::composeKey(firstOrder.side(), firstOrder.ticks())},
    ticks_{firstOrder.ticks()},
    resd_{firstOrder.resd()},
    quotd_{firstOrder.quotd()},
    count_{1}
{
}

Level::~Level() noexcept = default;

Level::Level(Level&&) = default;

LevelSet::~LevelSet() noexcept
{
    set_.clear_and_dispose([](Level* ptr) { delete ptr; });
}

LevelSet::LevelSet(LevelSet&&) = default;

LevelSet& LevelSet::operator =(LevelSet&&) = default;

Level& LevelSet::insert(ValuePtr rec) noexcept
{
    auto result = set_.insert(*rec);
    if (result.second) {
        // Take ownership if inserted.
        rec.release();
    }
    return *result.first;
}

Level& LevelSet::insertOrReplace(ValuePtr rec) noexcept
{
    auto result = set_.insert(*rec);
    if (!result.second) {
        // Replace if exists.
        auto* prev = &*result.first;
        set_.replace_node(result.first, *rec);
        delete prev;
    }
    // Take ownership.
    return *rec.release();
}

} // swirly
