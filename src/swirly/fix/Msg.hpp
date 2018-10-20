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
#ifndef SWIRLY_FIX_MSG_HPP
#define SWIRLY_FIX_MSG_HPP

#include <swirly/fix/Group.hpp>
#include <swirly/fix/Lexer.hpp>

namespace swirly {
inline namespace fix {

template <typename TagOptsT, Tag... TagN>
void parse_fix_list(FixLexer& lex, TagList<TagOptsT, TagN...>& tl)
{
    while (!lex.empty()) {
        const auto [t, v] = lex.next();
        tl.set(box<Tag>(t), v);
    }
}

namespace detail {
struct ExecutionReportOpts {
    template <typename TagListT, typename TagT>
    static constexpr bool empty(const TagListT& tl, TagT)
    {
        bool ret{false};
        if constexpr (std::is_same_v<TagT, TagType<Tag::LastQty>>) {
            ret = get<Tag::LastQty>(tl) == 0_lts;
        } else if constexpr (std::is_same_v<TagT, TagType<Tag::LastPx>>) {
            ret = get<Tag::LastQty>(tl) == 0_lts;
        }
        return ret;
    }
};
} // namespace detail

// clang-format off
using ExecutionReport = TagList<detail::ExecutionReportOpts,
                                Tag::Symbol,
                                Tag::SettlDate,
                                Tag::ExecId,
                                Tag::OrderId,
                                Tag::ExecType,
                                Tag::OrdStatus,
                                Tag::Side,
                                Tag::OrderQty,
                                Tag::Price,
                                Tag::LeavesQty,
                                Tag::CumQty,
                                Tag::AvgPx,
                                Tag::LastPx,
                                // LastQty must be after LastPx for empty predicate to work.
                                Tag::LastQty,
                                Tag::MinQty
                                >;
// clang-format on

using Logon = TagList<NoTagOpts, Tag::EncryptMethod, Tag::HeartBtInt>;

struct SWIRLY_API MarketDataSnapshot : TagList<NoTagOpts, Tag::Symbol, Tag::SettlDate> {
    using Base = TagList<NoTagOpts, Tag::Symbol, Tag::SettlDate>;
    void clear()
    {
        Base::clear();
        md_entries.clear();
    }
    std::vector<MdEntry> md_entries;
};

template <Tag GetN>
const auto& get(const MarketDataSnapshot& mds) noexcept
{
    return mds.get(TagType<GetN>{});
}

template <Tag GetN>
auto& get(MarketDataSnapshot& mds) noexcept
{
    return mds.get(TagType<GetN>{});
}

template <Tag GetN>
void set(MarketDataSnapshot& mds, std::string_view sv)
{
    mds.set(TagType<GetN>{}, sv);
}

template <typename StreamT>
StreamT& operator<<(StreamT& os, const MarketDataSnapshot& mds)
{
    mds.put(os);
    const auto n = static_cast<int>(mds.md_entries.size());
    os << put_fix<Tag::NoMdEntries>(n);
    for (const auto& md_entry : mds.md_entries) {
        os << md_entry;
    }
    return os;
}

using TradingSessionStatus = TagList<NoTagOpts, Tag::TradingSessionId, Tag::TradSesStatus>;

SWIRLY_API void parse_body(FixLexer& lex, ExecutionReport& body);
SWIRLY_API void parse_body(FixLexer& lex, Logon& body);
SWIRLY_API void parse_body(FixLexer& lex, MarketDataSnapshot& body);
SWIRLY_API void parse_body(FixLexer& lex, TradingSessionStatus& body);

template <typename BodyT>
void parse_body(std::string_view msg, std::size_t body_off, BodyT& body)
{
    msg.remove_suffix(CheckSumLen);
    FixLexer lex{msg, body_off};
    parse_body(lex, body);
}

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_MSG_HPP
