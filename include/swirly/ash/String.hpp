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
using experimental::string_view;
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
  // Use int to save space.
  int len;
  char buf[MaxN];
};

template <std::size_t MaxN>
constexpr std::string_view operator+(const StringData<MaxN>& s) noexcept
{
  return {s.buf, static_cast<std::size_t>(s.len)};
}

/**
 * String buffer with fixed upper-bound.
 */
template <std::size_t MaxN>
class String {
 public:
  template <std::size_t MaxR>
  String(const String<MaxR>& rhs) noexcept
  {
    assign(rhs.data(), rhs.size());
  }
  String(std::string_view rhs) noexcept { assign(rhs.data(), rhs.size()); }
  constexpr String() noexcept = default;

  ~String() noexcept = default;

  // Copy.
  String(const String& rhs) noexcept { assign(rhs.data(), rhs.size()); }
  String& operator=(const String& rhs) noexcept
  {
    assign(rhs.data(), rhs.size());
    return *this;
  }

  // Move.
  constexpr String(String&&) noexcept = default;
  constexpr String& operator=(String&&) noexcept = default;

  template <std::size_t MaxR>
  String& operator=(const String<MaxR>& rhs) noexcept
  {
    assign(rhs.data(), rhs.size());
    return *this;
  }
  String& operator=(std::string_view rhs) noexcept
  {
    assign(rhs.data(), rhs.size());
    return *this;
  }
  template <std::size_t MaxR>
  int compare(const String<MaxR>& rhs) const noexcept
  {
    return compare(rhs.data(), rhs.size());
  }
  int compare(std::string_view rhs) const noexcept { return compare(rhs.data(), rhs.size()); }
  constexpr const char* data() const noexcept { return buf_; }
  constexpr bool empty() const noexcept { return len_ == 0; }
  constexpr std::size_t size() const noexcept { return len_; }
  constexpr void clear() noexcept { len_ = 0; }

 private:
  int compare(const char* rdata, std::size_t rlen) const noexcept
  {
    int result{std::memcmp(buf_, rdata, std::min(size(), rlen))};
    if (result == 0) {
      result = swirly::compare(size(), rlen);
    }
    return result;
  }
  void assign(const char* rdata, std::size_t rlen) noexcept
  {
    len_ = std::min(rlen, MaxN);
    if (len_ > 0) {
      std::memcpy(buf_, rdata, len_);
    }
  }
  // Length in the first cache-line.
  // Use int to save space.
  int len_{0};
  char buf_[MaxN];
};

template <std::size_t MaxN>
constexpr std::string_view operator+(const String<MaxN>& s) noexcept
{
  return {s.data(), s.size()};
}

template <std::size_t MaxL, std::size_t MaxR>
bool operator==(const String<MaxL>& lhs, const String<MaxR>& rhs) noexcept
{
  return lhs.compare(rhs) == 0;
}

template <std::size_t MaxN>
bool operator==(const String<MaxN>& lhs, std::string_view rhs) noexcept
{
  return lhs.compare(rhs) == 0;
}

