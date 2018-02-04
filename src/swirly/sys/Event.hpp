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
#ifndef SWIRLY_SYS_EVENT_HPP
#define SWIRLY_SYS_EVENT_HPP

#include <cstdint>
#include <type_traits>
#include <utility>

namespace swirly {

using FileEvents = std::uint32_t;

struct MsgEvent {
    int type;
    char data[1524];
};
static_assert(std::is_pod_v<MsgEvent>);
static_assert(sizeof(MsgEvent) + sizeof(std::int64_t) == 1536);

template <typename DataT>
void emplaceEvent(MsgEvent& ev, int type) noexcept
{
    static_assert(alignof(DataT) <= 8);
    static_assert(std::is_nothrow_default_constructible_v<DataT>);
    static_assert(std::is_trivially_copyable_v<DataT>);
    ev.type = type;
    ::new (ev.data) DataT{};
}

template <typename DataT, typename... ArgsT>
void emplaceEvent(MsgEvent& ev, int type, ArgsT&&... args) noexcept
{
    static_assert(alignof(DataT) <= 8);
    static_assert(std::is_nothrow_constructible_v<DataT, ArgsT...>);
    static_assert(std::is_trivially_copyable_v<DataT>);
    ev.type = type;
    ::new (ev.data) DataT{std::forward<ArgsT>(args)...};
}

template <typename DataT>
const DataT& data(const MsgEvent& ev) noexcept
{
    return *reinterpret_cast<const DataT*>(ev.data);
}

template <typename DataT>
DataT& data(MsgEvent& ev) noexcept
{
    return *reinterpret_cast<DataT*>(ev.data);
}

} // namespace swirly

#endif // SWIRLY_SYS_EVENT_HPP
