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
#ifndef SWIRLY_FIX_HEADER_HPP
#define SWIRLY_FIX_HEADER_HPP

#include <swirly/fix/List.hpp>
#include <swirly/fix/SessId.hpp>

namespace swirly {
inline namespace fix {
namespace detail {
struct FixHeaderOpts {
    template <typename TagListT, typename TagT>
    static constexpr bool empty(const TagListT& tl, TagT)
    {
        bool ret{false};
        if constexpr (std::is_same_v<TagT, TagType<Tag::PossDupFlag>>) {
            ret = !get<Tag::PossDupFlag>(tl);
        } else if constexpr (std::is_same_v<TagT, TagType<Tag::PossResend>>) {
            ret = !get<Tag::PossResend>(tl);
        }
        return ret;
    }
};
} // namespace detail

// clang-format off
using FixHeader = TagList<detail::FixHeaderOpts,
                          Tag::MsgType,
                          Tag::SenderCompId,
                          Tag::TargetCompId,
                          Tag::MsgSeqNum,
                          Tag::SendingTime,
                          Tag::PossDupFlag,
                          Tag::PossResend
                          >;
// clang-format on

using FixHeaderView = FixHeader::View;

SWIRLY_API std::size_t parse_header(std::string_view msg, std::size_t msg_type_off, FixHeader& hdr);

SWIRLY_API std::size_t parse_header(std::string_view msg, std::size_t msg_type_off,
                                    FixHeaderView& hdr);

// TODO: consider adding support for FixHeader type.
template <typename StringT>
BasicFixSessId<StringT> get_sess_id(Version ver, const FixHeaderView& hdr) noexcept
{
    return {ver, StringT{get<Tag::SenderCompId>(hdr)}, StringT{get<Tag::TargetCompId>(hdr)}};
}

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_HEADER_HPP
