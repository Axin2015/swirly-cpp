/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2017 Swirly Cloud Limited.
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

#include <swirly/sys/Muxer.hpp>
#include <swirly/sys/RefCount.hpp>

namespace swirly {

class Reactor;

class SWIRLY_API EventHandler : public RefCount<EventHandler, ThreadUnsafePolicy> {
  public:
    explicit EventHandler(Reactor& reactor) noexcept : reactor_(reactor) {}
    virtual ~EventHandler() noexcept;

    // Copy.
    EventHandler(const EventHandler&) noexcept = delete;
    EventHandler& operator=(const EventHandler&) noexcept = delete;

    // Move.
    EventHandler(EventHandler&&) noexcept = delete;
    EventHandler& operator=(EventHandler&&) noexcept = delete;

    void ioEvent(int fd, EventMask events) { onIoEvent(fd, events); }

  protected:
    virtual void onIoEvent(int fd, EventMask events) = 0;

    const Reactor& reactor() const noexcept { return reactor_; }

    Reactor& reactor() noexcept { return reactor_; }

  private:
    Reactor& reactor_;
};

using EventHandlerPtr = boost::intrusive_ptr<EventHandler>;

class SWIRLY_API Reactor {
  public:
    enum : EventMask {
        In = Muxer::In,
        Pri = Muxer::Pri,
        Out = Muxer::Out,
        Err = Muxer::Err,
        Hup = Muxer::Hup
    };
    using Descriptor = typename Muxer::Descriptor;
    using Event = typename Muxer::Event;
    struct Data {
        int sid{};
        EventMask mask{};
        EventHandlerPtr eh;
    };
    explicit Reactor(std::size_t sizeHint) : mux_{sizeHint} { data_.resize(sizeHint); }
    ~Reactor() noexcept = default;

    // Copy.
    Reactor(const Reactor&) = default;
    Reactor& operator=(const Reactor&) = default;

    // Move.
    Reactor(Reactor&&) = default;
    Reactor& operator=(Reactor&&) = default;

    void swap(Reactor& rhs) noexcept { mux_.swap(rhs.mux_); }

    int poll() const;

    int poll(std::chrono::milliseconds timeout) const;

    void insert(int fd, EventMask mask, const EventHandlerPtr& eh);

    void update(int fd, EventMask mask);

    void erase(int fd);

  private:
    void dispatch(Event* events, int size) const;

    Muxer mux_;
    std::vector<Data> data_;
};

} // namespace swirly

#endif // SWIRLY_SYS_REACTOR_HPP
