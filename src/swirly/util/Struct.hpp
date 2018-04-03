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
#ifndef SWIRLY_UTIL_STRUCT_HPP
#define SWIRLY_UTIL_STRUCT_HPP

#include <cstdint>

// The Struct concept is essentially a tag-based tuple that was inspired by:
// 1. Bronek Kozicki and his open souce projects (https://github.com/Bronek)
// 2. std::tuple<>
// 3. http://blogs.microsoft.co.il/sasha/2015/01/12/implementing-tuple-part-1/

namespace swirly {
inline namespace util {
namespace detail {

template <typename TagT, typename ValueT>
struct Member {
    constexpr explicit Member(const ValueT& value)
    : value{value}
    {
    }
    ValueT value;
};

template <typename...>
struct Struct;

template <>
struct Struct<> {

    enum : std::size_t { Size = 0 };

    template <typename TagT, typename ValueT>
    constexpr auto extend(const ValueT& head) const
    {
        return Struct<Member<TagT, ValueT>>{head};
    }

    constexpr const auto& slice(...) const noexcept { return *this; }
    constexpr auto& slice(...) noexcept { return *this; }
};

template <typename TagT, typename ValueT, typename... TagsT, typename... ValuesT>
struct Struct<Member<TagT, ValueT>, Member<TagsT, ValuesT>...> : Struct<Member<TagsT, ValuesT>...> {

    using Tail = Struct<Member<TagsT, ValuesT>...>;
    enum : std::size_t { Size = 1 + Tail::Size };

    constexpr explicit Struct(const ValueT& head)
    : head{head}
    {
    }
    constexpr Struct(const ValueT& head, const Tail& tail)
    : Tail{tail}
    , head{head}
    {
    }
    template <typename TagU, typename ValueU>
    constexpr auto extend(const ValueU& head) const
    {
        return Struct<Member<TagU, ValueU>, Member<TagT, ValueT>, //
                      Member<TagsT, ValuesT>...>{head, *this};
    }

    using Tail::slice;
    constexpr const auto& slice(TagT) const noexcept { return *this; };
    constexpr auto& slice(TagT) noexcept { return *this; };

    ValueT head;
};

} // namespace detail

constexpr auto Struct = detail::Struct<>{};

template <typename... TagsT, typename... ValuesT>
constexpr bool empty(const detail::Struct<detail::Member<TagsT, ValuesT>...>& s)
{
    return s.Size == 0;
}

template <typename... TagsT, typename... ValuesT>
constexpr std::size_t size(const detail::Struct<detail::Member<TagsT, ValuesT>...>& s)
{
    return s.Size;
}

template <typename TagT, typename... TagsT, typename... ValuesT>
constexpr bool has(const detail::Struct<detail::Member<TagsT, ValuesT>...>& s, TagT tag = {})
{
    return s.slice(tag).Size > 0;
}

template <typename TagT, typename... TagsT, typename... ValuesT>
constexpr const auto& get(const detail::Struct<detail::Member<TagsT, ValuesT>...>& s, TagT tag = {})
{
    return s.slice(tag).head;
}

template <typename TagT, typename... TagsT, typename... ValuesT>
constexpr auto& get(detail::Struct<detail::Member<TagsT, ValuesT>...>& s, TagT tag = {})
{
    return s.slice(tag).head;
}

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_STRUCT_HPP
