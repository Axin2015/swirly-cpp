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

#include <swirly/util/StringBuf.hpp>
#include <swirly/util/Time.hpp>

#include <ostream>

namespace swirly {
inline namespace fix {

template <int TagN, typename ValueT, typename EnableT = void>
class FixField;

template <int TagN, typename ValueT>
class FixField<TagN, ValueT, std::enable_if_t<!std::is_arithmetic_v<ValueT>>> {
  public:
    using ValueType = ValueT;

    FixField() = default;
    ~FixField() = default;

    // Copy.
    FixField(const FixField&) = default;
    FixField& operator=(const FixField&) = default;

    // Move.
    FixField(FixField&&) = default;
    FixField& operator=(FixField&&) = default;

    FixField(const ValueT& value)
    : value_{value}
    {
    }
    FixField& operator=(const ValueT& value)
    {
        value_ = value;
        return *this;
    }
    FixField(ValueT&& value)
    : value_{std::move(value)}
    {
    }
    FixField& operator=(ValueT&& value)
    {
        value_ = std::move(value);
        return *this;
    }
    static constexpr int tag() noexcept { return TagN; }
    constexpr const ValueT& value() const noexcept { return value_; }

  private:
    ValueT value_{};
};

template <int TagN, typename ValueT>
class FixField<TagN, ValueT, std::enable_if_t<std::is_arithmetic_v<ValueT>>> {
  public:
    using ValueType = ValueT;

    constexpr FixField() noexcept = default;
    ~FixField() = default;

    // Copy.
    constexpr FixField(const FixField&) noexcept = default;
    constexpr FixField& operator=(const FixField&) noexcept = default;

    // Move.
    constexpr FixField(FixField&&) noexcept = default;
    constexpr FixField& operator=(FixField&&) noexcept = default;

    constexpr FixField(ValueT value) noexcept
    : value_{value}
    {
    }
    constexpr FixField& operator=(ValueT value) noexcept
    {
        value_ = value;
        return *this;
    }
    static constexpr int tag() noexcept { return TagN; }
    constexpr ValueT value() const noexcept { return value_; }

  private:
    ValueT value_{};
};

static_assert(FixField<9, int>{101}.value() == FixField<9, int>{101}.value());

template <int TagN, typename ValueT>
bool operator==(const FixField<TagN, ValueT>& lhs, const FixField<TagN, ValueT>& rhs) noexcept
{
    return lhs.value() == rhs.value();
}

template <int TagN, typename ValueT>
bool operator!=(const FixField<TagN, ValueT>& lhs, const FixField<TagN, ValueT>& rhs) noexcept
{
    return lhs.value() != rhs.value();
}

template <int TagN, typename ValueT>
std::ostream& operator<<(std::ostream& os, const FixField<TagN, ValueT>& field)
{
    os << field.tag() << '=';
    if constexpr (std::is_same_v<ValueT, Time>) {
        os << putTime<Millis>(field.value(), "%Y%m%d-%T");
    } else {
        os << field.value();
    }
    return os << '\1';
}

template <int TagN>
using DoubleFixField = FixField<TagN, double>;

template <int TagN>
using IntFixField = FixField<TagN, int>;

template <int TagN, std::size_t MaxN>
using StringFixField = FixField<TagN, StringBuf<MaxN>>;

template <int TagN>
using TimeFixField = FixField<TagN, Time>;

using BeginString = StringFixField<8, 7>;
using BodyLength = IntFixField<9>;
using MsgSeqNum = IntFixField<34>;
using MsgType = StringFixField<35, 2>;
using SenderCompId = StringFixField<49, 32>;
using SendingTime = TimeFixField<52>;
using TargetCompId = StringFixField<56, 32>;
using EncryptMethod = IntFixField<98>;
using HeartBtInt = IntFixField<108>;

} // namespace fix
} // namespace swirly

#endif // SWIRLY_FIX_FIELD_HPP
