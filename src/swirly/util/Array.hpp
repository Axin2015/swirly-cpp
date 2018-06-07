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
#ifndef SWIRLY_UTIL_ARRAY_HPP
#define SWIRLY_UTIL_ARRAY_HPP

#include <cstddef> // ptrdiff_t
#include <iterator>
#include <vector>

namespace swirly {
inline namespace util {

template <typename ValueT>
class ArrayView {
  public:
    using value_type = ValueT;

    using pointer = const ValueT*;
    using const_pointer = const ValueT*;

    using reference = const ValueT&;
    using const_reference = const ValueT&;

    using const_iterator = const ValueT*;
    using iterator = const_iterator;

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;

    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;

    constexpr ArrayView(const ValueT* ptr, std::size_t len) noexcept
    : len_{len}
    , ptr_{ptr}
    {
    }
    template <typename TypeU, std::size_t SizeN>
    constexpr ArrayView(TypeU (&arr)[SizeN]) noexcept
    : len_{SizeN}
    , ptr_{arr}
    {
    }
    ArrayView(const std::vector<ValueT>& arr) noexcept
    : len_{arr.size()}
    , ptr_{arr.empty() ? nullptr : &arr[0]}
    {
    }
    constexpr ArrayView() noexcept = default;
    ~ArrayView() = default;

    // Copy.
    constexpr ArrayView(const ArrayView& rhs) noexcept = default;
    constexpr ArrayView& operator=(const ArrayView& rhs) noexcept = default;

    // Move.
    constexpr ArrayView(ArrayView&&) noexcept = default;
    constexpr ArrayView& operator=(ArrayView&&) noexcept = default;

    constexpr const_iterator begin() const noexcept { return ptr_; }
    constexpr const_iterator end() const noexcept { return ptr_ + len_; }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    constexpr const_iterator cbegin() const noexcept { return ptr_; }
    constexpr const_iterator cend() const noexcept { return ptr_ + len_; }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
    constexpr const ValueT& operator[](size_type pos) const noexcept { return ptr_[pos]; }
    constexpr const ValueT& front() const noexcept { return ptr_[0]; }
    constexpr const ValueT& back() const noexcept { return ptr_[len_ - 1]; }
    constexpr const ValueT* data() const noexcept { return ptr_; }
    constexpr bool empty() const noexcept { return len_ == 0; }
    constexpr std::size_t size() const noexcept { return len_; }
    constexpr void clear() noexcept
    {
        len_ = 0;
        ptr_ = nullptr;
    }
    void swap(ArrayView& rhs) noexcept
    {
        std::swap(len_, rhs.len_);
        std::swap(ptr_, rhs.ptr_);
    }

  private:
    // Length in the first cache-line.
    std::size_t len_{0};
    const ValueT* ptr_{nullptr};
};

template <typename ValueT>
constexpr ArrayView<std::remove_volatile_t<ValueT>> make_array_view(const ValueT* ptr,
                                                                    std::size_t len) noexcept
{
    return {ptr, len};
}

template <typename ValueT, std::size_t SizeN>
constexpr ArrayView<std::remove_cv_t<ValueT>> make_array_view(ValueT (&arr)[SizeN]) noexcept
{
    return {arr};
}

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_ARRAY_HPP
