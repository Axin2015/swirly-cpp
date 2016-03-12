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
#ifndef SWIRLY_ASH_ENUM_HPP
#define SWIRLY_ASH_ENUM_HPP

#include <iosfwd>
#include <type_traits>

/**
 * @addtogroup Util
 * @{
 */

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

template <typename EnumT>
struct EnumTraits {
  static void print(std::ostream& os, EnumT val) noexcept { os << unbox(val); }
};

template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
std::ostream& operator<<(std::ostream& os, EnumT val) noexcept
{
  EnumTraits<EnumT>::print(os, val);
  return os;
}

namespace enumops {

// Assignment operators

/**
 * Addition assignment.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT& operator+=(EnumT& lhs, EnumT rhs) noexcept
{
  lhs = box<EnumT>(unbox(lhs) + unbox(rhs));
  return lhs;
}

/**
 * Subtraction assignment.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT& operator-=(EnumT& lhs, EnumT rhs) noexcept
{
  lhs = box<EnumT>(unbox(lhs) - unbox(rhs));
  return lhs;
}

/**
 * Multiplication assignment.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT& operator*=(EnumT& lhs, EnumT rhs) noexcept
{
  lhs = box<EnumT>(unbox(lhs) * unbox(rhs));
  return lhs;
}

/**
 * Division assignment.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT& operator/=(EnumT& lhs, EnumT rhs) noexcept
{
  lhs = box<EnumT>(unbox(lhs) / unbox(rhs));
  return lhs;
}

/**
 * Modulo assignment.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT& operator%=(EnumT& lhs, EnumT rhs) noexcept
{
  lhs = box<EnumT>(unbox(lhs) % unbox(rhs));
  return lhs;
}

/**
 * Bitwise AND assignment.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT& operator&=(EnumT& lhs, EnumT rhs) noexcept
{
  lhs = box<EnumT>(unbox(lhs) & unbox(rhs));
  return lhs;
}

/**
 * Bitwise OR assignment.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT& operator|=(EnumT& lhs, EnumT rhs) noexcept
{
  lhs = box<EnumT>(unbox(lhs) | unbox(rhs));
  return lhs;
}

/**
 * Bitwise XOR assignment.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT& operator^=(EnumT& lhs, EnumT rhs) noexcept
{
  lhs = box<EnumT>(unbox(lhs) ^ unbox(rhs));
  return lhs;
}

/**
 * Bitwise left shift assignment.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT& operator<<=(EnumT& lhs, EnumT rhs) noexcept
{
  lhs = box<EnumT>(unbox(lhs) << unbox(rhs));
  return lhs;
}

/**
 * Bitwise right shift assignment.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT& operator>>=(EnumT& lhs, EnumT rhs) noexcept
{
  lhs = box<EnumT>(unbox(lhs) >> unbox(rhs));
  return lhs;
}

// Increment/decrement operators

/**
 * Pre-increment.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT& operator++(EnumT& lhs) noexcept
{
  lhs = box<EnumT>(unbox(lhs) + 1);
  return lhs;
}

/**
 * Pre-decrement.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT& operator--(EnumT& lhs) noexcept
{
  lhs = box<EnumT>(unbox(lhs) - 1);
  return lhs;
}

/**
 * Post-increment.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator++(EnumT& lhs, int)noexcept
{
  const EnumT prev{lhs};
  lhs = box<EnumT>(unbox(lhs) + 1);
  return prev;
}

/**
 * Post-decrement.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator--(EnumT& lhs, int)noexcept
{
  const EnumT prev{lhs};
  lhs = box<EnumT>(unbox(lhs) - 1);
  return prev;
}

// Arithmetic operators

/**
 * Unary plus.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator+(EnumT lhs) noexcept
{
  return box<EnumT>(+unbox(lhs));
}

/**
 * Unary minus.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator-(EnumT lhs) noexcept
{
  return box<EnumT>(-unbox(lhs));
}

/**
 * Addition.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator+(EnumT lhs, EnumT rhs) noexcept
{
  return box<EnumT>(unbox(lhs) + unbox(rhs));
}

/**
 * Subtraction.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator-(EnumT lhs, EnumT rhs) noexcept
{
  return box<EnumT>(unbox(lhs) - unbox(rhs));
}

/**
 * Multiplication.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator*(EnumT lhs, EnumT rhs) noexcept
{
  return box<EnumT>(unbox(lhs) * unbox(rhs));
}

/**
 * Division.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator/(EnumT lhs, EnumT rhs) noexcept
{
  return box<EnumT>(unbox(lhs) / unbox(rhs));
}

/**
 * Modulo.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator%(EnumT lhs, EnumT rhs) noexcept
{
  return box<EnumT>(unbox(lhs) % unbox(rhs));
}

/**
 * Bitwise NOT.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator~(EnumT lhs) noexcept
{
  return box<EnumT>(~unbox(lhs));
}

/**
 * Bitwise AND.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator&(EnumT lhs, EnumT rhs) noexcept
{
  return box<EnumT>(unbox(lhs) & unbox(rhs));
}

/**
 * Bitwise OR.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator|(EnumT lhs, EnumT rhs) noexcept
{
  return box<EnumT>(unbox(lhs) | unbox(rhs));
}

/**
 * Bitwise XOR.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator^(EnumT lhs, EnumT rhs) noexcept
{
  return box<EnumT>(unbox(lhs) ^ unbox(rhs));
}

/**
 * Bitwise left shift.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator<<(EnumT lhs, EnumT rhs) noexcept
{
  return box<EnumT>(unbox(lhs) << unbox(rhs));
}

/**
 * Bitwise right shift.
 */
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>::value>>
constexpr EnumT operator>>(EnumT lhs, EnumT rhs) noexcept
{
  return box<EnumT>(unbox(lhs) >> unbox(rhs));
}

} // enumops

} // swirly

/** @} */

#endif // SWIRLY_ASH_ENUM_HPP
