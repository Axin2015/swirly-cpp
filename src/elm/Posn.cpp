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
#include <swirly/elm/Posn.hpp>

namespace swirly {

Posn::~Posn() noexcept = default;

Posn::Posn(Posn&&) = default;

TraderPosnSet::~TraderPosnSet() noexcept
{
    set_.clear_and_dispose([](Posn* ptr) { ptr->release(); });
}

TraderPosnSet::TraderPosnSet(TraderPosnSet&&) = default;

TraderPosnSet& TraderPosnSet::operator =(TraderPosnSet&&) = default;

TraderPosnSet::ValuePtr TraderPosnSet::insert(const ValuePtr& request) noexcept
{
    auto result = set_.insert(*request);
    if (result.second) {
        // Take ownership if inserted.
        request->addRef();
    }
    return &*result.first;
}

TraderPosnSet::ValuePtr TraderPosnSet::insertOrReplace(const ValuePtr& request) noexcept
{
    auto result = set_.insert(*request);
    if (!result.second) {
        // Replace if exists.
        auto& prev = *result.first;
        set_.replace_node(result.first, *request);
        prev.release();
    }
    // Take ownership.
    request->addRef();
    return request;
}

} // swirly
