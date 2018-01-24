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
#ifndef SWIRLY_SYS_MEMORY_HPP
#define SWIRLY_SYS_MEMORY_HPP

#include <swirly/sys/Math.hpp>

namespace swirly {

// Assumptions:
// sysconf(_SC_LEVEL1_DCACHE_LINESIZE) == 64
// sysconf(_SC_PAGESIZE) == 4096

enum : std::size_t {
    CacheLineBits = 6,
    CacheLineSize = 1 << CacheLineBits,
    PageBits = 12,
    PageSize = 1 << PageBits
};

constexpr std::size_t ceilCacheLine(std::size_t size) noexcept
{
    return ceilPow2<CacheLineBits>(size);
}

constexpr std::size_t ceilPage(std::size_t size) noexcept
{
    return ceilPow2<PageBits>(size);
}

} // namespace swirly

#endif // SWIRLY_SYS_MEMORY_HPP
