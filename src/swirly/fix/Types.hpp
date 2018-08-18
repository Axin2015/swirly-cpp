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
#ifndef SWIRLY_FIX_TYPES_HPP
#define SWIRLY_FIX_TYPES_HPP

#include <swirly/util/Version.hpp>

namespace swirly {
inline namespace fix {

/**
 * Assumption: begin-string is always FIX.x.y format.
 */
enum : std::size_t { BodyLenStart = "8=FIX.x.y^9="sv.size(), CheckSumLen = "10=000^"sv.size() };

/**
 * Ascii 1: Start Of Header.
 */
constexpr char Soh = '\01';

constexpr Version Fix42{4, 2};
constexpr Version Fix43{4, 3};
constexpr Version Fix44{4, 4};
constexpr Version Fix50{5, 0};

enum class FixMode {
    /**
     * The Initiator establishes the telecommunications link and initiates the session via
     * transmission of the initial Logon message.
     */
    Initiator = 1,
    /**
     * The Acceptor is the receiving party of the FIX session. This party has responsibility to
     * perform first level authentication and formally declare the connection request "accepted"
     * through transmission of an acknowledgment Logon message.
     */
    Acceptor
};
using FixPair = std::pair<int, std::string_view>;

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_TYPES_HPP
