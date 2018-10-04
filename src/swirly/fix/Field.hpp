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

#include <swirly/util/Date.hpp>
#include <swirly/util/StringBuf.hpp>
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

    template <typename ArgT>
    constexpr FixField(ArgT&& arg)
    : value(std::forward<ArgT>(arg))
    {
    }
    template <typename ArgT>
    constexpr FixField& operator=(ArgT&& arg)
    {
        value = std::forward<ArgT>(arg);
        return *this;
    }
    static constexpr int Tag = TagN;
    ValueT value{};
};

template <int TagN, typename ValueT>
bool operator==(const FixField<TagN, ValueT>& lhs, const FixField<TagN, ValueT>& rhs) noexcept
{
    return lhs.value == rhs.value;
}

template <int TagN, typename ValueT>
bool operator!=(const FixField<TagN, ValueT>& lhs, const FixField<TagN, ValueT>& rhs) noexcept
{
    return lhs.value != rhs.value;
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
using LongField = FixField<TagN, long>;

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
using PriceField = LongField<TagN>;

template <int TagN>
using QtyField = LongField<TagN>;

using BeginString = StringField<8, 7>;
using BodyLength = LengthField<9>;
using MsgSeqNum = SeqNumField<34>;
using MsgType = StringField<35, 2>;
using SenderCompId = StringField<49, 32>;
using PossDupFlag = BoolField<43>;
using SendingTime = TimeField<52>;
using SymbolField = StringField<55, 16>;
using TargetCompId = StringField<56, 32>;
using PossResend = BoolField<97>;
using EncryptMethod = IntField<98>;
using HeartBtInt = IntField<108>;
using NoMdEntries = NumInGroupField<268>;
using MdEntryType = CharField<269>;
using MdEntryPx = PriceField<270>;
using MdEntrySize = QtyField<271>;
using TradingSessionId = StringField<336, 32>;
using TradSesStatus = IntField<340>;
using MaturityDate = LocalMktDateField<541>;

static_assert(FixField<8, std::string_view>{}.Tag == 8);
static_assert(BodyLength{101}.value == BodyLength{101}.value);

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_FIELD_HPP
