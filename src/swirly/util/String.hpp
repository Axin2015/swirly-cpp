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

#include <swirly/util/TypeTraits.hpp>

#include <cstring>
#include <sstream>
#include <string_view>

namespace swirly {
using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;
inline namespace util {

template <typename ValueT>
inline ValueT from_string(std::string_view sv) noexcept
{
    using Traits = TypeTraits<ValueT>;
    return Traits::from_string(sv);
}

template <typename ValueT>
inline ValueT from_string(const std::string& s) noexcept
{
    using Traits = TypeTraits<ValueT>;
    return Traits::from_string(s);
}

template <typename ValueT, typename std::enable_if_t<std::is_arithmetic_v<ValueT>>* = nullptr>
std::string to_string(ValueT val)
{
    return std::to_string(val);
}

template <typename ValueT, typename std::enable_if_t<!std::is_arithmetic_v<ValueT>>* = nullptr>
std::string to_string(const ValueT& val)
{
    std::stringstream ss;
    ss << val;
    return ss.str();
}

template <std::size_t SizeN>
std::string_view to_string_view(const char (&val)[SizeN]) noexcept
{
    return {val, strnlen(val, SizeN)};
}

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

inline std::string_view ltrim_copy(std::string_view s) noexcept
{
    ltrim(s);
    return s;
}

inline std::string ltrim_copy(std::string s) noexcept
{
    ltrim(s);
    return s;
}

inline std::string_view rtrim_copy(std::string_view s) noexcept
{
    rtrim(s);
    return s;
}

inline std::string rtrim_copy(std::string s) noexcept
{
    rtrim(s);
    return s;
}

inline std::string_view trim_copy(std::string_view s) noexcept
{
    trim(s);
    return s;
}

inline std::string trim_copy(std::string s) noexcept
{
    trim(s);
    return s;
}

SWIRLY_API std::pair<std::string_view, std::string_view> split_pair(std::string_view s,
                                                                    char delim) noexcept;

SWIRLY_API std::pair<std::string, std::string> split_pair(const std::string& s, char delim);

template <char PadC>
inline std::size_t pstrlen(const char* src, std::size_t n) noexcept
{
    if constexpr (PadC == '\0') {
        // Optimised case.
        return strnlen(src, n);
    } else {
        std::size_t i{0};
        while (i < n && src[i] != PadC) {
            ++i;
        }
        return i;
    }
}

template <char PadC, std::size_t SizeN>
inline std::size_t pstrlen(const char (&src)[SizeN]) noexcept
{
    return pstrlen<PadC>(src, SizeN);
}

template <char PadC>
inline std::size_t pstrcpy(char* dst, const char* src, std::size_t n) noexcept
{
    if constexpr (PadC == '\0') {
        // Optimised case.
        return stpncpy(dst, src, n) - dst;
    } else {
        std::size_t i{0};
        for (; i < n && src[i] != '\0'; ++i) {
            dst[i] = src[i];
        }
        for (std::size_t j{i}; j < n; ++j) {
            dst[j] = PadC;
        }
        return i;
    }
}

template <char PadC, std::size_t SizeN>
inline std::size_t pstrcpy(char (&dst)[SizeN], const char* src) noexcept
{
    return pstrcpy<PadC>(dst, src, SizeN);
}

template <char PadC>
inline std::size_t pstrcpy(char* dst, std::string_view src, std::size_t n) noexcept
{
    const std::size_t len{std::min(n, src.size())};
    if (len > 0) {
        std::memcpy(dst, src.data(), len);
    }
    if (len < n) {
        std::memset(dst + len, PadC, n - len);
    }
    return len;
}

template <char PadC, std::size_t SizeN>
inline std::size_t pstrcpy(char (&dst)[SizeN], std::string_view src) noexcept
{
    return pstrcpy<PadC>(dst, src, SizeN);
}

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_STRING_HPP
