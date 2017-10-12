/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#ifndef SWIRLY_UTIL_SYMBOL_HPP
#define SWIRLY_UTIL_SYMBOL_HPP

#include <swirly/util/Compare.hpp>
#include <swirly/util/Limits.hpp>

#include <swirly/Config.h>

#include <experimental/string_view>

#include <cstdint>
#include <cstring>
#include <memory>

namespace std {
using experimental::string_view;
}

namespace swirly {

static_assert(MaxSymbol == 16, "must be specific size");

/**
 * Memorable identifier.
 */
class Symbol {
  public:
    using value_type = char;

    using pointer = const char*;
    using const_pointer = const char*;

    using reference = const char&;
    using const_reference = const char&;

    using const_iterator = const char*;
    using iterator = const_iterator;

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;

    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;

    Symbol(std::string_view rhs) noexcept { assign(rhs.data(), rhs.size()); }
    constexpr Symbol() noexcept : u64_{0, 0} {}
    ~Symbol() noexcept = default;

    // Copy.
    constexpr Symbol(const Symbol& rhs) noexcept : u64_{rhs.u64_[0], rhs.u64_[1]} {}
    constexpr Symbol& operator=(const Symbol& rhs) noexcept
    {
        u64_[0] = rhs.u64_[0];
        u64_[1] = rhs.u64_[1];
        return *this;
    }

    // Move.
    constexpr Symbol(Symbol&&) noexcept = default;
    Symbol& operator=(Symbol&&) noexcept = default;

    Symbol& operator=(std::string_view rhs) noexcept
    {
        assign(rhs.data(), rhs.size());
        return *this;
    }
    constexpr const_iterator begin() const noexcept { return buf_; }
    const_iterator end() const noexcept { return buf_ + size(); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    constexpr const_iterator cbegin() const noexcept { return buf_; }
    const_iterator cend() const noexcept { return buf_ + size(); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
    constexpr const char& operator[](size_type pos) const noexcept { return buf_[pos]; }
    constexpr const char& front() const noexcept { return buf_[0]; }
    const char& back() const noexcept { return buf_[size() - 1]; }

    int compare(const Symbol& rhs) const noexcept
    {
        // This function is on the critical path. Avoid memcmp when possible.
        return equal(rhs) ? 0 : std::memcmp(buf_, rhs.buf_, sizeof(buf_));
    }
    int compare(std::string_view rhs) const noexcept { return compare(rhs.data(), rhs.size()); }
    constexpr const char* data() const noexcept { return buf_; }
    constexpr bool empty() const noexcept { return u64_[0] == 0 && u64_[1] == 0; }
    constexpr bool equal(const Symbol& rhs) const noexcept
    {
        return u64_[0] == rhs.u64_[0] && u64_[1] == rhs.u64_[1];
    }
    std::size_t size() const noexcept { return strnlen(buf_, sizeof(buf_)); }
    constexpr void clear() noexcept
    {
        u64_[0] = 0;
        u64_[1] = 0;
    }

  private:
    int compare(const char* rdata, std::size_t rlen) const noexcept
    {
        const std::size_t len{size()};
        int result{std::memcmp(buf_, rdata, std::min(len, rlen))};
        if (result == 0) {
            result = swirly::compare(len, rlen);
        }
        return result;
    }
    constexpr void assign(const char* rdata, std::size_t rlen) noexcept
    {
        const std::size_t len{std::min(sizeof(buf_), rlen)};
        std::size_t i{0};
        for (; i < len; ++i) {
            buf_[i] = rdata[i];
        }
        for (; i < sizeof(buf_); ++i) {
            buf_[i] = '\0';
        }
    }
    union {
        int64_t u64_[2];
        char buf_[MaxSymbol];
    };
};

inline std::string_view operator+(Symbol symbol) noexcept
{
    return {symbol.data(), symbol.size()};
}

constexpr bool operator==(Symbol lhs, Symbol rhs) noexcept
{
    return lhs.equal(rhs);
}

inline bool operator==(Symbol lhs, std::string_view rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

inline bool operator==(std::string_view lhs, Symbol rhs) noexcept
{
    return 0 == rhs.compare(lhs);
}

constexpr bool operator!=(Symbol lhs, Symbol rhs) noexcept
{
    return !lhs.equal(rhs);
}

inline bool operator!=(Symbol lhs, std::string_view rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}

inline bool operator!=(std::string_view lhs, Symbol rhs) noexcept
{
    return 0 != rhs.compare(lhs);
}

inline bool operator<(Symbol lhs, Symbol rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

inline bool operator<(Symbol lhs, std::string_view rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

inline bool operator<(std::string_view lhs, Symbol rhs) noexcept
{
    return 0 < rhs.compare(lhs);
}

inline bool operator<=(Symbol lhs, Symbol rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

inline bool operator<=(Symbol lhs, std::string_view rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

inline bool operator<=(std::string_view lhs, Symbol rhs) noexcept
{
    return 0 <= rhs.compare(lhs);
}

inline bool operator>(Symbol lhs, Symbol rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

inline bool operator>(Symbol lhs, std::string_view rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

inline bool operator>(std::string_view lhs, Symbol rhs) noexcept
{
    return 0 > rhs.compare(lhs);
}

inline bool operator>=(Symbol lhs, Symbol rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

inline bool operator>=(Symbol lhs, std::string_view rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

inline bool operator>=(std::string_view lhs, Symbol rhs) noexcept
{
    return 0 >= rhs.compare(lhs);
}

inline std::ostream& operator<<(std::ostream& os, Symbol rhs)
{
    return os << +rhs;
}

template <std::size_t SizeN>
void setCString(char (&lhs)[SizeN], Symbol rhs) noexcept
{
    constexpr std::size_t len{std::min(SizeN, MaxSymbol)};
    std::size_t i{0};
    for (; i < len; ++i) {
        lhs[i] = rhs[i];
    }
    for (; i < SizeN; ++i) {
        lhs[i] = '\0';
    }
}

} // namespace swirly

#endif // SWIRLY_UTIL_SYMBOL_HPP
