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
#ifndef SWIRLY_UTIL_ENUM_HPP
#define SWIRLY_UTIL_ENUM_HPP

#include <iosfwd>
#include <type_traits>

namespace swirly {

template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT box(typename std::underlying_type_t<EnumT> val) noexcept
{
  return static_cast<EnumT>(val);
}

template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr typename std::underlying_type_t<EnumT> unbox(EnumT val) noexcept
{
  return static_cast<typename std::underlying_type_t<EnumT>>(val);
}

template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
std::ostream& operator<<(std::ostream& os, EnumT val)
{
  return os << unbox(val);
}

} // swirly

#endif // SWIRLY_UTIL_ENUM_HPP
