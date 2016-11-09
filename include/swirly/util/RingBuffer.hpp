/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2016 Swirly Cloud Limited.
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
#ifndef SWIRLY_UTIL_RINGBUFFER_HPP
#define SWIRLY_UTIL_RINGBUFFER_HPP

#include <swirly/util/Math.hpp>

#include <cstdint>
#include <memory>

namespace swirly {

template <typename ValueT>
class RingBuffer {
 public:
  explicit RingBuffer(std::size_t capacity)
    : capacity_{nextPow2(capacity)}, mask_{capacity_ - 1}, buf_{new ValueT[capacity_]}
  {
  }
  ~RingBuffer() noexcept = default;

  // Copy.
  RingBuffer(const RingBuffer& rhs) = delete;
  RingBuffer& operator=(const RingBuffer& rhs) = delete;

  // Move.
  RingBuffer(RingBuffer&&) = default;
  RingBuffer& operator=(RingBuffer&&) = default;

  bool empty() const noexcept { return rpos_ == wpos_; }
  bool full() const noexcept { return size() == capacity_; }
  std::size_t size() const noexcept { return wpos_ - rpos_; }
  const ValueT& front() const noexcept { return buf_[rpos_ & mask_]; }
  const ValueT& back() const noexcept { return buf_[wpos_ & mask_]; }

  ValueT& front() noexcept { return buf_[rpos_ & mask_]; }
  ValueT& back() noexcept { return buf_[wpos_ & mask_]; }
  void clear() noexcept { rpos_ = wpos_ = 0; }
  void pop() noexcept { ++rpos_; }
  void push(const ValueT& val)
  {
    auto& ref = buf_[wpos_ & mask_];
    ref = val;
    if (full()) {
      ++rpos_;
    }
    ++wpos_;
  }
  template <typename FnT>
  void read(FnT fn)
  {
    const auto& ref = buf_[rpos_ & mask_];
    fn(ref);
    ++rpos_;
  }
  template <typename FnT>
  void write(FnT fn)
  {
    auto& ref = buf_[wpos_ & mask_];
    fn(ref);
    if (full()) {
      ++rpos_;
    }
    ++wpos_;
  }

 private:
  // Ensure that read and write positions are in different cache-lines.
  struct alignas(64) {
    std::size_t capacity_;
    std::size_t mask_;
    uint64_t rpos_{0};
  };
  uint64_t wpos_{0};
  std::unique_ptr<ValueT[]> buf_;
};

} // swirly

#endif // SWIRLY_UTIL_RINGBUFFER_HPP
