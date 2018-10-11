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
#ifndef SWIRLY_FIX_FIELD_HPP
#define SWIRLY_FIX_FIELD_HPP

#include <swirly/fix/Stream.hpp>

#include <swirly/fin/IntTypes.hpp>

#include <swirly/util/Date.hpp>
#include <swirly/util/StringBuf.hpp>
#include <swirly/util/Symbol.hpp>
#include <swirly/util/Time.hpp>

namespace swirly {
inline namespace fix {

template <typename ValueT>
struct ViewTraits {
    using Type = void;
};

template <>
struct ViewTraits<std::string> {
    using Type = std::string_view;
};

template <std::size_t MaxN>
struct ViewTraits<StringBuf<MaxN>> {
    using Type = std::string_view;
};

template <int TagN, typename ValueT>
struct FixField {
    using Type = ValueT;
    using View = FixField<TagN, typename ViewTraits<ValueT>::Type>;

    FixField() = default;
    ~FixField() = default;

    // Copy.
    FixField(const FixField&) = default;
    FixField& operator=(const FixField&) = default;

    // Move.
    FixField(FixField&&) = default;
    FixField& operator=(FixField&&) = default;

    constexpr FixField(std::string_view sv)
    : value{from_string<ValueT>(sv)}
    {
    }
    constexpr FixField(const std::string& s)
    : value{from_string<ValueT>(s)}
    {
    }
    constexpr FixField(const char* s)
    : value{from_string<ValueT>(s)}
    {
    }
    template <typename ArgT>
    constexpr FixField(ArgT&& arg)
    : value{std::forward<ArgT>(arg)}
    {
    }
    constexpr FixField& operator=(std::string_view sv)
    {
        value = from_string<ValueT>(sv);
        return *this;
    }
    constexpr FixField& operator=(const std::string& s)
    {
        value = from_string<ValueT>(s);
        return *this;
    }
    constexpr FixField& operator=(const char* s)
    {
        value = from_string<ValueT>(s);
        return *this;
    }
    template <typename ArgT>
    constexpr FixField& operator=(ArgT&& arg)
    {
        value = std::forward<ArgT>(arg);
        return *this;
    }
    void clear() { value = ValueT{}; }
    static constexpr int Tag = TagN;
    ValueT value{};
};

template <int TagN, typename ValueT>
bool operator==(const FixField<TagN, ValueT>& lhs, const FixField<TagN, ValueT>& rhs) noexcept
{
    return lhs.value == rhs.value;
}

template <int TagN, typename ValueT, typename RhsT>
bool operator==(const FixField<TagN, ValueT>& lhs, const RhsT& rhs) noexcept
{
    return lhs.value == rhs;
}

template <int TagN, typename ValueT, typename LhsT>
bool operator==(const LhsT& lhs, const FixField<TagN, ValueT>& rhs) noexcept
{
    return lhs == rhs.value;
}

template <int TagN, typename ValueT>
bool operator!=(const FixField<TagN, ValueT>& lhs, const FixField<TagN, ValueT>& rhs) noexcept
{
    return lhs.value != rhs.value;
}

template <int TagN, typename ValueT, typename RhsT>
bool operator!=(const FixField<TagN, ValueT>& lhs, const RhsT& rhs) noexcept
{
    return lhs.value != rhs;
}

template <int TagN, typename ValueT, typename LhsT>
bool operator!=(const LhsT& lhs, const FixField<TagN, ValueT>& rhs) noexcept
{
    return lhs != rhs.value;
}

template <int TagN, typename ValueT>
FixStream& operator<<(FixStream& os, const FixField<TagN, ValueT>& field)
{
    os.put_num(field.Tag);
    os.put('=');
    if constexpr (std::is_same_v<ValueT, WallTime>) {
        os << put_time<Millis>(field.value, "%Y%m%d-%T");
    } else if constexpr (std::is_same_v<ValueT, bool>) {
        os.put(field.value ? 'Y' : 'N');
    } else if constexpr (std::is_integral_v<ValueT>) {
        // FIXME: switch to std::is_arithmetic_v<> once GCC and Clang have floating-point overloads
        // for the std::to_chars() function.
        os.put_num(field.value);
    } else {
        os << field.value;
    }
    os.put('\1');
    return os;
}

template <int TagN, typename ValueT>
std::ostream& operator<<(std::ostream& os, const FixField<TagN, ValueT>& field)
{
    os << field.Tag;
    os.put('=');
    if constexpr (std::is_same_v<ValueT, WallTime>) {
        os << put_time<Millis>(field.value, "%Y%m%d-%T");
    } else if constexpr (std::is_same_v<ValueT, bool>) {
        os.put(field.value ? 'Y' : 'N');
    } else {
        os << field.value;
    }
    os.put('\1');
    return os;
}

template <int TagN>
using BoolField = FixField<TagN, bool>;

template <int TagN>
using CharField = FixField<TagN, char>;

template <int TagN>
using DateField = FixField<TagN, IsoDate>;

// Double fields should be avoided for performance reasons.
// Ticks and lots should be used instead of price and quantity.
template <int TagN>
using DoubleField = FixField<TagN, double>;

template <int TagN>
using IntField = FixField<TagN, int>;

template <int TagN>
using Id64Field = FixField<TagN, Id64>;

template <int TagN, std::size_t MaxN>
using StringField = FixField<TagN, StringBuf<MaxN>>;

template <int TagN>
using TimeField = FixField<TagN, WallTime>;

// Date aliases.

template <int TagN>
using LocalMktDateField = DateField<TagN>;

template <int TagN>
using UtcDateOnlyField = DateField<TagN>;

// Int aliases.

template <int TagN>
using LengthField = IntField<TagN>;

template <int TagN>
using NumInGroupField = IntField<TagN>;

template <int TagN>
using SeqNumField = IntField<TagN>;

// Long aliases.

template <int TagN>
using PriceField = FixField<TagN, Ticks>;

template <int TagN>
using QtyField = FixField<TagN, Lots>;

using AvgPx = PriceField<6>;
using BeginString = StringField<8, 7>;
using BodyLength = LengthField<9>;
using CumQty = QtyField<14>;
using EncryptMethod = IntField<98>;
using ExecId = Id64Field<17>;
using ExecType = CharField<150>;
using HeartBtInt = IntField<108>;
using LastPx = PriceField<31>;
using LastQty = QtyField<32>;
using LeavesQty = QtyField<151>;
using MaturityDate = LocalMktDateField<541>;
using MdEntryPx = PriceField<270>;
using MdEntrySize = QtyField<271>;
using MdEntryType = CharField<269>;
using MinQty = QtyField<110>;
using MsgSeqNum = SeqNumField<34>;
using MsgType = StringField<35, 2>;
using NoMdEntries = NumInGroupField<268>;
using OrdStatus = CharField<39>;
using OrderId = Id64Field<37>;
using OrderQty = QtyField<38>;
using PossDupFlag = BoolField<43>;
using PossResend = BoolField<97>;
using Price = PriceField<44>;
using SenderCompId = StringField<49, 32>;
using SendingTime = TimeField<52>;
using SideField = CharField<54>;
using SymbolField = FixField<55, Symbol>;
using TargetCompId = StringField<56, 32>;
using TradSesStatus = IntField<340>;
using TradingSessionId = StringField<336, 32>;

static_assert(FixField<8, std::string_view>{}.Tag == 8);
static_assert(BodyLength{101}.value == BodyLength{101}.value);

constexpr State from_fix(ExecType exec_type) noexcept
{
    State val{};
    switch (exec_type.value) {
    case '0':
        val = State::New;
        break;
    case '4':
        val = State::Cancel;
        break;
    case '5':
        val = State::Revise;
        break;
    case 'F':
        val = State::Trade;
        break;
    default:
        val = State::None;
    }
    return val;
}
static_assert(from_fix(ExecType{'F'}) == State::Trade);

constexpr State from_fix(OrdStatus ord_status) noexcept
{
    State val{};
    switch (ord_status.value) {
    case '0':
        val = State::New;
        break;
    case '1':
    case '2':
        val = State::Trade;
        break;
    case '4':
        val = State::Cancel;
        break;
    case '5':
        val = State::Revise;
        break;
    default:
        val = State::None;
    }
    return val;
}
static_assert(from_fix(OrdStatus{'1'}) == State::Trade);

template <typename FixFieldT>
inline FixFieldT to_fix(State state, Lots resd)
{
    typename FixFieldT::Type val;
    switch (state) {
    case State::New:
        val = '0';
        break;
    case State::Revise:
        val = '5';
        break;
    case State::Cancel:
        val = '4';
        break;
    case State::Trade:
        if constexpr (std::is_same_v<FixFieldT, ExecType>) {
            (void)resd;
            val = 'F';
        } else if constexpr (std::is_same_v<FixFieldT, OrdStatus>) {
            val = resd == 0_lts ? '2' : '1';
        } else {
            static_assert(DependentFalse<FixFieldT>::value);
        }
        break;
    case State::None:
    default:
        throw std::domain_error{"invalid state"};
    }
    return {val};
}

constexpr Side from_fix(SideField side) noexcept
{
    Side val{};
    switch (side.value) {
    case '1':
        val = Side::Buy;
        break;
    case '2':
        val = Side::Sell;
        break;
    default:
        val = Side::None;
    }
    return val;
}
static_assert(from_fix(SideField{'1'}) == Side::Buy);

inline SideField to_fix(Side side)
{
    SideField::Type val;
    switch (side) {
    case Side::Buy:
        val = '1';
        break;
    case Side::Sell:
        val = '2';
        break;
    case Side::None:
    default:
        throw std::domain_error{"invalid side"};
    }
    return {val};
}

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_FIELD_HPP
