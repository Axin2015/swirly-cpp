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
#ifndef SWIRLY_SYS_BOUNDEDQUEUE_HPP
#define SWIRLY_SYS_BOUNDEDQUEUE_HPP

#include <swirly/sys/Math.hpp>

#include <atomic>
#include <cassert>
#include <memory>

namespace swirly {

// Thanks to Dmitry Vyukov for this algorithm.
template <typename ValueT>
class BoundedQueue {
    static_assert(std::is_trivially_copyable_v<ValueT>);
    enum : std::size_t { CacheLineSize = 64 };
    struct alignas(CacheLineSize) Entry {
        std::atomic<std::size_t> seq;
        ValueT val;
    };

  public:
    explicit BoundedQueue(std::size_t capacity)
      : capacity_{nextPow2(capacity)}
      , mask_{capacity_ - 1}
      , buf_{new (std::align_val_t(alignof(Entry))) Entry[capacity_]}
    {
        assert(capacity >= 2);
        // Initialise sequence numbers.
        for (std::size_t i{0}; i < capacity; ++i) {
            buf_[i].seq.store(i, std::memory_order_relaxed);
        }
    }
    ~BoundedQueue() noexcept = default;

    // Copy.
    BoundedQueue(const BoundedQueue& rhs) = delete;
    BoundedQueue& operator=(const BoundedQueue& rhs) = delete;

    // Move.
    BoundedQueue(BoundedQueue&&) = delete;
    BoundedQueue& operator=(BoundedQueue&&) = delete;

    /**
     * Returns true if the queue is empty.
     */
    bool empty() const noexcept
    {
        // Acquire prevents reordering of these loads.
        const auto rpos = rpos_.load(std::memory_order_acquire);
        const auto wpos = wpos_.load(std::memory_order_relaxed);
        return rpos == wpos;
    }
    /**
     * Returns true if the number of elements in the queue.
     */
    std::size_t size() const noexcept
    {
        // Acquire prevents reordering of these loads.
        const auto rpos = rpos_.load(std::memory_order_acquire);
        const auto wpos = wpos_.load(std::memory_order_relaxed);
        return wpos - rpos;
    }
    /**
     * Returns false if queue is empty.
     */
    template <typename FnT>
    bool fetch(FnT fn) noexcept
    {
        static_assert(std::is_nothrow_invocable_v<FnT, ValueT&&>);
        auto rpos = rpos_.load(std::memory_order_relaxed);
        for (;;) {
            auto& entry = buf_[rpos & mask_];
            const auto seq = entry.seq.load(std::memory_order_acquire);
            const auto diff = static_cast<ssize_t>(seq) - static_cast<ssize_t>(rpos + 1);
            if (diff == 0) {
                // The compare_exchange_weak function re-reads wpos on failure.
                if (rpos_.compare_exchange_weak(rpos, rpos + 1, std::memory_order_relaxed)) {
                    fn(std::move(entry.val));
                    // Commit.
                    entry.seq.store(rpos + capacity_, std::memory_order_release);
                    break;
                }
            } else if (diff < 0) {
                return false;
            } else {
                rpos = rpos_.load(std::memory_order_relaxed);
            }
        }
        return true;
    }
    /**
     * Returns false if capacity is exceeded.
     */
    template <typename FnT>
    bool post(FnT fn) noexcept
    {
        static_assert(std::is_nothrow_invocable_v<FnT, ValueT&>);
        auto wpos = wpos_.load(std::memory_order_relaxed);
        for (;;) {
            auto& entry = buf_[wpos & mask_];
            const auto seq = entry.seq.load(std::memory_order_acquire);
            const auto diff = static_cast<ssize_t>(seq) - static_cast<ssize_t>(wpos);
            if (diff == 0) {
                // The compare_exchange_weak function re-reads wpos on failure.
                if (wpos_.compare_exchange_weak(wpos, wpos + 1, std::memory_order_relaxed)) {
                    fn(entry.val);
                    // Commit.
                    entry.seq.store(wpos + 1, std::memory_order_release);
                    break;
                }
            } else if (diff < 0) {
                return false;
            } else {
                wpos = wpos_.load(std::memory_order_relaxed);
            }
        }
        return true;
    }
    /**
     * Returns false if queue is empty.
     */
    bool pop(ValueT& val) noexcept
    {
        static_assert(std::is_nothrow_move_assignable_v<ValueT>);
        return fetch([&val](ValueT && ref) noexcept { val = std::move(ref); });
    }
    /**
     * Returns false if capacity is exceeded.
     */
    bool push(const ValueT& val) noexcept
    {
        static_assert(std::is_nothrow_copy_assignable_v<ValueT>);
        return post([&val](ValueT & ref) noexcept { ref = val; });
    }
    /**
     * Returns false if capacity is exceeded.
     */
    bool push(ValueT&& val) noexcept
    {
        static_assert(std::is_nothrow_move_assignable_v<ValueT>);
        return post([&val](ValueT & ref) noexcept { ref = std::move(val); });
    }

  private:
    // Array is in first cache-line along with read-only members.
    const std::size_t capacity_;
    const std::size_t mask_;
    std::unique_ptr<Entry[]> buf_;
    // Ensure that read and write positions are in different cache-lines.
    alignas(CacheLineSize) std::atomic<std::size_t> rpos_{0};
    alignas(CacheLineSize) std::atomic<std::size_t> wpos_{0};
};

} // namespace swirly

#endif // SWIRLY_SYS_BOUNDEDQUEUE_HPP
