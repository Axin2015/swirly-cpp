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
#include <swirly/sys/Muxer.hpp>
#include <swirly/sys/Timer.hpp>

namespace swirly {

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

using SubHandle = Handle<SubPolicy>;

class SWIRLY_API Reactor {
  public:
    Reactor() noexcept = default;
    virtual ~Reactor() noexcept;

    // Copy.
    Reactor(const Reactor&) noexcept = default;
    Reactor& operator=(const Reactor&) noexcept = default;

    // Move.
    Reactor(Reactor&&) noexcept = default;
    Reactor& operator=(Reactor&&) noexcept = default;

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
    Timer setTimer(Time expiry, Duration interval, Priority priority,
                   const EventHandlerPtr& handler)
    {
        return doSetTimer(expiry, interval, priority, handler);
    }
    Timer setTimer(Time expiry, Priority priority, const EventHandlerPtr& handler)
    {
        return doSetTimer(expiry, priority, handler);
    }
    int poll(std::chrono::milliseconds timeout = std::chrono::milliseconds::max())
    {
        return doPoll(timeout);
    }

  protected:
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

    virtual Timer doSetTimer(Time expiry, Duration interval, Priority priority,
                             const EventHandlerPtr& handler)
        = 0;
    virtual Timer doSetTimer(Time expiry, Priority priority, const EventHandlerPtr& handler) = 0;

    virtual int doPoll(std::chrono::milliseconds timeout) = 0;
};

inline void SubPolicy::close(Id id) noexcept
{
    id.reactor->unsubscribe(id.value);
}

class SWIRLY_API EpollReactor : public Reactor {
  public:
    using Event = typename EpollMuxer::Event;

    explicit EpollReactor(std::size_t sizeHint = 1024);
    ~EpollReactor() noexcept override;

    // Copy.
    EpollReactor(const EpollReactor&) = delete;
    EpollReactor& operator=(const EpollReactor&) = delete;

    // Move.
    EpollReactor(EpollReactor&&) = delete;
    EpollReactor& operator=(EpollReactor&&) = delete;

  protected:
    /**
     * Thread-safe.
     */
    bool doClosed() const noexcept override;

    /**
     * Thread-safe.
     */
    void doClose() noexcept override;

    SubHandle doSubscribe(int fd, unsigned events, const EventHandlerPtr& handler) override;
    void doUnsubscribe(int fd) noexcept override;

    void doSetEvents(int fd, unsigned events) override;

    Timer doSetTimer(Time expiry, Duration interval, Priority priority,
                     const EventHandlerPtr& handler) override;
    Timer doSetTimer(Time expiry, Priority priority, const EventHandlerPtr& handler) override;

    int doPoll(std::chrono::milliseconds timeout) override;

  private:
    int dispatch(Event* buf, int size, Time now);

    struct Data {
        int sid{};
        unsigned events{};
        EventHandlerPtr handler;
    };
    EpollMuxer mux_;
    std::vector<Data> data_;
    EventFd efd_;
    static_assert(static_cast<int>(Priority::High) == 0);
    static_assert(static_cast<int>(Priority::Low) == 1);
    std::array<TimerQueue, 2> tqs_;
    std::atomic<bool> closed_{false};
};

} // namespace swirly

#endif // SWIRLY_SYS_REACTOR_HPP
