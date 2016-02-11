/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#ifndef SWIRLY_ASH_TYPES_HPP
#define SWIRLY_ASH_TYPES_HPP

#include <swirly/ash/Enum.hpp>

#include <cstdint>

namespace swirly {

/**
 * @addtogroup Date
 * @{
 */

/**
 * ISO8601 date in yyymmdd format.
 */
enum class IsoDate : int {};

constexpr IsoDate operator""_dt(unsigned long long val) noexcept
{
  return box<IsoDate>(val);
}

/**
 * Julian day.
 */
enum class Jday : int {};

constexpr Jday operator""_jd(unsigned long long val) noexcept
{
  return box<Jday>(val);
}

/**
 * Milliseconds since Unix epoch.
 */
enum class Millis : int64_t {};

constexpr Millis operator""_ms(unsigned long long val) noexcept
{
  return box<Millis>(val);
}

/** @} */

} // swirly

#endif // SWIRLY_ASH_TYPES_HPP
