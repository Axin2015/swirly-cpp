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
#ifndef SWIRLY_ASH_RINGBUFFER_HPP
#define SWIRLY_ASH_RINGBUFFER_HPP

#include <swirly/ash/Math.hpp>

#include <cstdint>

namespace swirly {

/**
 * @addtogroup Util
 * @{
 */

template <typename ValueT, std::size_t CapacityN>
class RingBuffer {
  static_assert(swirly::isPow2(CapacityN), "capacity must be power of two");
  enum : std::size_t { Mask = CapacityN - 1 };

 public:
  RingBuffer() = default;
  ~RingBuffer() noexcept = default;

  // Copy.
  RingBuffer(const RingBuffer& rhs) = delete;
  RingBuffer& operator=(const RingBuffer& rhs) = delete;

  // Move.
  RingBuffer(RingBuffer&&) = delete;
  RingBuffer& operator=(RingBuffer&&) = delete;

  bool empty() const noexcept { return rpos_ == wpos_; }
  bool full() const noexcept { return size() == CapacityN; }
  std::size_t size() const noexcept { return wpos_ - rpos_; }
  void clear() noexcept { rpos_ = wpos_ = 0; }
  void read(ValueT& val)
  {
    const auto& ref = buf_[rpos_ & Mask];
    val = ref;
    ++rpos_;
  }
  template <typename FnT>
  void read(FnT fn)
  {
    const auto& ref = buf_[rpos_ & Mask];
    fn(ref);
    ++rpos_;
  }
  void write(const ValueT& val)
  {
    auto& ref = buf_[wpos_ & Mask];
    ref = val;
    if (full()) {
      ++rpos_;
    }
    ++wpos_;
  }
  template <typename FnT>
  void write(FnT fn)
  {
    auto& ref = buf_[wpos_ & Mask];
    fn(ref);
    if (full()) {
      ++rpos_;
    }
    ++wpos_;
  }

 private:
  // Ensure that read and write positions are in different cache-lines.
  struct alignas(64) {
    uint64_t rpos_{0};
  };
  uint64_t wpos_{0};
  ValueT buf_[CapacityN];
};

/** @} */

} // swirly

#endif // SWIRLY_ASH_RINGBUFFER_HPP