template <std::size_t MaxN>
bool operator==(std::string_view lhs, const String<MaxN>& rhs) noexcept
{
  return 0 == rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
bool operator!=(const String<MaxL>& lhs, const String<MaxR>& rhs) noexcept
{
  return lhs.compare(rhs) != 0;
}

template <std::size_t MaxN>
bool operator!=(const String<MaxN>& lhs, std::string_view rhs) noexcept
{
  return lhs.compare(rhs) != 0;
}

template <std::size_t MaxN>
bool operator!=(std::string_view lhs, const String<MaxN>& rhs) noexcept
{
  return 0 != rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
bool operator<(const String<MaxL>& lhs, const String<MaxR>& rhs) noexcept
{
  return lhs.compare(rhs) < 0;
}

template <std::size_t MaxN>
bool operator<(const String<MaxN>& lhs, std::string_view rhs) noexcept
{
  return lhs.compare(rhs) < 0;
}

template <std::size_t MaxN>
bool operator<(std::string_view lhs, const String<MaxN>& rhs) noexcept
{
  return 0 < rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
bool operator<=(const String<MaxL>& lhs, const String<MaxR>& rhs) noexcept
{
  return lhs.compare(rhs) <= 0;
}

template <std::size_t MaxN>
bool operator<=(const String<MaxN>& lhs, std::string_view rhs) noexcept
{
  return lhs.compare(rhs) <= 0;
}

template <std::size_t MaxN>
bool operator<=(std::string_view lhs, const String<MaxN>& rhs) noexcept
{
  return 0 <= rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
bool operator>(const String<MaxL>& lhs, const String<MaxR>& rhs) noexcept
{
  return lhs.compare(rhs) > 0;
}

template <std::size_t MaxN>
bool operator>(const String<MaxN>& lhs, std::string_view rhs) noexcept
{
  return lhs.compare(rhs) > 0;
}

template <std::size_t MaxN>
bool operator>(std::string_view lhs, const String<MaxN>& rhs) noexcept
{
  return 0 > rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
bool operator>=(const String<MaxL>& lhs, const String<MaxR>& rhs) noexcept
{
  return lhs.compare(rhs) >= 0;
}

template <std::size_t MaxN>
bool operator>=(const String<MaxN>& lhs, std::string_view rhs) noexcept
{
  return lhs.compare(rhs) >= 0;
}

template <std::size_t MaxN>
bool operator>=(std::string_view lhs, const String<MaxN>& rhs) noexcept
{
  return 0 >= rhs.compare(lhs);
}

template <std::size_t MaxN>
std::ostream& operator<<(std::ostream& os, const String<MaxN>& rhs)
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

template <std::size_t SizeN>
std::string_view toStringView(const char (&val)[SizeN]) noexcept
{
  return {val, strnlen(val, SizeN)};
}

template <std::size_t SizeN>
std::size_t setCString(char (&lhs)[SizeN], const char* rhs) noexcept
{
  return stpncpy(lhs, rhs, SizeN) - &lhs[0];
}

template <std::size_t SizeN>
std::size_t setCString(char (&lhs)[SizeN], const char* rdata, std::size_t rlen) noexcept
{
  const size_t len{std::min(SizeN, rlen)};
  if (len > 0) {
    std::memcpy(lhs, rdata, len);
  }
  if (len < SizeN) {
    std::memset(lhs + len, 0, SizeN - len);
  }
  return len;
}

template <std::size_t SizeN>
std::size_t setCString(char (&lhs)[SizeN], std::string_view rhs) noexcept
{
  return setCString(lhs, rhs.data(), rhs.size());
}

SWIRLY_API uint64_t stou64(std::string_view sv) noexcept;

SWIRLY_API bool stob(std::string_view sv, bool dfl = false) noexcept;

SWIRLY_API void ltrim(std::string_view& s) noexcept;

SWIRLY_API void ltrim(std::string& s) noexcept;

SWIRLY_API void rtrim(std::string_view& s) noexcept;

SWIRLY_API void rtrim(std::string& s) noexcept;

inline void trim(std::string_view& s) noexcept
{
  ltrim(s);
  rtrim(s);
}

inline void trim(std::string& s) noexcept
{
  ltrim(s);
  rtrim(s);
}

inline std::string_view ltrimCopy(std::string_view s) noexcept
{
  ltrim(s);
  return s;
}

inline std::string ltrimCopy(std::string s) noexcept
{
  ltrim(s);
  return s;
}

inline std::string_view rtrimCopy(std::string_view s) noexcept
{
  rtrim(s);
  return s;
}

inline std::string rtrimCopy(std::string s) noexcept
{
  rtrim(s);
  return s;
}

inline std::string_view trimCopy(std::string_view s) noexcept
{
  trim(s);
  return s;
}

inline std::string trimCopy(std::string s) noexcept
{
  trim(s);
  return s;
}

SWIRLY_API std::pair<std::string_view, std::string_view> splitPair(std::string_view s,
                                                                   char delim) noexcept;

SWIRLY_API std::pair<std::string, std::string> splitPair(const std::string& s, char delim);

/** @} */

} // swirly

#endif // SWIRLY_ASH_STRING_HPP
