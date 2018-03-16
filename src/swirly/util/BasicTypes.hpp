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
#ifndef SWIRLY_UTIL_BASICTYPES_HPP
#define SWIRLY_UTIL_BASICTYPES_HPP

#include <swirly/util/IntWrapper.hpp>

namespace swirly {
inline namespace util {
struct Id32Policy : Int32Policy {
};
struct Id64Policy : Int64Policy {
};

/**
 * 32 bit identifier.
 */
using Id32 = IntWrapper<Id32Policy>;

constexpr Id32 operator""_id32(unsigned long long val) noexcept
{
    return Id32{val};
}

/**
 * 64 bit identifier.
 */
using Id64 = IntWrapper<Id64Policy>;

constexpr Id64 operator""_id64(unsigned long long val) noexcept
{
    return Id64{val};
}

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_BASICTYPES_HPP
