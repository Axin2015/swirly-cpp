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

#include <swirly/sys/AsyncHandler.hpp>
#include <swirly/sys/Muxer.hpp>
#include <swirly/sys/Timer.hpp>

namespace swirly {

class SWIRLY_API Reactor {
  public:
    enum : EventMask {
        In = Muxer::In,
        Pri = Muxer::Pri,
        Out = Muxer::Out,
        Err = Muxer::Err,
        Hup = Muxer::Hup
    };
    using Id = typename Muxer::Id;
    using Event = typename Muxer::Event;
    struct Data {
        int sid{};
        EventMask mask{};
        AsyncHandlerPtr handler;
    };
    explicit Reactor(std::size_t sizeHint) : mux_{sizeHint} { data_.resize(sizeHint); }
    ~Reactor() noexcept = default;

    // Copy.
    Reactor(const Reactor&) = delete;
    Reactor& operator=(const Reactor&) = delete;

    // Move.
    Reactor(Reactor&&) = default;
    Reactor& operator=(Reactor&&) = default;

    void swap(Reactor& rhs) noexcept { mux_.swap(rhs.mux_); }

    void attach(int fd, EventMask mask, const AsyncHandlerPtr& handler);
    void setMask(int fd, EventMask mask);
    void detach(int fd);

    Timer setTimer(Time expiry, Duration interval, const AsyncHandlerPtr& handler);
    Timer setTimer(Time expiry, const AsyncHandlerPtr& handler);

    bool resetTimer(long id, Duration interval);
    bool resetTimer(Timer::Id id, Duration interval);

    void cancelTimer(long id);
    void cancelTimer(Timer::Id id);

    int poll(std::chrono::milliseconds timeout = std::chrono::milliseconds::max());

  private:
    void dispatch(Event* events, int size, Time now);

    Muxer mux_;
    TimerQueue tq_;
    std::vector<Data> data_;
};

} // namespace swirly

#endif // SWIRLY_SYS_REACTOR_HPP
