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

enum class Priority { High = 0, Low = 1 };

using IoSlot = BasicSlot<int, unsigned, Time>;

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
                reactor_->doUnsubscribe(fd_, sid_);
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

        // Modify IO event subscription.
        void setEvents(unsigned events, IoSlot slot)
        {
            reactor_->doSetEvents(fd_, sid_, events, slot);
        }
        void setEvents(unsigned events) { reactor_->doSetEvents(fd_, sid_, events); }

      private:
        Reactor* reactor_{nullptr};
        int fd_{-1}, sid_{0};
    };

    Reactor() noexcept = default;
    virtual ~Reactor();

    // Copy.
    Reactor(const Reactor&) noexcept = delete;
    Reactor& operator=(const Reactor&) noexcept = delete;

    // Move.
    Reactor(Reactor&&) noexcept = delete;
    Reactor& operator=(Reactor&&) noexcept = delete;

    void interrupt() noexcept { doInterrupt(); }
    // clang-format off
    [[nodiscard]] Handle subscribe(int fd, unsigned events, IoSlot slot)
    {
        return doSubscribe(fd, events, slot);
    }
    [[nodiscard]] Timer timer(Time expiry, Duration interval, Priority priority, TimerSlot slot)
    {
        return doTimer(expiry, interval, priority, slot);
    }
    [[nodiscard]] Timer timer(Time expiry, Priority priority, TimerSlot slot)
    {
        return doTimer(expiry, priority, slot);
    }
    // clang-format on
    int poll(Millis timeout = Millis::max())
    {
        return doPoll(UnixClock::now(), timeout);
    }

  protected:
    /**
     * Overload for unit-testing.
     */
    int poll(Time now, Millis timeout) { return doPoll(now, timeout); }

    virtual void doInterrupt() noexcept = 0;

    virtual Handle doSubscribe(int fd, unsigned events, IoSlot slot) = 0;
    virtual void doUnsubscribe(int fd, int sid) noexcept = 0;

    virtual void doSetEvents(int fd, int sid, unsigned events, IoSlot slot) = 0;
    virtual void doSetEvents(int fd, int sid, unsigned events) = 0;

    virtual Timer doTimer(Time expiry, Duration interval, Priority priority, TimerSlot slot) = 0;
    virtual Timer doTimer(Time expiry, Priority priority, TimerSlot slot) = 0;

    virtual int doPoll(Time now, Millis timeout) = 0;
};

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_REACTOR_HPP
