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

#include <swirly/util/String.hpp>
#include <swirly/util/Time.hpp>

namespace swirly {
inline namespace util {

template <typename ValueT, typename EnableT = void>
struct TypeTraits;

template <>
struct TypeTraits<std::int16_t> {
    static auto from_string(std::string_view sv) noexcept { return stoi16(sv); }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s.data(), s.size()});
    }
};

template <>
struct TypeTraits<std::int32_t> {
    static auto from_string(std::string_view sv) noexcept { return stoi32(sv); }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s.data(), s.size()});
    }
};

template <>
struct TypeTraits<std::int64_t> {
    static auto from_string(std::string_view sv) noexcept { return stoi32(sv); }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s.data(), s.size()});
    }
};

template <>
struct TypeTraits<std::uint16_t> {
    static auto from_string(std::string_view sv) noexcept { return stou16(sv); }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s.data(), s.size()});
    }
};

template <>
struct TypeTraits<std::uint32_t> {
    static auto from_string(std::string_view sv) noexcept { return stou32(sv); }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s.data(), s.size()});
    }
};

template <>
struct TypeTraits<std::uint64_t> {
    static auto from_string(std::string_view sv) noexcept { return stou32(sv); }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s.data(), s.size()});
    }
};

template <>
struct TypeTraits<bool> {
    static auto from_string(std::string_view sv) noexcept { return stob(sv); }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s.data(), s.size()});
    }
};

template <typename ValueT>
struct TypeTraits<ValueT, std::enable_if_t<std::is_enum_v<ValueT>>> {
    static auto from_string(std::string_view sv) noexcept
    {
        using UnderlyingTraits = TypeTraits<std::underlying_type_t<ValueT>>;
        return static_cast<ValueT>(UnderlyingTraits::from_string(sv));
    }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s.data(), s.size()});
    }
};

template <>
struct TypeTraits<Time> {
    static auto from_string(std::string_view sv) noexcept { return to_time(Millis{stoi64(sv)}); }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s.data(), s.size()});
    }
};

template <>
struct TypeTraits<std::string_view> {
    static auto from_string(std::string_view sv) noexcept { return sv; }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s.data(), s.size()});
    }
};

template <>
struct TypeTraits<std::string> {
    static auto from_string(std::string_view sv) noexcept
    {
        return std::string{sv.data(), sv.size()};
    }
    static auto from_string(const std::string& s) noexcept { return s; }
};

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

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_TYPETRAITS_HPP
