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
#ifndef SWIRLY_DISRUPT_SEQUENCE_HPP
#define SWIRLY_DISRUPT_SEQUENCE_HPP

#include "Types.hpp"

#include <swirly/Config.h>

#include <atomic>

namespace swirly {

class SWIRLY_API Sequence {
  public:
    explicit Sequence(long value) noexcept
      : value_{value}
    {
    }
    ~Sequence() noexcept = default;

    // Copy.
    Sequence(const Sequence&) noexcept = default;
    Sequence& operator=(const Sequence&) noexcept = default;

    // Move.
    Sequence(Sequence&&) noexcept = default;
    Sequence& operator=(Sequence&&) noexcept = default;

    auto get() const noexcept { return value_.load(std::memory_order_acquire); }

    void set(long value) noexcept { value_.store(value, std::memory_order_release); }

    void setVolatile(long value) noexcept
    {
        // Java volatile has Store/Load barrier after write.
        value_.store(value, std::memory_order_seq_cst);
    }

    bool compareAndSetWeak(long expectedValue, long newValue) noexcept
    {
        return value_.compare_exchange_weak(expectedValue, newValue, std::memory_order_release,
                                            std::memory_order_relaxed);
    }

    bool compareAndSetStrong(long expectedValue, long newValue) noexcept
    {
        return value_.compare_exchange_strong(expectedValue, newValue, std::memory_order_release,
                                              std::memory_order_relaxed);
    }

    long incrementAndGet() noexcept { return addAndGet(1L); }

    long addAndGet(long increment) noexcept { return value_.fetch_add(increment) + increment; }

  private:
    // Avoid false sharing.
    alignas(64) std::atomic<long> value_;
};

} // namespace swirly

#endif // SWIRLY_DISRUPT_SEQUENCE_HPP
