/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#include "Conf.hpp"

#include <swirly/util/VarSub.hpp>

using namespace std;

namespace swirly {

Conf::Conf() = default;
Conf::~Conf() noexcept = default;

// Copy.
Conf::Conf(const Conf&) = default;
Conf& Conf::operator=(const Conf&) = default;

// Move.
Conf::Conf(Conf&&) = default;
Conf& Conf::operator=(Conf&&) = default;

void Conf::read(istream& is)
{
    VarSub varSub;
    parsePairs(is, [this, &varSub](const auto& key, string val) {
        varSub(val);
        this->map_.emplace(key, move(val));
    });
}

} // swirly
