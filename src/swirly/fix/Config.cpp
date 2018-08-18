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

namespace swirly {
inline namespace fix {
using namespace std;

FixConfig::~FixConfig() = default;

FixConfig::FixConfig(const FixConfig& rhs) = default;
FixConfig& FixConfig::operator=(const FixConfig& rhs) = default;

FixConfig::FixConfig(FixConfig&&) = default;
FixConfig& FixConfig::operator=(FixConfig&&) = default;

void FixConfig::clear() noexcept
{
    root.clear();
    sess_map.clear();
}

void FixConfig::read(istream& is)
{
    string next;
    if (root.read_section(is, next).eof()) {
        return;
    }
    do {
        if (next != "session") {
            throw runtime_error{"invalid config section: "s + next};
        }
        Config config;
        config.set_parent(root);
        config.read_section(is, next);
        sess_map.emplace(get_sess_id<string>(config), move(config));
        if (is.eof()) {
            return;
        }
    } while (!is.eof());
}

} // namespace fix
} // namespace swirly
