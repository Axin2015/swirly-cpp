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
#ifndef SWIRLY_FIG_SERV_HPP
#define SWIRLY_FIG_SERV_HPP

#include <swirly/ash/Defs.hpp>

namespace swirly {

class SWIRLY_API Serv {
public:
    constexpr Serv() noexcept = default;
    ~Serv() noexcept;

    // Copy.
    constexpr Serv(const Serv&) = default;
    Serv& operator =(const Serv&) = default;

    // Move.
    constexpr Serv(Serv&&) = default;
    Serv& operator =(Serv&&) = default;
};

} // swirly

#endif // SWIRLY_FIG_SERV_HPP
