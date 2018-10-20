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
#ifndef SWIRLY_FIX_TRAITS_HPP
#define SWIRLY_FIX_TRAITS_HPP

#include <swirly/fix/Stream.hpp>
#include <swirly/fix/Tag.hpp>

#include <swirly/app/Exception.hpp>

#include <swirly/util/Date.hpp>
#include <swirly/util/Enum.hpp>
#include <swirly/util/StringBuf.hpp>
#include <swirly/util/Symbol.hpp>

namespace swirly {
inline namespace fix {

template <typename ValueT, typename EnableT = void>
struct FixTraits {
    using ValueType = ValueT;
    using ViewType = ValueT;
    static void parse(std::string_view sv, ValueT& val) { val = from_string<ValueT>(sv); }
    static void put(FixStream& os, const ValueT& val) { os << val; }
    static void put(std::ostream& os, const ValueT& val) { os << val; }
    static void clear(ValueT& val) { val = ValueT{}; }
};

template <>
struct FixTraits<bool> {
    using ValueType = bool;
    using ViewType = bool;
    static void parse(std::string_view sv, bool& val) noexcept { val = stob(sv); }
    static void put(FixStream& os, bool val) { os.put(val ? 'Y' : 'N'); }
    static void put(std::ostream& os, bool val) { os.put(val ? 'Y' : 'N'); }
    static void clear(bool& val) noexcept { val = false; }
};

template <>
struct FixTraits<ExecType> {
    using ValueType = ExecType;
    using ViewType = ExecType;
    static constexpr void parse(std::string_view sv, ExecType& val) noexcept
    {
        if (sv.size() != 1) {
            val.state = State::None;
            return;
        }
        switch (sv[0]) {
        case '0':
            val.state = State::New;
            break;
        case '4':
            val.state = State::Cancel;
            break;
        case '5':
            val.state = State::Revise;
            break;
        case 'F':
            val.state = State::Trade;
            break;
        default:
            val.state = State::None;
        }
    }
    static void put(FixStream& os, ExecType val) { os << to_char(val); }
    static void put(std::ostream& os, ExecType val) { os << to_char(val); }
    static void clear(ExecType& val) noexcept { val = ExecType{}; }
    static char to_char(ExecType val)
    {
        char ch;
        switch (val.state) {
        case State::New:
            ch = '0';
            break;
        case State::Revise:
            ch = '5';
            break;
        case State::Cancel:
            ch = '4';
            break;
        case State::Trade:
            ch = 'F';
            break;
        case State::None:
        default:
            throw std::domain_error{"invalid state"};
        }
        return ch;
    }
};

template <>
struct FixTraits<MdEntryType> {
    using ValueType = MdEntryType;
    using ViewType = MdEntryType;
    static constexpr void parse(std::string_view sv, MdEntryType& val) noexcept
    {
        if (sv.size() != 1) {
            val.side = Side::None;
            return;
        }
        switch (sv[0]) {
        case '0':
            val.side = Side::Buy;
            break;
        case '1':
            val.side = Side::Sell;
            break;
        default:
            val.side = Side::None;
        }
    }
    static void put(FixStream& os, MdEntryType val) { os << to_char(val); }
    static void put(std::ostream& os, MdEntryType val) { os << to_char(val); }
    static void clear(MdEntryType& val) noexcept { val = MdEntryType{}; }
    static char to_char(MdEntryType val)
    {
        char ch;
        switch (val.side) {
        case Side::Buy:
            ch = '0';
            break;
        case Side::Sell:
            ch = '1';
            break;
        case Side::None:
        default:
            throw std::domain_error{"invalid side"};
        }
        return ch;
    }
};

template <>
struct FixTraits<OrdStatus> {
    using ValueType = OrdStatus;
    using ViewType = OrdStatus;
    static constexpr void parse(std::string_view sv, OrdStatus& val) noexcept
    {
        if (sv.size() != 1) {
            val.state = State::None;
            return;
        }
        switch (sv[0]) {
        case '0':
            val.state = State::New;
            break;
        case '1':
        case '2':
            val.state = State::Trade;
            break;
        case '4':
            val.state = State::Cancel;
            break;
        case '5':
            val.state = State::Revise;
            break;
        default:
            val.state = State::None;
        }
    }
    static void put(FixStream& os, OrdStatus val) { os << to_char(val); }
    static void put(std::ostream& os, OrdStatus val) { os << to_char(val); }
    static void clear(OrdStatus& val) noexcept { val = OrdStatus{}; }
    static char to_char(OrdStatus val)
    {
        char ch;
        switch (val.state) {
        case State::New:
            ch = '0';
            break;
        case State::Revise:
            ch = '5';
            break;
        case State::Cancel:
            ch = '4';
            break;
        case State::Trade:
            ch = val.filled ? '2' : '1';
            break;
        case State::None:
        default:
            throw std::domain_error{"invalid state"};
        }
        return ch;
    }
};

template <>
struct FixTraits<Seconds> {
    using ValueType = Seconds;
    using ViewType = Seconds;
    static constexpr void parse(std::string_view sv, Seconds& val) noexcept
    {
        val = from_string<Seconds>(sv);
    }
    static void put(FixStream& os, Seconds val) { os.put_num(val.count()); }
    static void put(std::ostream& os, Seconds val) { os << val.count(); }
    static void clear(Seconds& val) noexcept { val = Seconds{}; }
};

template <>
struct FixTraits<Side> {
    using ValueType = Side;
    using ViewType = Side;
    static constexpr void parse(std::string_view sv, Side& val) noexcept
    {
        if (sv.size() != 1) {
            val = Side::None;
            return;
        }
        switch (sv[0]) {
        case '1':
            val = Side::Buy;
            break;
        case '2':
            val = Side::Sell;
            break;
        default:
            val = Side::None;
        }
    }
    static void put(FixStream& os, Side val) { os << to_char(val); }
    static void put(std::ostream& os, Side val) { os << to_char(val); }
    static void clear(Side& val) noexcept { val = Side{}; }
    static char to_char(Side val)
    {
        char ch;
        switch (val) {
        case Side::Buy:
            ch = '1';
            break;
        case Side::Sell:
            ch = '2';
            break;
        case Side::None:
        default:
            throw std::domain_error{"invalid side"};
        }
        return ch;
    }
};

template <>
struct FixTraits<WallTime> {
    using ValueType = WallTime;
    using ViewType = WallTime;
    static void parse(std::string_view sv, WallTime& val)
    {
        auto [time, valid] = parse_time(sv);
        if (!valid) {
            throw ProtocolException{"invalid FIX time"};
        }
        val = time;
    }
    static void put(FixStream& os, WallTime val) { os << put_time<Millis>(val, "%Y%m%d-%T"); }
    static void put(std::ostream& os, WallTime val) { os << put_time<Millis>(val, "%Y%m%d-%T"); }
    static void clear(WallTime& val) noexcept { val = WallTime{}; }
};

template <typename ValueT>
struct FixTraits<ValueT, std::enable_if_t<is_int_wrapper<ValueT>>> {
    using ValueType = ValueT;
    using ViewType = ValueT;
    static constexpr void parse(std::string_view sv, ValueT& val) noexcept
    {
        val = from_string<ValueT>(sv);
    }
    static void put(FixStream& os, ValueT val) { os.put_num(val.count()); }
    static void put(std::ostream& os, ValueT val) { os << val; }
    static void clear(ValueT& val) noexcept { val = ValueT{}; }
};

// FIXME: switch to std::is_arithmetic_v<> once GCC and Clang have floating-point overloads for the
// std::to_chars() function.
template <typename ValueT>
struct FixTraits<ValueT, std::enable_if_t<std::is_integral_v<ValueT>>> {
    using ValueType = ValueT;
    using ViewType = ValueT;
    static constexpr void parse(std::string_view sv, ValueT& val) noexcept
    {
        val = from_string<ValueT>(sv);
    }
    static void put(FixStream& os, ValueT val) { os.put_num(val); }
    static void put(std::ostream& os, ValueT val) { os << val; }
    static void clear(ValueT& val) noexcept { val = ValueT{}; }
};

template <std::size_t MaxN>
struct StringTraits {
    using ValueType = StringBuf<MaxN>;
    using ViewType = std::string_view;
    static void parse(std::string_view sv, ValueType& val) { val = sv; }
    static constexpr void parse(std::string_view sv, ViewType& val) noexcept { val = sv; }
    static void put(FixStream& os, const ValueType& val) { os << val; }
    static void put(FixStream& os, ViewType val) { os << val; }
    static void put(std::ostream& os, const ValueType& val) { os << val; }
    static void put(std::ostream& os, ViewType val) { os << val; }
    static void clear(ValueType& val) noexcept { val.clear(); }
    static void clear(ViewType& val) noexcept { val = ViewType{}; }
};

// Basic types.
using BoolTraits = FixTraits<bool>;
using CharTraits = FixTraits<char>;
using DateTraits = FixTraits<IsoDate>;
// Double fields should be avoided for performance reasons.
// Use ticks and lots instead of floating-point price and quantity.
using DoubleTraits = FixTraits<double>;
using ExecTypeTraits = FixTraits<ExecType>;
using Id64Traits = FixTraits<Id64>;
using IntTraits = FixTraits<int>;
using MdEntryTraits = FixTraits<MdEntryType>;
using OrdStatusTraits = FixTraits<OrdStatus>;
using PriceTraits = FixTraits<Ticks>;
using QtyTraits = FixTraits<Lots>;
using SecondsTraits = FixTraits<Seconds>;
using SideTraits = FixTraits<Side>;
using SymbolTraits = FixTraits<Symbol>;
using TimeTraits = FixTraits<WallTime>;

// Type aliases.
using CompIdTraits = StringTraits<32>;
using LengthTraits = IntTraits;
using LocalMktDateTraits = DateTraits;
using NumInGroupTraits = IntTraits;
using SeqNumTraits = IntTraits;
using UtcDateOnlyTraits = DateTraits;

template <Tag TagN>
struct TagTraits;

template <Tag TagN>
using FixValueType = typename TagTraits<TagN>::ValueType;

template <Tag TagN>
using FixViewType = typename TagTraits<TagN>::ViewType;

template <Tag TagN>
auto parse_fix_value(std::string_view sv)
{
    using TagTraits = fix::TagTraits<TagN>;
    typename TagTraits::ValueType val;
    TagTraits::parse(sv, val);
    return val;
}

template <Tag TagN>
auto parse_fix_view(std::string_view sv)
{
    using TagTraits = fix::TagTraits<TagN>;
    typename TagTraits::ViewType val;
    TagTraits::parse(sv, val);
    return val;
}

// clang-format off
template <> struct TagTraits<Tag::AvgPx> : PriceTraits {};
template <> struct TagTraits<Tag::BeginString> : StringTraits<7> {};
template <> struct TagTraits<Tag::BodyLength> : LengthTraits {};
template <> struct TagTraits<Tag::CumQty> : QtyTraits {};
template <> struct TagTraits<Tag::EncryptMethod> : IntTraits {};
template <> struct TagTraits<Tag::ExecId> : Id64Traits {};
template <> struct TagTraits<Tag::ExecType> : ExecTypeTraits {};
template <> struct TagTraits<Tag::HeartBtInt> : SecondsTraits {};
template <> struct TagTraits<Tag::LastPx> : PriceTraits {};
template <> struct TagTraits<Tag::LastQty> : QtyTraits {};
template <> struct TagTraits<Tag::LeavesQty> : QtyTraits {};
template <> struct TagTraits<Tag::SettlDate> : DateTraits {};
template <> struct TagTraits<Tag::MdEntryPx> : PriceTraits {};
template <> struct TagTraits<Tag::MdEntrySize> : QtyTraits {};
template <> struct TagTraits<Tag::MdEntryType> : MdEntryTraits {};
template <> struct TagTraits<Tag::MinQty> : QtyTraits {};
template <> struct TagTraits<Tag::MsgSeqNum> : SeqNumTraits {};
template <> struct TagTraits<Tag::MsgType> : StringTraits<2> {};
template <> struct TagTraits<Tag::NoMdEntries> : NumInGroupTraits {};
template <> struct TagTraits<Tag::OrdStatus> : OrdStatusTraits {};
template <> struct TagTraits<Tag::OrderId> : Id64Traits {};
template <> struct TagTraits<Tag::OrderQty> : QtyTraits {};
template <> struct TagTraits<Tag::PossDupFlag> : BoolTraits {};
template <> struct TagTraits<Tag::PossResend> : BoolTraits {};
template <> struct TagTraits<Tag::Price> : PriceTraits {};
template <> struct TagTraits<Tag::SenderCompId> : CompIdTraits {};
template <> struct TagTraits<Tag::SendingTime> : TimeTraits {};
template <> struct TagTraits<Tag::Side> : SideTraits {};
template <> struct TagTraits<Tag::Symbol> : SymbolTraits {};
template <> struct TagTraits<Tag::TargetCompId> : CompIdTraits {};
template <> struct TagTraits<Tag::TradSesStatus> : IntTraits {};
template <> struct TagTraits<Tag::TradingSessionId> : StringTraits<32> {};
// clang-format on

template <Tag TagN>
struct PutFix {
    FixViewType<TagN> value;
};

template <Tag TagN>
FixStream& operator<<(FixStream& os, const PutFix<TagN>& field)
{
    os.put_num(+TagN);
    os.put('=');
    TagTraits<TagN>::put(os, field.value);
    os.put('\1');
    return os;
}

template <Tag TagN>
std::ostream& operator<<(std::ostream& os, const PutFix<TagN>& field)
{
    os << +TagN;
    os.put('=');
    TagTraits<TagN>::put(os, field.value);
    os.put('\1');
    return os;
}

template <Tag TagN, typename ArgT>
auto put_fix(ArgT&& arg)
{
    return PutFix<TagN>{std::forward<ArgT>(arg)};
}

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_TRAITS_HPP
