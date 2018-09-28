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
#ifndef SWIRLY_SYS_TIMER_HPP
#define SWIRLY_SYS_TIMER_HPP

#include <swirly/util/Slot.hpp>
#include <swirly/util/Time.hpp>

#include <swirly/Config.h>

#include <boost/intrusive_ptr.hpp>

#include <memory>
#include <vector>

namespace swirly {
inline namespace sys {

class Timer;
class TimerQueue;
using TimerSlot = BasicSlot<CyclTime, Timer&>;

class SWIRLY_API Timer {
    friend class TimerQueue;

  public:
    struct Impl {
        union {
            /**
             * Singly-linked free-list.
             */
            Impl* next;
            TimerQueue* tq;
        };
        int ref_count;
        long id;
        WallTime expiry;
        Duration interval;
        TimerSlot slot;
    };

    explicit Timer(Impl* impl)
    : impl_{impl, false}
    {
    }
    Timer(std::nullptr_t = nullptr) noexcept {}
    ~Timer() = default;

    // Copy.
    Timer(const Timer&) = default;
    Timer& operator=(const Timer&) = default;

    // Move.
    Timer(Timer&&) noexcept = default;
    Timer& operator=(Timer&&) noexcept = default;

    bool empty() const noexcept { return !impl_; }
    explicit operator bool() const noexcept { return impl_ != nullptr; }
    long id() const noexcept { return impl_ ? impl_->id : 0; }
    bool pending() const noexcept { return impl_ != nullptr && bool{impl_->slot}; }

    WallTime expiry() const noexcept { return impl_->expiry; }
    Duration interval() const noexcept { return impl_->interval; }
    /**
     * Setting the interval will not reschedule any pending timer.
     */
    template <typename RepT, typename PeriodT>
    void set_interval(std::chrono::duration<RepT, PeriodT> interval) noexcept
    {
        using namespace std::chrono;
        impl_->interval = duration_cast<Duration>(interval);
    }
    void reset(std::nullptr_t = nullptr) noexcept { impl_.reset(); }
    void swap(Timer& rhs) noexcept { impl_.swap(rhs.impl_); }
    void cancel() noexcept;

  private:
    void set_expiry(WallTime expiry) noexcept { impl_->expiry = expiry; }
    TimerSlot& slot() noexcept { return impl_->slot; }

    boost::intrusive_ptr<Timer::Impl> impl_;
};

/**
 * Equal to.
 */
inline bool operator==(const Timer& lhs, const Timer& rhs) noexcept
{
    return lhs.id() == rhs.id();
}

/**
 * Not equal to.
 */
inline bool operator!=(const Timer& lhs, const Timer& rhs) noexcept
{
    return lhs.id() != rhs.id();
}

/**
 * Less than.
 */
inline bool operator<(const Timer& lhs, const Timer& rhs) noexcept
{
    return lhs.id() < rhs.id();
}

/**
 * Greater than.
 */
inline bool operator>(const Timer& lhs, const Timer& rhs) noexcept
{
    return lhs.id() > rhs.id();
}

/**
 * Less than or equal to.
 */
inline bool operator<=(const Timer& lhs, const Timer& rhs) noexcept
{
    return lhs.id() <= rhs.id();
}

/**
 * Greater than or equal to.
 */
inline bool operator>=(const Timer& lhs, const Timer& rhs) noexcept
{
    return lhs.id() >= rhs.id();
}

class SWIRLY_API TimerPool {
    using SlabPtr = std::unique_ptr<Timer::Impl[]>;

  public:
    TimerPool() = default;

    // Copy.
    TimerPool(const TimerPool&) = delete;
    TimerPool& operator=(const TimerPool&) = delete;

    // Move.
    TimerPool(TimerPool&&) = delete;
    TimerPool& operator=(TimerPool&&) = delete;

    Timer::Impl* alloc(WallTime expiry, Duration interval, TimerSlot slot);
    void dealloc(Timer::Impl* impl) noexcept
    {
        assert(impl);
        impl->next = free_;
        free_ = impl;
    }

  private:
    std::vector<SlabPtr> slabs_;
    /**
     * Head of free-list.
     */
    Timer::Impl* free_{nullptr};
    /**
     * Heap of timers ordered by expiry time.
     */
    std::vector<Timer> heap_;
};

class SWIRLY_API TimerQueue {
    friend class Timer;
    friend void intrusive_ptr_add_ref(Timer::Impl*) noexcept;
    friend void intrusive_ptr_release(Timer::Impl*) noexcept;

    using SlabPtr = std::unique_ptr<Timer::Impl[]>;

  public:
    /**
     * Implicit conversion from pool is allowed, so that TimerQueue arrays can be aggregate
     * initialised.
     */
    TimerQueue(TimerPool& pool)
    : pool_(pool)
    {
    }

    // Copy.
    TimerQueue(const TimerQueue&) = delete;
    TimerQueue& operator=(const TimerQueue&) = delete;

    // Move.
    TimerQueue(TimerQueue&&) = delete;
    TimerQueue& operator=(TimerQueue&&) = delete;

    std::size_t size() const noexcept { return heap_.size() - cancelled_; }
    bool empty() const noexcept { return size() == 0; }
    const Timer& front() const { return heap_.front(); }

    // clang-format off
    /**
     * Throws std::bad_alloc only.
     */
    [[nodiscard]] Timer insert(WallTime expiry, Duration interval, TimerSlot slot);
    /**
     * Throws std::bad_alloc only.
     */
    [[nodiscard]] Timer insert(WallTime expiry, TimerSlot slot)
    {
        return insert(expiry, Duration::zero(), slot);
    }
    // clang-format on

    int dispatch(CyclTime now);

  private:
    Timer alloc(WallTime expiry, Duration interval, TimerSlot slot);
    void cancel() noexcept;
    void expire(CyclTime now);
    void gc() noexcept;
    Timer pop() noexcept;

    TimerPool& pool_;
    long max_id_{};
    int cancelled_{};
    /**
     * Heap of timers ordered by expiry time.
     */
    std::vector<Timer> heap_;
};

inline void intrusive_ptr_add_ref(Timer::Impl* impl) noexcept
{
    ++impl->ref_count;
}

SWIRLY_API void intrusive_ptr_release(Timer::Impl* impl) noexcept;

inline void Timer::cancel() noexcept
{
    // If pending, then reset the slot and inform the queue that the timer has been cancelled.
    if (impl_->slot) {
        impl_->slot.reset();
        impl_->tq->cancel();
    }
}
} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_TIMER_HPP
