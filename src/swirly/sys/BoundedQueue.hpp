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

#include <swirly/sys/Memory.hpp>

#include <atomic>
#include <cassert>
#include <memory>

namespace swirly {

// Thanks to Dmitry Vyukov for this algorithm.
template <typename ValueT>
class BoundedQueue {
    static_assert(std::is_trivially_copyable_v<ValueT>);
    struct alignas(CacheLineSize) Entry {
        std::int64_t seq;
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
        for (std::int64_t i{0}; i < static_cast<std::int64_t>(capacity); ++i) {
            __atomic_store_n(&buf_[i].seq, i, __ATOMIC_RELAXED);
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
        const auto rpos = __atomic_load_n(&rpos_, __ATOMIC_ACQUIRE);
        const auto wpos = __atomic_load_n(&wpos_, __ATOMIC_RELAXED);
        return rpos == wpos;
    }
    /**
     * Returns true if the number of elements in the queue.
     */
    std::size_t size() const noexcept
    {
        // Acquire prevents reordering of these loads.
        const auto rpos = __atomic_load_n(&rpos_, __ATOMIC_ACQUIRE);
        const auto wpos = __atomic_load_n(&wpos_, __ATOMIC_RELAXED);
        return wpos - rpos;
    }
    /**
     * Returns false if queue is empty.
     */
    template <typename FnT>
    bool fetch(FnT fn) noexcept
    {
        static_assert(std::is_nothrow_invocable_v<FnT, ValueT&&>);
        auto rpos = __atomic_load_n(&rpos_, __ATOMIC_RELAXED);
        for (;;) {
            auto& entry = buf_[rpos & mask_];
            const auto seq = __atomic_load_n(&entry.seq, __ATOMIC_ACQUIRE);
            const auto diff = seq - (rpos + 1);
            if (diff == 0) {
                // The compare_exchange_weak function re-reads wpos on failure.
                if (__atomic_compare_exchange_n(&rpos_, &rpos, rpos + 1, true, __ATOMIC_RELAXED,
                                                __ATOMIC_RELAXED)) {
                    fn(std::move(entry.val));
                    // Commit.
                    __atomic_store_n(&entry.seq, rpos + capacity_, __ATOMIC_RELEASE);
                    break;
                }
            } else if (diff < 0) {
                return false;
            } else {
                rpos = __atomic_load_n(&rpos_, __ATOMIC_RELAXED);
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
        auto wpos = __atomic_load_n(&wpos_, __ATOMIC_RELAXED);
        for (;;) {
            auto& entry = buf_[wpos & mask_];
            const auto seq = __atomic_load_n(&entry.seq, __ATOMIC_ACQUIRE);
            const auto diff = seq - wpos;
            if (diff == 0) {
                // The compare_exchange_weak function re-reads wpos on failure.
                if (__atomic_compare_exchange_n(&wpos_, &wpos, wpos + 1, true, __ATOMIC_RELAXED,
                                                __ATOMIC_RELAXED)) {
                    fn(entry.val);
                    // Commit.
                    __atomic_store_n(&entry.seq, wpos + 1, __ATOMIC_RELEASE);
                    break;
                }
            } else if (diff < 0) {
                return false;
            } else {
                wpos = __atomic_load_n(&wpos_, __ATOMIC_RELAXED);
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
    // Ensure that read and write positions are in different cache-lines.
    alignas(CacheLineSize) std::int64_t rpos_{0};
    alignas(CacheLineSize) std::int64_t wpos_{0};
    std::unique_ptr<Entry[]> buf_;
};

} // namespace swirly

#endif // SWIRLY_SYS_BOUNDEDQUEUE_HPP
