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
#include <swirly/sys/Handle.hpp>
#include <swirly/sys/Muxer.hpp>
#include <swirly/sys/Timer.hpp>

namespace swirly {

struct TokenPolicy {
    struct Id {
        Reactor* reactor{nullptr};
        int value{-1};
    };
    static constexpr Id invalid() noexcept { return {}; }
    static void close(Id id) noexcept;
};

inline bool operator==(TokenPolicy::Id lhs, TokenPolicy::Id rhs) noexcept
{
    assert(lhs.reactor == rhs.reactor || !lhs.reactor || !rhs.reactor);
    return lhs.value == rhs.value;
}

inline bool operator!=(TokenPolicy::Id lhs, TokenPolicy::Id rhs) noexcept
{
    assert(lhs.reactor == rhs.reactor || !lhs.reactor || !rhs.reactor);
    return lhs.value != rhs.value;
}

using Token = Handle<TokenPolicy>;

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

    Token attach(int fd, EventMask mask, const AsyncHandlerPtr& handler);
    void mask(int fd, EventMask mask);
    void detach(int fd) noexcept;

    Timer timer(Time expiry, Duration interval, const AsyncHandlerPtr& handler);
    Timer timer(Time expiry, const AsyncHandlerPtr& handler);

    int poll(std::chrono::milliseconds timeout = std::chrono::milliseconds::max());

  private:
    int dispatch(Event* events, int size, Time now);

    Muxer mux_;
    TimerQueue tq_;
    std::vector<Data> data_;
};

inline void TokenPolicy::close(Id id) noexcept
{
    id.reactor->detach(id.value);
}

} // namespace swirly

#endif // SWIRLY_SYS_REACTOR_HPP
