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
#ifndef SWIRLY_UTIL_STRING_HPP
#define SWIRLY_UTIL_STRING_HPP

#include <swirly/Config.h>

#include <cstring>
#include <sstream>
#include <string_view>

namespace swirly {
inline namespace util {

using namespace std::literals::string_view_literals;

template <typename ValueT, typename std::enable_if_t<std::is_arithmetic_v<ValueT>>* = nullptr>
std::string toString(ValueT val)
{
    return std::to_string(val);
}

template <typename ValueT, typename std::enable_if_t<!std::is_arithmetic_v<ValueT>>* = nullptr>
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
void setCString(char (&lhs)[SizeN], const char* rhs) noexcept
{
    strncpy(lhs, rhs, SizeN);
}

template <std::size_t SizeN>
void setCString(char (&lhs)[SizeN], const char* rdata, std::size_t rlen) noexcept
{
    const std::size_t len{std::min(SizeN, rlen)};
    if (len > 0) {
        std::memcpy(lhs, rdata, len);
    }
    if (len < SizeN) {
        std::memset(lhs + len, 0, SizeN - len);
    }
}

template <std::size_t SizeN>
void setCString(char (&lhs)[SizeN], std::string_view rhs) noexcept
{
    setCString(lhs, rhs.data(), rhs.size());
}

SWIRLY_API int16_t stoi16(std::string_view sv) noexcept;

SWIRLY_API int32_t stoi32(std::string_view sv) noexcept;

SWIRLY_API int64_t stoi64(std::string_view sv) noexcept;

SWIRLY_API uint16_t stou16(std::string_view sv) noexcept;

SWIRLY_API uint32_t stou32(std::string_view sv) noexcept;

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

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_STRING_HPP
