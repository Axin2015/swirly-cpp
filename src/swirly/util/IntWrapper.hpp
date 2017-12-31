/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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
#ifndef SWIRLY_UTIL_INTWRAPPER_HPP
#define SWIRLY_UTIL_INTWRAPPER_HPP

#include <swirly/Config.h>

#include <cstdint>
#include <iosfwd>
#include <limits>
#include <type_traits>

namespace swirly {

template <typename ValueT>
struct IntPolicy {
    using ValueType = ValueT;
    static constexpr ValueT min() noexcept { return std::numeric_limits<ValueT>::min(); }
    static constexpr ValueT max() noexcept { return std::numeric_limits<ValueT>::max(); }
};
using Int32Policy = IntPolicy<std::int32_t>;
using Int64Policy = IntPolicy<std::int64_t>;

struct IntBase {
};

template <typename PolicyT>
struct SWIRLY_PACKED IntWrapper : IntBase {
    using ValueType = typename PolicyT::ValueType;

    template <typename RhsT,
              typename = typename std::enable_if_t<std::is_convertible<RhsT, ValueType>::value>>
    constexpr explicit IntWrapper(RhsT rhs) noexcept
      : value_{static_cast<ValueType>(rhs)}
    {
    }

    constexpr IntWrapper() noexcept = default;
    ~IntWrapper() noexcept = default;

    // Copy.
    constexpr IntWrapper(const IntWrapper&) noexcept = default;
    constexpr IntWrapper& operator=(const IntWrapper&) noexcept = default;

    // Move.
    constexpr IntWrapper(IntWrapper&&) noexcept = default;
    constexpr IntWrapper& operator=(IntWrapper&&) noexcept = default;

    constexpr ValueType count() const noexcept { return value_; }

    static constexpr IntWrapper min() noexcept { return PolicyT::min(); }
    static constexpr IntWrapper max() noexcept { return PolicyT::max(); }
    static constexpr IntWrapper zero() noexcept { return IntWrapper{0}; }

    // Assignment.

    /**
     * Addition assignment.
     */
    IntWrapper& operator+=(IntWrapper rhs) noexcept
    {
        value_ += rhs.value_;
        return *this;
    }

    /**
     * Subtraction assignment.
     */
    IntWrapper& operator-=(IntWrapper rhs) noexcept
    {
        value_ -= rhs.value_;
        return *this;
    }

    /**
     * Multiplication assignment.
     */
    IntWrapper& operator*=(IntWrapper rhs) noexcept
    {
        value_ *= rhs.value_;
        return *this;
    }

    /**
     * Division assignment.
     */
    IntWrapper& operator/=(IntWrapper rhs) noexcept
    {
        value_ /= rhs.value_;
        return *this;
    }

    /**
     * Modulo assignment.
     */
    IntWrapper& operator%=(IntWrapper rhs) noexcept
    {
        value_ %= rhs.value_;
        return *this;
    }

    /**
     * Bitwise AND assignment.
     */
    IntWrapper& operator&=(IntWrapper rhs) noexcept
    {
        value_ &= rhs.value_;
        return *this;
    }

    /**
     * Bitwise OR assignment.
     */
    IntWrapper& operator|=(IntWrapper rhs) noexcept
    {
        value_ |= rhs.value_;
        return *this;
    }

    /**
     * Bitwise XOR assignment.
     */
    IntWrapper& operator^=(IntWrapper rhs) noexcept
    {
        value_ ^= rhs.value_;
        return *this;
    }

    /**
     * Bitwise left shift assignment.
     */
    IntWrapper& operator<<=(IntWrapper rhs) noexcept
    {
        value_ <<= rhs.value_;
        return *this;
    }

    /**
     * Bitwise right shift assignment.
     */
    IntWrapper& operator>>=(IntWrapper rhs) noexcept
    {
        value_ >>= rhs.value_;
        return *this;
    }

    // Increment/Decrement.

    /**
     * Pre-increment.
     */
    IntWrapper& operator++() noexcept
    {
        ++value_;
        return *this;
    }

