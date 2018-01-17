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

struct FileTokenPolicy {
    struct Id {
        Reactor* reactor{nullptr};
        int value{-1};
    };
    static constexpr Id invalid() noexcept { return {}; }
    static void close(Id id) noexcept;
};

inline bool operator==(FileTokenPolicy::Id lhs, FileTokenPolicy::Id rhs) noexcept
{
    assert(lhs.reactor == rhs.reactor || !lhs.reactor || !rhs.reactor);
    return lhs.value == rhs.value;
}

inline bool operator!=(FileTokenPolicy::Id lhs, FileTokenPolicy::Id rhs) noexcept
{
    assert(lhs.reactor == rhs.reactor || !lhs.reactor || !rhs.reactor);
    return lhs.value != rhs.value;
}

using FileToken = Handle<FileTokenPolicy>;

struct EventTokenPolicy {
    struct Id {
        Reactor* reactor{nullptr};
        Address addr{Address::None};
        EventHandler* handler{nullptr};
    };
    static constexpr Id invalid() noexcept { return {}; }
    static void close(Id id) noexcept;
};

inline bool operator==(EventTokenPolicy::Id lhs, EventTokenPolicy::Id rhs) noexcept
{
    assert(lhs.reactor == rhs.reactor || !lhs.reactor || !rhs.reactor);
    return lhs.addr == rhs.addr && lhs.handler == rhs.handler;
}

inline bool operator!=(EventTokenPolicy::Id lhs, EventTokenPolicy::Id rhs) noexcept
{
    assert(lhs.reactor == rhs.reactor || !lhs.reactor || !rhs.reactor);
    return lhs.addr != rhs.addr || lhs.handler != rhs.handler;
}

using EventToken = Handle<EventTokenPolicy>;

class SWIRLY_API Reactor {
  public:
    enum : FileEvents {
        In = Muxer::In,
        Pri = Muxer::Pri,
        Out = Muxer::Out,
        Err = Muxer::Err,
        Hup = Muxer::Hup
    };
    using Id = typename Muxer::Id;
    using FileEvent = typename Muxer::FileEvent;
    struct Data {
        int sid{};
        FileEvents mask{};
        EventHandlerPtr handler;
    };
    explicit Reactor(std::size_t sizeHint = 1024);
    ~Reactor() noexcept;

    // Copy.
    Reactor(const Reactor&) = delete;
    Reactor& operator=(const Reactor&) = delete;

    // Move.
    Reactor(Reactor&&);
    Reactor& operator=(Reactor&&);

    void swap(Reactor& rhs) noexcept { impl_.swap(rhs.impl_); }

    /**
     * Thread-safe.
     */
    void postEvent(const Event& ev);

    /**
     * Thread-safe.
     */
    void postEvent(Event&& ev);

    FileToken subscribe(int fd, FileEvents mask, const EventHandlerPtr& handler);
    EventToken subscribe(Address addr, const EventHandlerPtr& handler);

    void unsubscribe(int fd) noexcept;
    void unsubscribe(Address addr, const EventHandler& handler) noexcept;

    void setMask(int fd, FileEvents mask);

    Timer setTimer(Time expiry, Duration interval, const EventHandlerPtr& handler);
    Timer setTimer(Time expiry, const EventHandlerPtr& handler);

    int poll(std::chrono::milliseconds timeout = std::chrono::milliseconds::max());

  private:
    int dispatch(FileEvent* buf, int size, Time now);

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

inline void FileTokenPolicy::close(Id id) noexcept
{
    id.reactor->unsubscribe(id.value);
}

inline void EventTokenPolicy::close(Id id) noexcept
{
    id.reactor->unsubscribe(id.addr, *id.handler);
}

/**
 * Set the Reactors vector used by postEvent(). This vector must not be modified once set.
 */
SWIRLY_API void setReactors(std::vector<Reactor>& rs) noexcept;

/**
 * Post Event to each Reactor in the Reactors vector.
 */
SWIRLY_API void postEvent(const Event& ev);

} // namespace swirly

#endif // SWIRLY_SYS_REACTOR_HPP
