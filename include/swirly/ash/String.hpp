/*
 * Swirly Order-Book and Matching-Engine.
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

namespace swirly {

/**
 * @addtogroup Util
 * @{
 */

using StringView = std::experimental::string_view;

constexpr StringView operator""_sv(const char* str, std::size_t len) noexcept
{
    return {str, len};
}

template <std::size_t MaxN>
struct StringData {

    // Length in the first cache-line.
    std::size_t len;
    char buf[MaxN];
};

/**
 * String buffer with fixed upper-bound.
 */
template <std::size_t MaxN>
class StringBuf : protected StringData<MaxN> {
    using StringData<MaxN>::len;
    using StringData<MaxN>::buf;
 public:
    using Data = StringData<MaxN>;

    template <std::size_t MaxR>
    constexpr StringBuf(const StringBuf<MaxR>& rhs) noexcept
    {
        *this = rhs;
    }
    constexpr StringBuf(const StringView& rhs) noexcept
    {
        *this = rhs;
    }
    constexpr StringBuf() noexcept
    {
        len = 0;
    }

    ~StringBuf() noexcept = default;

    // Copy.
    constexpr StringBuf(const StringBuf& rhs) noexcept
    {
        *this = rhs;
    }
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
    constexpr StringBuf& operator=(const StringView& rhs) noexcept
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
    constexpr int compare(const StringView& rhs) const noexcept
    {
        int result{std::memcmp(buf, rhs.data(), std::min(len, rhs.size()))};
        if (result == 0)
            result = swirly::compare(len, rhs.size());
        return result;
    }
    constexpr const char* data() const noexcept
    {
        return buf;
    }
    constexpr bool empty() const noexcept
    {
        return len == 0;
    }
    constexpr size_t size() const noexcept
    {
        return len;
    }
    constexpr StringView view() const noexcept
    {
        return {buf, len};
    }
};

template <std::size_t MaxL, std::size_t MaxR>
constexpr bool operator==(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

template <std::size_t MaxN>
constexpr bool operator==(const StringBuf<MaxN>& lhs, const StringView& rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

template <std::size_t MaxN>
constexpr bool operator==(const StringView& lhs, const StringBuf<MaxN>& rhs) noexcept
{
    return 0 == rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
constexpr bool operator!=(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}

template <std::size_t MaxN>
constexpr bool operator!=(const StringBuf<MaxN>& lhs, const StringView& rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}

template <std::size_t MaxN>
constexpr bool operator!=(const StringView& lhs, const StringBuf<MaxN>& rhs) noexcept
{
    return 0 != rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
constexpr bool operator<(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template <std::size_t MaxN>
constexpr bool operator<(const StringBuf<MaxN>& lhs, const StringView& rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template <std::size_t MaxN>
constexpr bool operator<(const StringView& lhs, const StringBuf<MaxN>& rhs) noexcept
{
    return 0 < rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
constexpr bool operator<=(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template <std::size_t MaxN>
constexpr bool operator<=(const StringBuf<MaxN>& lhs, const StringView& rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template <std::size_t MaxN>
constexpr bool operator<=(const StringView& lhs, const StringBuf<MaxN>& rhs) noexcept
{
    return 0 <= rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
constexpr bool operator>(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template <std::size_t MaxN>
constexpr bool operator>(const StringBuf<MaxN>& lhs, const StringView& rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template <std::size_t MaxN>
constexpr bool operator>(const StringView& lhs, const StringBuf<MaxN>& rhs) noexcept
{
    return 0 > rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
constexpr bool operator>=(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

template <std::size_t MaxN>
constexpr bool operator>=(const StringBuf<MaxN>& lhs, const StringView& rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

template <std::size_t MaxN>
constexpr bool operator>=(const StringView& lhs, const StringBuf<MaxN>& rhs) noexcept
{
    return 0 >= rhs.compare(lhs);
}

template <std::size_t MaxN>
constexpr std::ostream& operator<<(std::ostream& os, const StringBuf<MaxN>& rhs) noexcept
{
    return os << rhs.view();
}

/** @} */

} // swirly

#endif // SWIRLY_ASH_STRING_HPP
