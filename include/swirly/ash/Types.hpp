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

#include <swirly/ash/IntWrapper.hpp>

#include <cstdint>

namespace swirly {
struct Id32Policy : Int32Policy {
};
struct Id64Policy : Int64Policy {
};
struct IsoDatePolicy : Int32Policy {
};
struct JDayPolicy : Int32Policy {
};
struct MillisPolicy : Int64Policy {
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

/**
 * ISO8601 date in yyymmdd format.
 */
using IsoDate = IntWrapper<IsoDatePolicy>;

constexpr IsoDate operator""_ymd(unsigned long long val) noexcept
{
  return IsoDate{val};
}

/**
 * Julian day.
 */
using JDay = IntWrapper<JDayPolicy>;

constexpr JDay operator""_jd(unsigned long long val) noexcept
{
  return JDay{val};
}

/**
 * Milliseconds since Unix epoch.
 */
using Millis = IntWrapper<MillisPolicy>;

constexpr Millis operator""_ms(unsigned long long val) noexcept
{
  return Millis{val};
}

} // swirly

#endif // SWIRLY_ASH_TYPES_HPP
