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
#ifndef SWIRLY_UTIL_STRINGBUF_HPP
#define SWIRLY_UTIL_STRINGBUF_HPP

#include <swirly/util/Compare.hpp>

#include <cstring>
#include <string_view>

namespace swirly {
inline namespace util {

/**
 * String buffer with fixed upper-bound.
 */
template <std::size_t MaxN>
class StringBuf {
  public:
    template <std::size_t MaxR>
    StringBuf(const StringBuf<MaxR>& rhs) noexcept
    {
        assign(rhs.data(), rhs.size());
    }
    StringBuf(std::string_view rhs) noexcept { assign(rhs.data(), rhs.size()); }
    constexpr StringBuf() noexcept = default;

    ~StringBuf() noexcept = default;

    // Copy.
    StringBuf(const StringBuf& rhs) noexcept { assign(rhs.data(), rhs.size()); }
    StringBuf& operator=(const StringBuf& rhs) noexcept
    {
        assign(rhs.data(), rhs.size());
        return *this;
    }

    // Move.
    constexpr StringBuf(StringBuf&&) noexcept = default;
    constexpr StringBuf& operator=(StringBuf&&) noexcept = default;

    template <std::size_t MaxR>
    StringBuf& operator=(const StringBuf<MaxR>& rhs) noexcept
    {
        assign(rhs.data(), rhs.size());
        return *this;
    }
    StringBuf& operator=(std::string_view rhs) noexcept
    {
        assign(rhs.data(), rhs.size());
        return *this;
    }
    template <std::size_t MaxR>
    int compare(const StringBuf<MaxR>& rhs) const noexcept
    {
        return compare(rhs.data(), rhs.size());
    }
    int compare(std::string_view rhs) const noexcept { return compare(rhs.data(), rhs.size()); }
    constexpr const char* data() const noexcept { return buf_; }
    constexpr bool empty() const noexcept { return len_ == 0; }
    constexpr std::size_t size() const noexcept { return len_; }
    constexpr void clear() noexcept { len_ = 0; }

    void assign(std::string_view rhs) noexcept { assign(rhs.data(), rhs.size()); }
    void append(std::string_view rhs) noexcept { append(rhs.data(), rhs.size()); }
    template <std::size_t MaxR>
    StringBuf& operator+=(const StringBuf<MaxR>& rhs) noexcept
    {
        append(rhs.data(), rhs.size());
        return *this;
    }
    StringBuf& operator+=(std::string_view rhs) noexcept
    {
        append(rhs.data(), rhs.size());
        return *this;
    }

  private:
    void assign(const char* rdata, std::size_t rlen) noexcept
    {
        len_ = std::min(rlen, MaxN);
        if (len_ > 0) {
            std::memcpy(buf_, rdata, len_);
        }
    }
    void append(const char* rdata, std::size_t rlen) noexcept
    {
        rlen = std::min(rlen, MaxN - len_);
        if (rlen > 0) {
            std::memcpy(buf_ + len_, rdata, rlen);
            len_ += rlen;
        }
    }
    int compare(const char* rdata, std::size_t rlen) const noexcept
    {
        int result{std::memcmp(buf_, rdata, std::min(size(), rlen))};
        if (result == 0) {
            result = swirly::compare(size(), rlen);
        }
        return result;
    }
    // Length in the first cache-line.
    // Use int to save space.
    int len_{0};
    char buf_[MaxN];
};

template <std::size_t MaxN>
constexpr std::string_view operator+(const StringBuf<MaxN>& s) noexcept
{
    return {s.data(), s.size()};
}

template <std::size_t MaxL, std::size_t MaxR>
bool operator==(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

template <std::size_t MaxN>
bool operator==(const StringBuf<MaxN>& lhs, std::string_view rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

template <std::size_t MaxN>
bool operator==(std::string_view lhs, const StringBuf<MaxN>& rhs) noexcept
{
    return 0 == rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
bool operator!=(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}

template <std::size_t MaxN>
bool operator!=(const StringBuf<MaxN>& lhs, std::string_view rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}

template <std::size_t MaxN>
bool operator!=(std::string_view lhs, const StringBuf<MaxN>& rhs) noexcept
{
    return 0 != rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
bool operator<(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template <std::size_t MaxN>
bool operator<(const StringBuf<MaxN>& lhs, std::string_view rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template <std::size_t MaxN>
bool operator<(std::string_view lhs, const StringBuf<MaxN>& rhs) noexcept
{
    return 0 < rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
bool operator<=(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template <std::size_t MaxN>
bool operator<=(const StringBuf<MaxN>& lhs, std::string_view rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template <std::size_t MaxN>
bool operator<=(std::string_view lhs, const StringBuf<MaxN>& rhs) noexcept
{
    return 0 <= rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
bool operator>(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template <std::size_t MaxN>
bool operator>(const StringBuf<MaxN>& lhs, std::string_view rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template <std::size_t MaxN>
bool operator>(std::string_view lhs, const StringBuf<MaxN>& rhs) noexcept
{
    return 0 > rhs.compare(lhs);
}

template <std::size_t MaxL, std::size_t MaxR>
bool operator>=(const StringBuf<MaxL>& lhs, const StringBuf<MaxR>& rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

template <std::size_t MaxN>
bool operator>=(const StringBuf<MaxN>& lhs, std::string_view rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

template <std::size_t MaxN>
bool operator>=(std::string_view lhs, const StringBuf<MaxN>& rhs) noexcept
{
    return 0 >= rhs.compare(lhs);
}

template <std::size_t MaxN>
std::ostream& operator<<(std::ostream& os, const StringBuf<MaxN>& rhs)
{
    return std::operator<<(os, std::string_view{rhs.data(), rhs.size()});
}

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_STRINGBUF_HPP
