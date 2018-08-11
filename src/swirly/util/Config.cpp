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
#include "Config.hpp"

#include <swirly/util/VarSub.hpp>

namespace swirly {
inline namespace util {
using namespace std;

Config::Config() = default;
Config::~Config() = default;

// Copy.
Config::Config(const Config&) = default;
Config& Config::operator=(const Config&) = default;

// Move.
Config::Config(Config&&) = default;
Config& Config::operator=(Config&&) = default;

istream& Config::read_section(istream& is, string* next)
{
    VarSub var_sub;
    return parse_section(is,
                         [this, &var_sub](const auto& key, string val) {
                             var_sub(val);
                             map_.emplace(key, move(val));
                         },
                         next);
}

} // namespace util
} // namespace swirly
