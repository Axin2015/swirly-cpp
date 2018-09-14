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
#include "Hdr.hpp"

#include "Lexer.hpp"

#include <swirly/app/Exception.hpp>

#include <swirly/util/Date.hpp>

#include <algorithm>

namespace swirly {
inline namespace fix {
using namespace std;
namespace {

// Sorted array of FIX header tags.
// See https://www.onixs.biz/fix-dictionary/5.0/compBlock_StandardHeader.html
constexpr std::array<int, 32> HdrTags{8,   9,   34,  35,  43,  49,  50,  52,  56,   57,  90,
                                      91,  97,  115, 116, 122, 128, 129, 142, 143,  144, 145,
                                      212, 213, 347, 369, 627, 628, 629, 630, 1128, 1129};
static_assert(!HdrTags.empty());

inline bool is_hdr_tag(int tag) noexcept
{
    return binary_search(HdrTags.begin(), HdrTags.end(), tag);
}

} // namespace

ostream& operator<<(ostream& os, const FixHdr& hdr)
{
    os << hdr.msg_type << hdr.sender_comp_id << hdr.target_comp_id << hdr.msg_seq_num
       << hdr.sending_time;
    if (hdr.poss_dup) {
        os << *hdr.poss_dup;
    }
    if (hdr.poss_resend) {
        os << *hdr.poss_resend;
    }
    return os;
}

size_t parse_hdr(string_view msg, size_t msg_type_off, FixHdr& hdr)
{
    FixLexer lex{msg, msg_type_off};
    assert(!lex.empty());
    do {
        const auto [t, v] = lex.top();
        switch (t) {
        case MsgType::Tag:
            hdr.msg_type = v;
            break;
        case SenderCompId::Tag:
            hdr.sender_comp_id = v;
            break;
        case TargetCompId::Tag:
            hdr.target_comp_id = v;
            break;
        case MsgSeqNum::Tag:
            hdr.msg_seq_num = from_string<MsgSeqNum::Type>(v);
            break;
        case SendingTime::Tag: {
            auto [time, valid] = parse_time(v);
            if (!valid) {
                throw ProtocolException{"invalid SendingTime(52)"};
            }
            hdr.sending_time = time;
        } break;
        case PossDupFlag::Tag:
            hdr.poss_dup = from_string<PossDupFlag::Type>(v);
            break;
        case PossResend::Tag:
            hdr.poss_resend = from_string<PossResend::Type>(v);
            break;
        default:
            if (!is_hdr_tag(t)) {
                return lex.offset();
            }
        }
        lex.pop();
    } while (!lex.empty());
    return lex.offset();
}

} // namespace fix
} // namespace swirly
