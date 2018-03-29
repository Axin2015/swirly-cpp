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

#include <swirly/sys/EventHandler.hpp>
#include <swirly/sys/Handle.hpp>
#include <swirly/sys/Timer.hpp>

namespace swirly {
inline namespace sys {

enum class Priority { High = 0, Low = 1 };

struct SubPolicy {
    struct Id {
        Reactor* reactor{nullptr};
        int value{-1};
    };
    static constexpr Id invalid() noexcept { return {}; }
    static void close(Id id) noexcept;
};

inline bool operator==(SubPolicy::Id lhs, SubPolicy::Id rhs) noexcept
{
    assert(lhs.reactor == rhs.reactor || !lhs.reactor || !rhs.reactor);
    return lhs.value == rhs.value;
}

inline bool operator!=(SubPolicy::Id lhs, SubPolicy::Id rhs) noexcept
{
    assert(lhs.reactor == rhs.reactor || !lhs.reactor || !rhs.reactor);
    return lhs.value != rhs.value;
}

using SubHandle = BasicHandle<SubPolicy>;

class SWIRLY_API Reactor {
  public:
    Reactor() noexcept = default;
    virtual ~Reactor() noexcept;

    // Copy.
    Reactor(const Reactor&) noexcept = delete;
    Reactor& operator=(const Reactor&) noexcept = delete;

    // Move.
    Reactor(Reactor&&) noexcept = delete;
    Reactor& operator=(Reactor&&) noexcept = delete;

    bool closed() const noexcept { return doClosed(); }

    /**
     * Thread-safe.
     */
    void close() noexcept { doClose(); }
    SubHandle subscribe(int fd, unsigned events, const EventHandlerPtr& handler)
    {
        return doSubscribe(fd, events, handler);
    }
    void unsubscribe(int fd) noexcept { doUnsubscribe(fd); }
    void setEvents(int fd, unsigned events) { doSetEvents(fd, events); }
    Timer timer(Time expiry, Duration interval, Priority priority, const EventHandlerPtr& handler)
    {
        return doTimer(expiry, interval, priority, handler);
    }
    Timer timer(Time expiry, Priority priority, const EventHandlerPtr& handler)
    {
        return doTimer(expiry, priority, handler);
    }
    int poll(std::chrono::milliseconds timeout = std::chrono::milliseconds::max())
    {
        return doPoll(UnixClock::now(), timeout);
    }

  protected:
    /**
     * Overload for unit-testing.
     */
    int poll(Time now, Millis timeout) { return doPoll(now, timeout); }

    /**
     * Thread-safe.
     */
    virtual bool doClosed() const noexcept = 0;

    /**
     * Thread-safe.
     */
    virtual void doClose() noexcept = 0;

    virtual SubHandle doSubscribe(int fd, unsigned events, const EventHandlerPtr& handler) = 0;
    virtual void doUnsubscribe(int fd) noexcept = 0;

    virtual void doSetEvents(int fd, unsigned events) = 0;

    virtual Timer doTimer(Time expiry, Duration interval, Priority priority,
                          const EventHandlerPtr& handler)
        = 0;
    virtual Timer doTimer(Time expiry, Priority priority, const EventHandlerPtr& handler) = 0;

    virtual int doPoll(Time now, std::chrono::milliseconds timeout) = 0;
};

inline void SubPolicy::close(Id id) noexcept
{
    id.reactor->unsubscribe(id.value);
}

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_REACTOR_HPP
