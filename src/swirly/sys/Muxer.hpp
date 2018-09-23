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
#ifndef SWIRLY_SYS_MUXER_HPP
#define SWIRLY_SYS_MUXER_HPP

#include <swirly/sys/Epoll.hpp>
#include <swirly/sys/Event.hpp>
#include <swirly/sys/TimerFd.hpp>

#include <swirly/Config.h>

#include <unistd.h>

namespace swirly {
inline namespace sys {

class EpollMuxer {
  public:
    using Event = epoll_event;

    static constexpr int fd(const Event& ev) noexcept
    {
        return static_cast<int>(ev.data.u64 & 0xffffffff);
    }
    static constexpr int sid(const Event& ev) noexcept
    {
        return static_cast<int>(ev.data.u64 >> 32);
    }
    static constexpr unsigned events(const Event& ev) noexcept
    {
        unsigned n{};
        if (ev.events & EPOLLIN) {
            n |= EventIn;
        }
        if (ev.events & EPOLLPRI) {
            n |= EventPri;
        }
        if (ev.events & EPOLLOUT) {
            n |= EventOut;
        }
        if (ev.events & EPOLLERR) {
            n |= EventErr;
        }
        if (ev.events & EPOLLHUP) {
            n |= EventHup;
        }
        if (ev.events & EPOLLET) {
            n |= EventEt;
        }
        return n;
    }

    explicit EpollMuxer(std::size_t size_hint)
    : mux_{os::epoll_create(size_hint)}
    , tfd_{os::timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK)}
    {
        subscribe(*tfd_, 0, EventIn);
    }
    ~EpollMuxer() { unsubscribe(*tfd_); }

    // Copy.
    EpollMuxer(const EpollMuxer&) = delete;
    EpollMuxer& operator=(const EpollMuxer&) = delete;

    // Move.
    EpollMuxer(EpollMuxer&& rhs) = default;
    EpollMuxer& operator=(EpollMuxer&& rhs) = default;

    void swap(EpollMuxer& rhs) noexcept { std::swap(mux_, rhs.mux_); }
    int wait(Event buf[], std::size_t size, std::error_code& ec) const
    {
        // A zero timeout will disarm the timer.
        os::timerfd_settime(*tfd_, 0, Time{});
        return os::epoll_wait(*mux_, buf, size, -1, ec);
    }
    int wait(Event buf[], std::size_t size, Time timeout, std::error_code& ec) const
    {
        // A zero timeout will disarm the timer.
        os::timerfd_settime(*tfd_, 0, timeout);
        // Do not block if timer is zero.
        return os::epoll_wait(*mux_, buf, size, is_zero(timeout) ? 0 : -1, ec);
    }
    void subscribe(int fd, int sid, unsigned events)
    {
        Event ev;
        set_events(ev, fd, sid, events);
        os::epoll_ctl(*mux_, EPOLL_CTL_ADD, fd, ev);
    }
    void unsubscribe(int fd) noexcept
    {
        // In kernel versions before 2.6.9, the EPOLL_CTL_DEL operation required a non-null pointer
        // in event, even though this argument is ignored.
        Event ev{};
        std::error_code ec;
        os::epoll_ctl(*mux_, EPOLL_CTL_DEL, fd, ev, ec);
    }
    void set_events(int fd, int sid, unsigned events)
    {
        Event ev;
        set_events(ev, fd, sid, events);
        os::epoll_ctl(*mux_, EPOLL_CTL_MOD, fd, ev);
    }

  private:
    static void set_events(Event& ev, int fd, int sid, unsigned events) noexcept
    {
        unsigned n{};
        if (events & EventIn) {
            n |= EPOLLIN;
        }
        if (events & EventPri) {
            n |= EPOLLPRI;
        }
        if (events & EventOut) {
            n |= EPOLLOUT;
        }
        if (events & EventErr) {
            n |= EPOLLERR;
        }
        if (events & EventHup) {
            n |= EPOLLHUP;
        }
        if (events & EventEt) {
            n |= EPOLLET;
        }
        ev.events = n;
        ev.data.u64 = static_cast<std::uint64_t>(sid) << 32 | fd;
    }
    FileHandle mux_, tfd_;
};

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_MUXER_HPP
