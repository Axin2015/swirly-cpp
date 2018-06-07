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

#include <swirly/Config.h>

#include <chrono>

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
    : fh_{os::epoll_create(size_hint)}
    {
    }

    // Copy.
    EpollMuxer(const EpollMuxer&) = delete;
    EpollMuxer& operator=(const EpollMuxer&) = delete;

    // Move.
    EpollMuxer(EpollMuxer&& rhs) = default;
    EpollMuxer& operator=(EpollMuxer&& rhs) = default;

    void swap(EpollMuxer& rhs) noexcept { std::swap(fh_, rhs.fh_); }
    int wait(Event buf[], std::size_t size, std::chrono::milliseconds timeout,
             std::error_code& ec) const
    {
        return os::epoll_wait(*fh_, buf, size,
                              timeout == std::chrono::milliseconds::max() ? -1 : timeout.count(),
                              ec);
    }
    int wait(Event buf[], std::size_t size, std::error_code& ec) const
    {
        return wait(buf, size, std::chrono::milliseconds::max(), ec);
    }
    void subscribe(int fd, int sid, unsigned events)
    {
        Event ev;
        set_events(ev, fd, sid, events);
        os::epoll_ctl(*fh_, EPOLL_CTL_ADD, fd, ev);
    }
    void unsubscribe(int fd) noexcept
    {
        // In kernel versions before 2.6.9, the EPOLL_CTL_DEL operation required a non-null pointer
        // in event, even though this argument is ignored.
        Event ev{};
        std::error_code ec;
        os::epoll_ctl(*fh_, EPOLL_CTL_DEL, fd, ev, ec);
    }
    void set_events(int fd, int sid, unsigned events)
    {
        Event ev;
        set_events(ev, fd, sid, events);
        os::epoll_ctl(*fh_, EPOLL_CTL_MOD, fd, ev);
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
    FileHandle fh_;
};

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_MUXER_HPP
