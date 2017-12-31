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
#ifndef SWIRLY_SYS_MATH_HPP
#define SWIRLY_SYS_MATH_HPP

#include <cstddef> // size_t
#include <cstdint>

namespace swirly {

/**
 * @return true if size is a power of two.
 */
constexpr bool isPow2(std::size_t n) noexcept
{
    return n > 0 && (n & (n - 1)) == 0;
}

/**
 * @return the next power of two.
 */
inline unsigned nextPow2(unsigned n) noexcept
{
    // The result of __builtin_clz is undefined for zero values.
    return n <= 1 ? 1 : 1 << (sizeof(n) * 8 - __builtin_clz(n - 1));
}

/**
 * @return the next power of two.
 */
inline unsigned long nextPow2(unsigned long n) noexcept
{
    // The result of __builtin_clzl is undefined for zero values.
    return n <= 1 ? 1 : 1 << (sizeof(n) * 8 - __builtin_clzl(n - 1));
}

template <int BitsN>
constexpr std::size_t ceilPow2(std::size_t size) noexcept
{
    enum { Max = (1 << BitsN) - 1 };
    return ((size + Max) >> BitsN) << BitsN;
}

} // namespace swirly

#endif // SWIRLY_SYS_MATH_HPP
