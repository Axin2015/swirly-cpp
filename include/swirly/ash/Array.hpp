/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
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
#ifndef SWIRLY_ASH_ARRAY_HPP
#define SWIRLY_ASH_ARRAY_HPP

#include <cstddef> // ptrdiff_t
#include <iterator>

namespace swirly {

/**
 * @addtogroup Util
 * @{
 */

template <typename TypeT>
class ArrayView {

    // Length in the first cache-line.
    std::size_t len_;
    const TypeT* ptr_;

public:
    using value_type = TypeT;

    using pointer = const TypeT*;
    using const_pointer = const TypeT*;

    using reference = const TypeT&;
    using const_reference = const TypeT&;

    using const_iterator = const TypeT*;
    using iterator = const_iterator;

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;

    using difference_type = ptrdiff_t;
    using size_type = std::size_t;

    constexpr ArrayView(const TypeT* ptr, std::size_t len) noexcept
    :   len_{len},
        ptr_{ptr}
    {
    }
    constexpr ArrayView() noexcept
    :   len_{0},
        ptr_{nullptr}
    {
    }

    ~ArrayView() noexcept = default;

    // Copy.
    constexpr ArrayView(const ArrayView& rhs) noexcept = default;
    constexpr ArrayView& operator =(const ArrayView& rhs) noexcept = default;

    // Move.
    constexpr ArrayView(ArrayView&&) noexcept = default;
    constexpr ArrayView& operator =(ArrayView&&) noexcept = default;

    void clear() noexcept
    {
        len_ = 0;
        ptr_ = nullptr;
    }
    void swap(ArrayView& rhs) noexcept
    {
        std::swap(len_, rhs.len_);
        std::swap(ptr_, rhs.ptr_);
    }
    constexpr const_iterator begin() const noexcept
    {
        return ptr_;
    }
    constexpr const_iterator end() const noexcept
    {
        return ptr_ + len_;
    }
    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }
    constexpr const_iterator cbegin() const noexcept
    {
        return ptr_;
    }
    constexpr const_iterator cend() const noexcept
    {
        return ptr_ + len_;
    }
    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(begin());
    }
    constexpr const TypeT& operator [](size_type pos) const noexcept
    {
        return ptr_[pos];
    }
    constexpr const TypeT& front() const noexcept
    {
        return ptr_[0];
    }
    constexpr const TypeT& back() const noexcept
    {
        return ptr_[len_ - 1];
    }
    constexpr const TypeT* data() const noexcept
    {
        return ptr_;
    }
    constexpr bool empty() const noexcept
    {
        return len_ == 0;
    }
    constexpr size_t size() const noexcept
    {
        return len_;
    }
};

/** @} */

} // swirly

#endif // SWIRLY_ASH_ARRAY_HPP
