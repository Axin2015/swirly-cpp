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
#include "Timer.hpp"

#include <swirly/util/Log.hpp>

#include <algorithm>
#include <string>

namespace swirly {
inline namespace sys {
using namespace std;
namespace {
// Number of entries per 4K slab, assuming that malloc overhead is no more than 16 bytes.
constexpr size_t Overhead = 16;
constexpr size_t PageSize = 4096;
constexpr size_t SlabSize = (PageSize - Overhead) / sizeof(Timer);

bool isAfter(const Timer& lhs, const Timer& rhs)
{
    return lhs.expiry() > rhs.expiry();
}

} // namespace

Timer TimerQueue::insert(Time expiry, Duration interval, TimerSlot slot)
{
    assert(slot);

    heap_.reserve(heap_.size() + 1);
    const auto tmr = alloc(expiry, interval, slot);

    // Cannot fail.
    heap_.push_back(tmr);
    push_heap(heap_.begin(), heap_.end(), isAfter);

    return tmr;
}

int TimerQueue::dispatch(Time now)
{
    int n{};
    while (!heap_.empty()) {

        // If not pending, then must have been cancelled.
        if (!heap_.front().pending()) {
            pop();
            --cancelled_;
        } else if (heap_.front().expiry() <= now) {
            expire(now);
            ++n;
        } else {
            break;
        }
    }

    // Garbage collect if more than half of the timers have been cancelled.
    if (cancelled_ > static_cast<int>(heap_.size() >> 2)) {
        gc();
    }
    return n;
}

Timer TimerQueue::alloc(Time expiry, Duration interval, TimerSlot slot)
{
    Timer::Impl* impl;

    if (free_) {

        // Pop next free timer from stack.
        impl = free_;
        free_ = free_->next;

    } else {

        // Add new slab of timers to stack.
        SlabPtr slab{new Timer::Impl[SlabSize]};
        impl = &slab[0];

        for (size_t i{1}; i < SlabSize; ++i) {
            slab[i].next = free_;
            free_ = &slab[i];
        }
        slabs_.push_back(move(slab));
    }

    impl->tq = this;
    impl->refCount = 1;
    impl->id = ++maxId_;
    impl->expiry = expiry;
    impl->interval = interval;
    impl->slot = slot;

    return Timer{impl};
}

void TimerQueue::cancel() noexcept
{
    ++cancelled_;

    // Ensure that a pending timer is at the front of the queue.
    // If not pending, then must have been cancelled.
    while (!heap_.empty() && !heap_.front().pending()) {
        pop();
        --cancelled_;
    }
}

void TimerQueue::expire(Time now)
{
    // Pop timer.
    auto tmr = pop();
    assert(tmr.pending());
    try {
        // Notify user.
        tmr.slot().invoke(tmr, now);
    } catch (const std::exception& e) {
        using namespace string_literals;
        SWIRLY_ERROR << "error handling timer event: "sv << e.what();
    }

    if (!tmr.pending()) {

        // The timer was cancelled during the callback.
        --cancelled_;

    } else if (tmr.interval().count() > 0) {

        // Add interval to expiry, while ensuring that next expiry is always in the future.
        tmr.setExpiry(max(tmr.expiry() + tmr.interval(), now + 1ns));

        // Reschedule popped timer.
        heap_.push_back(tmr);
        push_heap(heap_.begin(), heap_.end(), isAfter);

    } else {

        // Free handler for non-repeating timer.
        tmr.slot().reset();
    }
}

void TimerQueue::gc() noexcept
{
    const auto it
        = remove_if(heap_.begin(), heap_.end(), [](const auto& tmr) { return !tmr.pending(); });
    heap_.erase(it, heap_.end());
    make_heap(heap_.begin(), heap_.end(), isAfter);
    cancelled_ = 0;
}

Timer TimerQueue::pop() noexcept
{
    auto tmr = heap_.front();
    pop_heap(heap_.begin(), heap_.end(), isAfter);
    heap_.pop_back();
    return tmr;
}

void intrusive_ptr_release(Timer::Impl* impl) noexcept
{
    --impl->refCount;
    if (impl->refCount == 1) {
        // Cancel pending if only one reference remains. If only one reference remains after
        // decrementing the counter, and the timer is still pending, then the final reference must
        // be held within the internal timer queue, which means that no more references exist
        // outside of the timer queue.
        if (impl->slot) {
            impl->slot.reset();
            impl->tq->cancel();
        }
    } else if (impl->refCount == 0) {
        auto& tq = *impl->tq;
        impl->next = tq.free_;
        tq.free_ = impl;
    }
}

} // namespace sys
} // namespace swirly
