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
#ifndef SWIRLY_FIX_HDR_HPP
#define SWIRLY_FIX_HDR_HPP

#include <swirly/fix/Field.hpp>
#include <swirly/fix/SessId.hpp>

namespace swirly {
inline namespace fix {

struct FixHdr {
    MsgType::View msg_type;
    SenderCompId::View sender_comp_id;
    TargetCompId::View target_comp_id;
    MsgSeqNum msg_seq_num;
    SendingTime sending_time;
    std::optional<PossDupFlag> poss_dup;
    std::optional<PossResend> poss_resend;
};

SWIRLY_API std::ostream& operator<<(std::ostream& os, const FixHdr& hdr);

SWIRLY_API std::size_t parse_hdr(std::string_view msg, std::size_t msg_type_off, FixHdr& hdr);

template <typename StringT>
BasicFixSessId<StringT> get_sess_id(Version ver, const FixHdr& hdr) noexcept
{
    return {ver, StringT{hdr.sender_comp_id.value}, StringT{hdr.target_comp_id.value}};
}

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_HDR_HPP
