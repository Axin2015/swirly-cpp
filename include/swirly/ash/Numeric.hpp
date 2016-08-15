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
#ifndef SWIRLY_ASH_NUMERIC_HPP
#define SWIRLY_ASH_NUMERIC_HPP

#include <cstdlib>

namespace swirly {
namespace detail {

template <typename ValueT>
struct NumericTraits;

template <>
struct NumericTraits<short> {
  static short cast(const char* str) noexcept { return std::strtol(str, nullptr, 0); }
};

template <>
struct NumericTraits<unsigned short> {
  static unsigned short cast(const char* str) noexcept { return std::strtoul(str, nullptr, 0); }
};

template <>
struct NumericTraits<int> {
  static int cast(const char* str) noexcept { return std::strtol(str, nullptr, 0); }
};

template <>
struct NumericTraits<unsigned int> {
  static unsigned int cast(const char* str) noexcept { return std::strtoul(str, nullptr, 0); }
};

template <>
struct NumericTraits<long> {
  static auto cast(const char* str) noexcept { return std::strtol(str, nullptr, 0); }
};

template <>
struct NumericTraits<unsigned long> {
  static auto cast(const char* str) noexcept { return std::strtoul(str, nullptr, 0); }
};

template <>
struct NumericTraits<long long> {
  static auto cast(const char* str) noexcept { return std::strtoll(str, nullptr, 0); }
};

template <>
struct NumericTraits<unsigned long long> {
  static auto cast(const char* str) noexcept { return std::strtoull(str, nullptr, 0); }
};

} // detail

template <typename ValueT>
inline ValueT numericCast(const char* str) noexcept
{
  return detail::NumericTraits<ValueT>::cast(str);
}

} // swirly

#endif // SWIRLY_ASH_NUMERIC_HPP
