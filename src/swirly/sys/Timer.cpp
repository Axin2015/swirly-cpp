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

#include "Log.hpp"

#include <algorithm>
#include <string>

namespace swirly {

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

Timer TimerQueue::insert(Time expiry, Duration interval, const ActorPtr& actor)
{
    assert(actor);

    heap_.reserve(heap_.size() + 1);
    const auto tmr = alloc(expiry, interval, actor);

    // Cannot fail.
    heap_.push_back(tmr);
    push_heap(heap_.begin(), heap_.end(), isAfter);

    return tmr;
}

int TimerQueue::dispatch(Time now)
{
    int n{};
    while (!heap_.empty()) {

        if (heap_.front().cancelled()) {

            // Pop cancelled timer from front.
            pop_heap(heap_.begin(), heap_.end(), isAfter);
            heap_.pop_back();
            --cancelled_;

        } else if (heap_.front().expiry() <= now) {

            expire(now);
            ++n;

        } else {
            break;
        }
    }
    return n;
}

Timer TimerQueue::alloc(Time expiry, Duration interval, const ActorPtr& actor)
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
    impl->actor = actor;

    return Timer{impl};
}

void TimerQueue::expire(Time now)
{
    // Remove timer.
    auto tmr = heap_.front();
    pop_heap(heap_.begin(), heap_.end(), isAfter);
    heap_.pop_back();

    try {
        // Notify user.
        tmr.actor()->onTimer(tmr, now);
    } catch (const std::exception& e) {
        using namespace string_literals;
        SWIRLY_ERROR("error handling timer event: "s + e.what());
    }

    if (tmr.cancelled()) {

        // The timer was cancelled during the callback.
        --cancelled_;

    } else if (tmr.interval().count() > 0) {

        // Add interval to expiry, while ensuring that next expiry is always in the future.
        tmr.setExpiry(max(tmr.expiry() + tmr.interval(), now + Nanos{1}));

        // Reschedule popped timer.
        heap_.push_back(tmr);
        push_heap(heap_.begin(), heap_.end(), isAfter);

    } else {

        // Free non-repeating timer.
        tmr.actor().reset();
    }

    if (cancelled_ > static_cast<int>(heap_.size() >> 2)) {
        // More than half of the timers have been cancelled, so purge them now.
        purge();
    }
}

void TimerQueue::purge() noexcept
{
    const auto it
        = remove_if(heap_.begin(), heap_.end(), [](const auto& tmr) { return tmr.cancelled(); });
    heap_.erase(it, heap_.end());
    cancelled_ = 0;
}

} // namespace swirly
