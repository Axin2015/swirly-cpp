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

#include <swirly/sys/EventHandler.hpp>
#include <swirly/sys/Time.hpp>

#include <swirly/Config.h>

#include <memory>
#include <vector>

namespace swirly {
inline namespace sys {

class TimerQueue;

class SWIRLY_API Timer {
    friend class TimerQueue;

  public:
    struct Impl {
        union {
            // Singly-linked free-list.
            Impl* next;
            TimerQueue* tq;
        };
        int refCount;
        long id;
        Time expiry;
        Duration interval;
        EventHandlerPtr handler;
    };
    Timer() = default;
    explicit Timer(Impl* impl)
    : impl_{impl, false}
    {
    }
    ~Timer() { reset(); }
    long id() const noexcept { return impl_->id; }
    Time expiry() const noexcept { return impl_->expiry; }
    Duration interval() const noexcept { return impl_->interval; }
    bool pending() const noexcept { return bool{impl_->handler}; }
    // Setting the interval will not reschedule any pending timer.
    void setInterval(Duration interval) noexcept { impl_->interval = interval; }
    void reset() noexcept;
    void cancel() noexcept;

  private:
    void setExpiry(Time expiry) noexcept { impl_->expiry = expiry; }
    EventHandlerPtr& handler() noexcept { return impl_->handler; }
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

class SWIRLY_API TimerQueue {
    friend class Timer;
    friend void intrusive_ptr_add_ref(Timer::Impl*) noexcept;
    friend void intrusive_ptr_release(Timer::Impl*) noexcept;

    using SlabPtr = std::unique_ptr<Timer::Impl[]>;

  public:
    TimerQueue() = default;

    // Copy.
    TimerQueue(const TimerQueue&) = delete;
    TimerQueue& operator=(const TimerQueue&) = delete;

    // Move.
    TimerQueue(TimerQueue&&) = default;
    TimerQueue& operator=(TimerQueue&&) = default;

    std::size_t size() const noexcept { return heap_.size() - cancelled_; }
    bool empty() const noexcept { return size() == 0; }
    const Timer& front() const { return heap_.front(); }

    [[nodiscard]] Timer insert(Time expiry, Duration interval, const EventHandlerPtr& handler);
    [[nodiscard]] Timer insert(Time expiry, const EventHandlerPtr& handler)
    {
        return insert(expiry, Duration::zero(), handler);
    }

    int dispatch(Time now);

  private:
    Timer alloc(Time expiry, Duration interval, const EventHandlerPtr& handler);
    void cancel() noexcept;
    void expire(Time now);
    void gc() noexcept;
    Timer pop() noexcept;

    long maxId_{};
    int cancelled_{};
    std::vector<SlabPtr> slabs_;
    // Head of free-list.
    Timer::Impl* free_{nullptr};
    // Heap of timers ordered by expiry time.
    std::vector<Timer> heap_;
};

inline void intrusive_ptr_add_ref(Timer::Impl* impl) noexcept
{
    ++impl->refCount;
}

inline void intrusive_ptr_release(Timer::Impl* impl) noexcept
{
    if (--impl->refCount == 0) {
        auto& tq = *impl->tq;
        impl->next = tq.free_;
        tq.free_ = impl;
    }
}

inline void Timer::reset() noexcept
{
    if (impl_) {
        // If pending, and if only two references to the timer remain (this one and the one in the
        // queue), then reset the handler and inform the queue that the timer has been cancelled.
        if (impl_->handler && impl_->refCount == 2) {
            impl_->handler.reset();
            impl_->tq->cancel();
        }
        impl_.reset();
    }
}

inline void Timer::cancel() noexcept
{
    // If pending, then reset the handler and inform the queue that the timer has been cancelled.
    if (impl_->handler) {
        impl_->handler.reset();
        impl_->tq->cancel();
    }
}
} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_TIMER_HPP
