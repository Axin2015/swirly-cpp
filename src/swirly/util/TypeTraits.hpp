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
#ifndef SWIRLY_UTIL_TYPETRAITS_HPP
#define SWIRLY_UTIL_TYPETRAITS_HPP

#include <swirly/util/Utility.hpp>

#include <string>

namespace swirly {
inline namespace util {

template <typename ValueT, typename EnableT = void>
struct TypeTraits;

template <typename ValueT>
struct TypeTraits<ValueT, std::enable_if_t<std::is_integral_v<ValueT>>> {
    static constexpr auto from_string(std::string_view sv) noexcept { return ston<ValueT>(sv); }
    static constexpr auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s});
    }
};

template <>
struct TypeTraits<bool> {
    static auto from_string(std::string_view sv) noexcept { return stob(sv); }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s});
    }
};

template <typename ValueT>
struct TypeTraits<ValueT, std::enable_if_t<std::is_enum_v<ValueT>>> {
    static constexpr auto from_string(std::string_view sv) noexcept
    {
        using UnderlyingTraits = TypeTraits<std::underlying_type_t<ValueT>>;
        return static_cast<ValueT>(UnderlyingTraits::from_string(sv));
    }
    static constexpr auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s});
    }
};

template <>
struct TypeTraits<std::string_view> {
    static constexpr auto from_string(std::string_view sv) noexcept { return sv; }
    /**
     * Disable conversion from std::string to std::string_view due to possible danger of dangling
     * reference to temporary.
     */
    static std::string_view from_string(const std::string& s) noexcept = delete;
};

template <>
struct TypeTraits<std::string> {
    static std::string from_string(std::string_view sv) noexcept { return {sv.data(), sv.size()}; }
    static std::string from_string(const std::string& s) noexcept { return s; }
};

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_TYPETRAITS_HPP
