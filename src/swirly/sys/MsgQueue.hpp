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
#ifndef SWIRLY_SYS_MSGQUEUE_HPP
#define SWIRLY_SYS_MSGQUEUE_HPP

#include <swirly/sys/BoundedQueue.hpp>
#include <swirly/sys/Event.hpp>
#include <swirly/sys/EventFile.hpp>

#include <swirly/Config.h>

namespace swirly {

class SWIRLY_API MsgQueue {
  public:
    MsgQueue(EventFile& ef, std::size_t capacity)
      : ef_(ef)
      , mq_{capacity}
    {
    }
    ~MsgQueue() noexcept;

    // Copy.
    MsgQueue(const MsgQueue& rhs) = delete;
    MsgQueue& operator=(const MsgQueue& rhs) = delete;

    // Move.
    MsgQueue(MsgQueue&&) = delete;
    MsgQueue& operator=(MsgQueue&&) = delete;

    /**
     * Returns the event file descriptor..
     */
    int fd() const noexcept { return ef_.fd(); }
    /**
     * Returns true if the queue is empty.
     */
    bool empty() const noexcept { return mq_.empty(); }
    /**
     * Returns true if the number of elements in the queue.
     */
    std::size_t size() const noexcept { return mq_.size(); }

    /**
     * Flush underlying event file.
     */
    void flush()
    {
        ef_.flush();
    }
    /**
     * Set state to parked if and only if the queue is empty. Returns false if the queue is not
     * empty.
     */
    bool park() noexcept
    {
        if (mq_.empty()) {
            // Sequential consistency prevents second empty() check from moving up across the
            // barrier and being optimised away.
            parked_.store(true, std::memory_order_seq_cst);
            if (mq_.empty()) {
                return true;
            }
            unpark();
        }
        return false;
    }
    /**
     * Set state to uparked.
     */
    void unpark() noexcept { parked_.store(false, std::memory_order_release); }

    /**
     * Returns false if queue is empty.
     */
    template <typename FnT>
    bool fetch(FnT fn) noexcept
    {
        return mq_.fetch(fn);
    }
    /**
     * Returns false if capacity is exceeded.
     */
    template <typename FnT>
    bool post(FnT fn)
    {
        if (!mq_.post(fn)) {
            return false;
        }
        if (parked_.load(std::memory_order_acquire)) {
            ef_.notify();
        }
        return true;
    }
    /**
     * Returns false if queue is empty.
     */
    bool pop(MsgEvent& ev) noexcept
    {
        return fetch([&ev](MsgEvent && ref) noexcept { ev = std::move(ref); });
    }
    /**
     * Returns false if capacity is exceeded.
     */
    bool push(const MsgEvent& ev) noexcept
    {
        return post([&ev](MsgEvent & ref) noexcept { ref = ev; });
    }
    /**
     * Returns false if capacity is exceeded.
     */
    bool push(MsgEvent&& ev) noexcept
    {
        return post([&ev](MsgEvent & ref) noexcept { ref = std::move(ev); });
    }

  private:
    EventFile& ef_;
    BoundedQueue<MsgEvent> mq_;
    std::atomic<bool> parked_{false};
};

} // namespace swirly

#endif // SWIRLY_SYS_MSGQUEUE_HPP
