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
#ifndef SWIRLY_FIX_SESSID_HPP
#define SWIRLY_FIX_SESSID_HPP

#include <swirly/fix/Field.hpp>

#include <swirly/util/Version.hpp>

namespace swirly {
inline namespace fix {

template <typename StringT>
struct BasicFixSessId {
    constexpr BasicFixSessId(Version version, StringT sender_comp_id, StringT target_comp_id)
    : version{version}
    , sender_comp_id{std::move(sender_comp_id)}
    , target_comp_id{std::move(target_comp_id)}
    {
    }
    BasicFixSessId() = default;
    ~BasicFixSessId() = default;

    // Copy.
    BasicFixSessId(const BasicFixSessId& rhs) = default;
    BasicFixSessId& operator=(const BasicFixSessId& rhs) = default;

    // Move.
    BasicFixSessId(BasicFixSessId&&) = default;
    BasicFixSessId& operator=(BasicFixSessId&&) = default;

    BasicFixSessId operator~() const { return {version, target_comp_id, sender_comp_id}; }

    template <typename StringU>
    constexpr int compare(const BasicFixSessId<StringU>& rhs) const noexcept
    {
        auto i = version.compare(rhs.version);
        if (i == 0) {
            i = sender_comp_id.compare(rhs.sender_comp_id);
            if (i == 0) {
                i = target_comp_id.compare(rhs.target_comp_id);
            }
        }
        return i;
    }

    Version version;
    StringT sender_comp_id;
    StringT target_comp_id;
};

template <typename StringT, typename StringU>
constexpr bool operator==(const BasicFixSessId<StringT>& lhs,
                          const BasicFixSessId<StringU>& rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

template <typename StringT, typename StringU>
constexpr bool operator!=(const BasicFixSessId<StringT>& lhs,
                          const BasicFixSessId<StringU>& rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}

template <typename StringT, typename StringU>
constexpr bool operator<(const BasicFixSessId<StringT>& lhs,
                         const BasicFixSessId<StringU>& rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template <typename StringT, typename StringU>
constexpr bool operator<=(const BasicFixSessId<StringT>& lhs,
                          const BasicFixSessId<StringU>& rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template <typename StringT, typename StringU>
constexpr bool operator>(const BasicFixSessId<StringT>& lhs,
                         const BasicFixSessId<StringU>& rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template <typename StringT, typename StringU>
constexpr bool operator>=(const BasicFixSessId<StringT>& lhs,
                          const BasicFixSessId<StringU>& rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

/**
 * Comparator for transparent key search.
 */
struct FixSessIdCompare {
    using is_transparent = void;
    template <typename StringT, typename StringU>
    constexpr bool operator()(const BasicFixSessId<StringT>& lhs,
                              const BasicFixSessId<StringU>& rhs) const noexcept
    {
        return lhs < rhs;
    }
};

template <typename StringT>
std::ostream& operator<<(std::ostream& os, const BasicFixSessId<StringT>& sess_id)
{
    return os << sess_id.version << ':' << sess_id.sender_comp_id << "->" << sess_id.target_comp_id;
}

template <typename StringT>
std::size_t hash_value(const BasicFixSessId<StringT>& sess_id)
{
    std::size_t h{0};
    boost::hash_combine(h, sess_id.version);
    boost::hash_combine(
        h, boost::hash_range(sess_id.sender_comp_id.begin(), sess_id.sender_comp_id.end()));
    boost::hash_combine(
        h, boost::hash_range(sess_id.target_comp_id.begin(), sess_id.target_comp_id.end()));
    return h;
}

using FixSessId = BasicFixSessId<std::string>;
using FixSessIdView = BasicFixSessId<std::string_view>;
static_assert(FixSessIdView{Version{1, 2}, "foo"sv, "bar"sv}
              == FixSessIdView{Version{1, 2}, "foo"sv, "bar"sv});

} // namespace fix
} // namespace swirly

namespace std {

template <typename StringT>
struct hash<swirly::BasicFixSessId<StringT>> {
    inline std::size_t operator()(const swirly::BasicFixSessId<StringT>& sess_id) const
    {
        return hash_value(sess_id);
    }
};

} // namespace std

#endif // SWIRLY_FIX_SESSID_HPP
