/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
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

namespace swirly {

using StringView = std::experimental::string_view;

template <std::size_t MaxN>
class SWIRLY_API StringBuf {
    std::size_t len_ = 0;
    char buf_[MaxN];
public:
    constexpr StringBuf(const char* buf, std::size_t len) noexcept
    :   len_{len}
    {
        memcpy(buf_, buf, std::min(len, MaxN));
    }
    constexpr explicit StringBuf(const char* buf) noexcept
    :   len_{strnlen(buf, MaxN)}
    {
        memcpy(buf_, buf, len_);
    }

    constexpr StringBuf() noexcept
    :   len_{0}
    {
    }

    ~StringBuf() noexcept = default;

    // Copy.
    constexpr StringBuf(const StringBuf&) noexcept = default;
    constexpr StringBuf& operator =(const StringBuf&) noexcept = default;

    // Move.
    constexpr StringBuf(StringBuf&&) noexcept = default;
    constexpr StringBuf& operator =(StringBuf&&) noexcept = default;

    template <std::size_t MaxR>
    constexpr int compare(const StringBuf<MaxR>& rhs) const noexcept
    {
        int ret {std::memcmp(buf_, rhs.data(), std::min(len_, rhs.size()))};
        if (ret == 0)
            ret = swirly::compare(len_, rhs.size());
        return ret;
    }
    constexpr const char* data() const noexcept
    {
        return buf_;
    }
    constexpr bool empty() const noexcept
    {
        return len_ == 0;
    }
    constexpr size_t size() const noexcept
    {
        return len_;
    }
    constexpr StringView view() const noexcept
    {
        return {buf_, len_};
    }
    constexpr operator StringView() const noexcept
    {
        return {buf_, len_};
    }
};

template <std::size_t MaxL, std::size_t MaxR>
inline constexpr bool operator ==(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

template <std::size_t MaxL, std::size_t MaxR>
inline constexpr bool operator !=(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}

template <std::size_t MaxL, std::size_t MaxR>
inline constexpr bool operator <(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template <std::size_t MaxL, std::size_t MaxR>
inline constexpr bool operator <=(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template <std::size_t MaxL, std::size_t MaxR>
inline constexpr bool operator >(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template <std::size_t MaxL, std::size_t MaxR>
inline constexpr bool operator >=(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

template <typename EnumT,
          typename std::enable_if_t<std::is_enum<EnumT>::value>* = nullptr>
inline std::ostream& operator <<(std::ostream& os, EnumT val)
{
    return os << enumToString(val);
}

} // swirly

#endif // SWIRLY_ASH_STRING_HPP
