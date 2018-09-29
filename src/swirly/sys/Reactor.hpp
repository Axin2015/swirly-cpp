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
#ifndef SWIRLY_SYS_REACTOR_HPP
#define SWIRLY_SYS_REACTOR_HPP

#include <swirly/sys/Timer.hpp>

namespace swirly {
inline namespace sys {

constexpr Duration NoTimeout{-1};
enum class Priority { High = 0, Low = 1 };

using IoSlot = BasicSlot<CyclTime, int, unsigned>;

class SWIRLY_API Reactor {
  public:
    class Handle {
      public:
        Handle(Reactor& reactor, int fd, int sid)
        : reactor_{&reactor}
        , fd_{fd}
        , sid_{sid}
        {
        }
        constexpr Handle(std::nullptr_t = nullptr) noexcept {}
        ~Handle() { reset(); }

        // Copy.
        Handle(const Handle&) = delete;
        Handle& operator=(const Handle&) = delete;

        // Move.
        Handle(Handle&& rhs) noexcept
        : reactor_{rhs.reactor_}
        , fd_{rhs.fd_}
        , sid_{rhs.sid_}
        {
            rhs.reactor_ = nullptr;
            rhs.fd_ = -1;
            rhs.sid_ = 0;
        }
        Handle& operator=(Handle&& rhs) noexcept
        {
            reset();
            swap(rhs);
            return *this;
        }
        bool empty() const noexcept { return reactor_ == nullptr; }
        explicit operator bool() const noexcept { return reactor_ != nullptr; }
        auto fd() const noexcept { return fd_; }
        auto sid() const noexcept { return sid_; }

        void reset(std::nullptr_t = nullptr) noexcept
        {
            if (reactor_) {
                reactor_->do_unsubscribe(fd_, sid_);
                reactor_ = nullptr;
                fd_ = -1;
                sid_ = 0;
            }
        }
        void swap(Handle& rhs) noexcept
        {
            std::swap(reactor_, rhs.reactor_);
            std::swap(fd_, rhs.fd_);
            std::swap(sid_, rhs.sid_);
        }

        /**
         * Modify IO event subscription.
         */
        void set_events(unsigned events, IoSlot slot)
        {
            assert(reactor_);
            reactor_->do_set_events(fd_, sid_, events, slot);
        }
        void set_events(unsigned events)
        {
            assert(reactor_);
            reactor_->do_set_events(fd_, sid_, events);
        }

      private:
        Reactor* reactor_{nullptr};
        int fd_{-1}, sid_{0};
    };

    Reactor() noexcept = default;
    virtual ~Reactor();

    // Copy.
    Reactor(const Reactor&) = delete;
    Reactor& operator=(const Reactor&) = delete;

    // Move.
    Reactor(Reactor&&) = delete;
    Reactor& operator=(Reactor&&) = delete;

    void interrupt() noexcept { do_interrupt(); }
    // clang-format off
    [[nodiscard]] Handle subscribe(int fd, unsigned events, IoSlot slot)
    {
        return do_subscribe(fd, events, slot);
    }
    /**
     * Throws std::bad_alloc only.
     */
    [[nodiscard]] Timer timer(MonoTime expiry, Duration interval, Priority priority, TimerSlot slot)
    {
        return do_timer(expiry, interval, priority, slot);
    }
    /**
     * Throws std::bad_alloc only.
     */
    [[nodiscard]] Timer timer(MonoTime expiry, Priority priority, TimerSlot slot)
    {
        return do_timer(expiry, priority, slot);
    }
    // clang-format on
    int poll(CyclTime now, Duration timeout = NoTimeout)
    {
        return do_poll(now, timeout);
    }

  protected:
    virtual void do_interrupt() noexcept = 0;

    virtual Handle do_subscribe(int fd, unsigned events, IoSlot slot) = 0;
    virtual void do_unsubscribe(int fd, int sid) noexcept = 0;

    virtual void do_set_events(int fd, int sid, unsigned events, IoSlot slot) = 0;
    virtual void do_set_events(int fd, int sid, unsigned events) = 0;

    /**
     * Throws std::bad_alloc only.
     */
    virtual Timer do_timer(MonoTime expiry, Duration interval, Priority priority, TimerSlot slot)
        = 0;
    /**
     * Throws std::bad_alloc only.
     */
    virtual Timer do_timer(MonoTime expiry, Priority priority, TimerSlot slot) = 0;

    virtual int do_poll(CyclTime now, Duration timeout) = 0;
};

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_REACTOR_HPP
