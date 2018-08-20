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
#ifndef SWIRLY_UTIL_VERSION_HPP
#define SWIRLY_UTIL_VERSION_HPP

#include <swirly/util/Compare.hpp>
#include <swirly/util/String.hpp>

#include <boost/functional/hash.hpp>

namespace swirly {
inline namespace util {

struct Version : protected Comparable<Version> {
    constexpr Version(int major = 0, int minor = 0) noexcept
    : major{major}
    , minor{minor}
    {
    }
    ~Version() = default;

    // Copy.
    constexpr Version(const Version& rhs) noexcept = default;
    constexpr Version& operator=(const Version& rhs) noexcept = default;

    // Move.
    constexpr Version(Version&&) noexcept = default;
    constexpr Version& operator=(Version&&) noexcept = default;

    constexpr int compare(const Version& rhs) const noexcept
    {
        auto i = swirly::compare(major, rhs.major);
        if (i == 0) {
            i = swirly::compare(minor, rhs.minor);
        }
        return i;
    }
    int major{}, minor{};
};
static_assert(Version{1, 2} == Version{1, 2});
static_assert(Version{1, 2} < Version{1, 3});
static_assert(Version{1, 2} < Version{2, 2});

SWIRLY_API std::ostream& operator<<(std::ostream& os, Version ver);

template <>
struct TypeTraits<Version> {
    static auto from_string(std::string_view sv) noexcept
    {
        const auto [major, minor] = split_pair(sv, '.');
        return Version{stoi32(major), stoi32(minor)};
    }
    static auto from_string(const std::string& s) noexcept
    {
        return from_string(std::string_view{s});
    }
};

inline std::size_t hash_value(swirly::Version ver)
{
    std::size_t h{0};
    boost::hash_combine(h, ver.major);
    boost::hash_combine(h, ver.minor);
    return h;
}

} // namespace util
} // namespace swirly

namespace std {

template <>
struct hash<swirly::Version> {
    inline std::size_t operator()(swirly::Version ver) const { return hash_value(ver); }
};

} // namespace std

#endif // SWIRLY_UTIL_VERSION_HPP
