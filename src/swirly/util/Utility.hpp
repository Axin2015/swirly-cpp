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
#ifndef SWIRLY_UTIL_UTILITY_HPP
#define SWIRLY_UTIL_UTILITY_HPP

#include <swirly/Config.h>

#include <cstdint>
#include <string_view>
#include <type_traits>

namespace swirly {
inline namespace util {

template <typename>
struct DependentFalse : std::false_type {
};

constexpr bool isdigit(int c) noexcept
{
    return c >= '0' && c <= '9';
}
static_assert(isdigit('0') && isdigit('9') && !isdigit('A'));

template <typename ValueT>
constexpr ValueT ston(std::string_view sv) noexcept
{
    auto it = sv.begin(), end = sv.end();

    bool neg{false};
    if constexpr (std::is_signed_v<ValueT>) {
        // Handle sign.
        if (*it == '-') {
            if (++it == end) {
                return 0;
            }
            neg = true;
        }
    }

    std::uint64_t n{0};
    if (isdigit(*it)) {
        n = *it++ - '0';
        while (it != end && isdigit(*it)) {
            n *= 10;
            n += *it++ - '0';
        }
    }
    return neg ? -n : n;
}
static_assert(ston<int>(std::string_view{"-123"}) == -123);

SWIRLY_API int hex_digits(int64_t i) noexcept;

SWIRLY_API bool stob(std::string_view sv, bool dfl = false) noexcept;

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_UTILITY_HPP
