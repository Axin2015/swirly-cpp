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
#ifndef SWIRLY_ASH_STRING_HPP
#define SWIRLY_ASH_STRING_HPP

#include <swirly/ash/Compare.hpp>
#include <swirly/ash/Defs.hpp>

#include <experimental/string_view>

#include <cstring>
#include <sstream>

namespace std {
using string_view = experimental::string_view;
}

namespace swirly {

/**
 * @addtogroup Util
 * @{
 */

constexpr std::string_view operator""_sv(const char* str, std::size_t len) noexcept
{
  return {str, len};
}

template <std::size_t MaxN>
struct StringData {

  // Length in the first cache-line.
  std::size_t len;
  char buf[MaxN];
};

template <std::size_t MaxN>
constexpr std::string_view operator+(const StringData<MaxN>& s) noexcept
{
  return {s.buf, s.len};
}

/**
 * String buffer with fixed upper-bound.
 */
template <std::size_t MaxN>
class StringBuf : protected StringData<MaxN> {
 public:
  using Data = StringData<MaxN>;

  template <std::size_t MaxR>
  constexpr StringBuf(const StringBuf<MaxR>& rhs) noexcept
  {
    *this = rhs;
  }
  constexpr StringBuf(const std::string_view& rhs) noexcept { *this = rhs; }
  constexpr StringBuf() noexcept { len = 0; }

  ~StringBuf() noexcept = default;

  // Copy.
  constexpr StringBuf(const StringBuf& rhs) noexcept { *this = rhs; }
  constexpr StringBuf& operator=(const StringBuf& rhs) noexcept
  {
    len = rhs.size();
    std::memcpy(buf, rhs.data(), len);
    return *this;
  }

  // Move.
  constexpr StringBuf(StringBuf&&) noexcept = default;
  constexpr StringBuf& operator=(StringBuf&&) noexcept = default;

  template <std::size_t MaxR>
  constexpr StringBuf& operator=(const StringBuf<MaxR>& rhs) noexcept
  {
    len = std::min(rhs.size(), MaxN);
    std::memcpy(buf, rhs.data(), len);
    return *this;
  }
  constexpr StringBuf& operator=(const std::string_view& rhs) noexcept
  {
    len = std::min(rhs.size(), MaxN);
    std::memcpy(buf, rhs.data(), len);
    return *this;
  }
  template <std::size_t MaxR>
  constexpr int compare(const StringBuf<MaxR>& rhs) const noexcept
  {
    int result{std::memcmp(buf, rhs.data(), std::min(len, rhs.size()))};
    if (result == 0)
      result = swirly::compare(len, rhs.size());
    return result;
  }
  constexpr int compare(const std::string_view& rhs) const noexcept
  {
    int result{std::memcmp(buf, rhs.data(), std::min(len, rhs.size()))};
    if (result == 0)
      result = swirly::compare(len, rhs.size());
    return result;
  }
  constexpr const char* data() const noexcept { return buf; }
  constexpr bool empty() const noexcept { return len == 0; }
  constexpr size_t size() const noexcept { return len; }

 private:
  using StringData<MaxN>::len;
  using StringData<MaxN>::buf;
};

template <std::size_t MaxN>
constexpr std::string_view operator+(const StringBuf<MaxN>& s) noexcept
{
  return {s.data(), s.size()};
}

template <std::size_t MaxL, std::size_t MaxR>
constexpr bool operator==(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
  return lhs.compare(rhs) == 0;
}

template <std::size_t MaxN>
constexpr bool operator==(const StringBuf<MaxN>& lhs, const std::string_view& rhs) noexcept
{
  return lhs.compare(rhs) == 0;
}

template <std::size_t MaxN>
constexpr bool operator==(const std::string_view& lhs, const StringBuf<MaxN>& rhs) noexcept
{
  return 0 == rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
constexpr bool operator!=(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
  return lhs.compare(rhs) != 0;
}

template <std::size_t MaxN>
constexpr bool operator!=(const StringBuf<MaxN>& lhs, const std::string_view& rhs) noexcept
{
  return lhs.compare(rhs) != 0;
}

template <std::size_t MaxN>
constexpr bool operator!=(const std::string_view& lhs, const StringBuf<MaxN>& rhs) noexcept
{
  return 0 != rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
constexpr bool operator<(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
  return lhs.compare(rhs) < 0;
}

template <std::size_t MaxN>
constexpr bool operator<(const StringBuf<MaxN>& lhs, const std::string_view& rhs) noexcept
{
  return lhs.compare(rhs) < 0;
}

template <std::size_t MaxN>
constexpr bool operator<(const std::string_view& lhs, const StringBuf<MaxN>& rhs) noexcept
{
  return 0 < rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
constexpr bool operator<=(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
  return lhs.compare(rhs) <= 0;
}

template <std::size_t MaxN>
constexpr bool operator<=(const StringBuf<MaxN>& lhs, const std::string_view& rhs) noexcept
{
  return lhs.compare(rhs) <= 0;
}

template <std::size_t MaxN>
constexpr bool operator<=(const std::string_view& lhs, const StringBuf<MaxN>& rhs) noexcept
{
  return 0 <= rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
constexpr bool operator>(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
  return lhs.compare(rhs) > 0;
}

template <std::size_t MaxN>
constexpr bool operator>(const StringBuf<MaxN>& lhs, const std::string_view& rhs) noexcept
{
  return lhs.compare(rhs) > 0;
}

template <std::size_t MaxN>
constexpr bool operator>(const std::string_view& lhs, const StringBuf<MaxN>& rhs) noexcept
{
  return 0 > rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
constexpr bool operator>=(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
  return lhs.compare(rhs) >= 0;
}

template <std::size_t MaxN>
constexpr bool operator>=(const StringBuf<MaxN>& lhs, const std::string_view& rhs) noexcept
{
  return lhs.compare(rhs) >= 0;
}

template <std::size_t MaxN>
constexpr bool operator>=(const std::string_view& lhs, const StringBuf<MaxN>& rhs) noexcept
{
  return 0 >= rhs.compare(lhs);
}

template <std::size_t MaxN>
constexpr std::ostream& operator<<(std::ostream& os, const StringBuf<MaxN>& rhs) noexcept
{
  return os << +rhs;
}

template <typename ValueT, typename std::enable_if_t<std::is_arithmetic<ValueT>::value>* = nullptr>
std::string toString(ValueT val)
{
  return std::to_string(val);
}

template <typename ValueT, typename std::enable_if_t<!std::is_arithmetic<ValueT>::value>* = nullptr>
std::string toString(const ValueT& val)
{
  std::stringstream ss;
  ss << val;
  return ss.str();
}

/** @} */

} // swirly

#endif // SWIRLY_ASH_STRING_HPP
