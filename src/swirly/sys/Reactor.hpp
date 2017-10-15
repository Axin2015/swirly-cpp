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
        explicit Data(long modcnt = 0, EventMask mask = 0) noexcept
            : modcnt{modcnt}, mask{mask}
        {
        }
        Data(long modcnt, EventMask mask, const EventHandlerPtr& eh) noexcept
            : modcnt{modcnt}, mask{mask}, eh{eh}
        {
        }
        long modcnt;
        EventMask mask;
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

    void insert(int fd, EventMask mask, const EventHandlerPtr& eh)
    {
        if (fd >= static_cast<int>(data_.size())) {
            data_.resize(fd + 1);
        }
        mux_.insert(fd, mask);
        data_[fd] = Data{++modcnt_, mask, eh};
    }
    void update(int fd, EventMask mask)
    {
        if (data_[fd].mask != mask) {
            mux_.update(fd, mask);
        }
    }
    void erase(int fd)
    {
        mux_.erase(fd);
        data_[fd] = Data{++modcnt_};
    }
    int poll()
    {
        // Record the current modification count immediately prior to the wait() call.
        const auto modcnt = modcnt_;

        Event events[16];
        const auto size = mux_.wait(events, 16);
        dispatch(modcnt, events, size);
        return size;
    }
    int poll(std::chrono::milliseconds timeout)
    {
        // Record the current modification count immediately prior to the wait() call.
        const auto modcnt = modcnt_;

        Event events[16];
        const auto size = mux_.wait(events, 16, timeout);
        dispatch(modcnt, events, size);
        return size;
    }

  private:
    void dispatch(long modcnt, Event* events, int size) const;

    Muxer mux_;
    // The modification counter is incremented whenever a socket is added or removed.
    long modcnt_{};
    std::vector<Data> data_;
};

} // namespace swirly

#endif // SWIRLY_SYS_REACTOR_HPP