    /**
     * Pre-decrement.
     */
    IntWrapper& operator--() noexcept
    {
        --value_;
        return *this;
    }

    /**
     * Post-increment.
     */
    IntWrapper operator++(int)noexcept { return IntWrapper{value_++}; }

    /**
     * Post-decrement.
     */
    IntWrapper operator--(int)noexcept { return IntWrapper{value_--}; }

    // Arithmetic.

    /**
     * Unary plus.
     */
    constexpr IntWrapper operator+() const noexcept { return IntWrapper{+value_}; }

    /**
     * Unary minus.
     */
    constexpr IntWrapper operator-() const noexcept { return IntWrapper{-value_}; }

    /**
     * Addition.
     */
    friend constexpr IntWrapper operator+(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return IntWrapper{lhs.value_ + rhs.value_};
    }

    /**
     * Subtraction.
     */
    friend constexpr IntWrapper operator-(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return IntWrapper{lhs.value_ - rhs.value_};
    }

    /**
     * Multiplication.
     */
    friend constexpr IntWrapper operator*(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return IntWrapper{lhs.value_ * rhs.value_};
    }

    /**
     * Division.
     */
    friend constexpr IntWrapper operator/(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return IntWrapper{lhs.value_ / rhs.value_};
    }

    /**
     * Modulo.
     */
    friend constexpr IntWrapper operator%(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return IntWrapper{lhs.value_ % rhs.value_};
    }

    // Bitwise.

    /**
     * Bitwise NOT.
     */
    constexpr IntWrapper operator~() const noexcept { return IntWrapper{~value_}; }

    /**
     * Bitwise AND.
     */
    friend constexpr IntWrapper operator&(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return IntWrapper{lhs.value_ & rhs.value_};
    }

    /**
     * Bitwise OR.
     */
    friend constexpr IntWrapper operator|(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return IntWrapper{lhs.value_ | rhs.value_};
    }

    /**
     * Bitwise XOR.
     */
    friend constexpr IntWrapper operator^(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return IntWrapper{lhs.value_ ^ rhs.value_};
    }

    /**
     * Bitwise left shift.
     */
    friend constexpr IntWrapper operator<<(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return IntWrapper{lhs.value_ << rhs.value_};
    }

    /**
     * Bitwise right shift.
     */
    friend constexpr IntWrapper operator>>(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return IntWrapper{lhs.value_ >> rhs.value_};
    }

    // Comparison.

    /**
     * Equal to.
     */
    friend constexpr bool operator==(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return lhs.value_ == rhs.value_;
    }

    /**
     * Not equal to.
     */
    friend constexpr bool operator!=(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return lhs.value_ != rhs.value_;
    }

    /**
     * Less than.
     */
    friend constexpr bool operator<(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return lhs.value_ < rhs.value_;
    }

    /**
     * Greater than.
     */
    friend constexpr bool operator>(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return lhs.value_ > rhs.value_;
    }

    /**
     * Less than or equal to.
     */
    friend constexpr bool operator<=(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return lhs.value_ <= rhs.value_;
    }

    /**
     * Greater than or equal to.
     */
    friend constexpr bool operator>=(IntWrapper lhs, IntWrapper rhs) noexcept
    {
        return lhs.value_ >= rhs.value_;
    }

    // Stream.

    /**
     * Insertion.
     */
    friend std::ostream& operator<<(std::ostream& os, IntWrapper rhs) { return os << rhs.value_; }

  private:
    ValueType value_;
};
static_assert(std::is_pod<IntWrapper<Int32Policy>>::value);
static_assert(sizeof(IntWrapper<Int32Policy>) == 4, "must be specific size");
static_assert(sizeof(IntWrapper<Int64Policy>) == 8, "must be specific size");

template <typename ValueT>
constexpr bool isIntWrapper = std::is_base_of<IntBase, ValueT>::value;

} // namespace swirly

#endif // SWIRLY_UTIL_INTWRAPPER_HPP
