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
#ifndef SWIRLY_FIX_CONFIG_HPP
#define SWIRLY_FIX_CONFIG_HPP

#include <swirly/fix/SessId.hpp>

#include <swirly/util/Config.hpp>

#include <map>

namespace swirly {
inline namespace fix {

struct SWIRLY_API FixConfig {

    FixConfig() = default;
    ~FixConfig();

    // Copy.
    FixConfig(const FixConfig& rhs);
    FixConfig& operator=(const FixConfig& rhs);

    // Move.
    FixConfig(FixConfig&&);
    FixConfig& operator=(FixConfig&&);

    void clear() noexcept;
    void read(std::istream& is);

    Config root;
    std::map<FixSessId, Config, FixSessIdCompare> sess_map;
};

template <typename StringT>
BasicFixSessId<StringT> get_sess_id(const Config& config)
{
    auto ver = config.get<Version>("version");
    auto sender_comp_id = config.get("sender_comp_id");
    auto target_comp_id = config.get("target_comp_id");
    return {ver, move(sender_comp_id), move(target_comp_id)};
}

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_CONFIG_HPP
