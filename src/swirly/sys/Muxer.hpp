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
#ifndef SWIRLY_SYS_MUXER_HPP
#define SWIRLY_SYS_MUXER_HPP

#include <swirly/sys/Poll.hpp>

#include <swirly/Config.h>

#include <chrono>
#include <vector>

#include <unistd.h>

namespace swirly {

using EventMask = std::uint32_t;

template <typename PolicyT>
class BasicMuxer {
  public:
    enum : EventMask {
        In = PolicyT::In,
        Pri = PolicyT::Pri,
        Out = PolicyT::Out,
        Err = PolicyT::Err,
        Hup = PolicyT::Hup
    };
    using Descriptor = typename PolicyT::Descriptor;
    static constexpr Descriptor invalid() noexcept { return PolicyT::invalid(); }

    using Event = typename PolicyT::Event;

    explicit BasicMuxer(std::size_t sizeHint) : md_{PolicyT::create(sizeHint)} {}
    ~BasicMuxer() noexcept
    {
        if (md_ != invalid()) {
            PolicyT::destroy(md_);
        }
    }

    // Copy.
    BasicMuxer(const BasicMuxer&) = delete;
    BasicMuxer& operator=(const BasicMuxer&) = delete;

    // Move.
    BasicMuxer(BasicMuxer&& rhs) : md_{rhs.md_} { rhs.md_ = invalid(); }
    BasicMuxer& operator=(BasicMuxer&& rhs)
    {
        close();
        std::swap(md_, rhs.md_);
        return *this;
    }

    void swap(BasicMuxer& rhs) noexcept { std::swap(md_, rhs.md_); }

    void insert(int sid, int fd, EventMask mask) { PolicyT::insert(md_, sid, fd, mask); }
    void update(int fd, EventMask mask) { PolicyT::update(md_, fd, mask); }
    void erase(int fd) { PolicyT::erase(md_, fd); }
    int wait(Event* buf, std::size_t size) const { return PolicyT::wait(md_, buf, size); }
    int wait(Event* buf, std::size_t size, std::chrono::milliseconds timeout) const
    {
        return PolicyT::wait(md_, buf, size, timeout);
    }

  private:
    void close() noexcept
    {
        if (md_ != invalid()) {
            PolicyT::close(md_);
            md_ = invalid();
        }
    }
    Descriptor md_{invalid()};
};

struct SWIRLY_API PollPolicy {
    struct Cmp {
        bool operator()(const pollfd& lhs, const pollfd& rhs) const noexcept
        {
            return lhs.fd < rhs.fd;
        }
    };
    struct Impl {
        explicit Impl(std::size_t sizeHint) { pfds.reserve(sizeHint); }
        std::vector<pollfd> pfds;
        std::vector<int> sids;
    };
    enum : EventMask { In = POLLIN, Pri = POLLPRI, Out = POLLOUT, Err = POLLERR, Hup = POLLHUP };
    using Descriptor = Impl*;
    struct Event {
        EventMask events;
        struct {
            std::uint64_t u64;
        } data;
    };
    static constexpr Impl* invalid() noexcept { return nullptr; }

    static void destroy(Impl* md) noexcept { delete md; }
    static Impl* create(std::size_t sizeHint) { return new Impl{sizeHint}; }
    static void insert(Impl* md, int sid, int fd, EventMask mask);
    static void update(Impl* md, int fd, EventMask mask);
    static void erase(Impl* md, int fd);
    static int wait(Impl* md, Event* buf, std::size_t size) { return wait(md, buf, size, -1); }
    static int wait(Impl* md, Event* buf, std::size_t size, std::chrono::milliseconds timeout)
    {
        return wait(md, buf, size, timeout.count());
    }

  private:
    static int wait(Impl* md, Event* buf, std::size_t size, int timeout);
};

using PollMuxer = BasicMuxer<PollPolicy>;

#if defined(__linux__)

struct EpollPolicy {
    enum : EventMask {
        In = EPOLLIN,
        Pri = EPOLLPRI,
        Out = EPOLLOUT,
        Err = EPOLLERR,
        Hup = EPOLLHUP
    };
    using Descriptor = int;
    using Event = epoll_event;
    static constexpr int invalid() noexcept { return -1; }

    static void destroy(int md) noexcept { ::close(md); }
    static int create(int sizeHint) { return sys::epoll_create(sizeHint); }
    static void insert(int md, int sid, int fd, EventMask mask)
    {
        Event event;
        event.events = mask;
        event.data.u64 = static_cast<std::uint64_t>(sid) << 32 | fd;
        sys::epoll_ctl(md, EPOLL_CTL_ADD, fd, event);
    }
    static void update(int md, int fd, EventMask mask)
    {
        Event event;
        event.events = mask;
        event.data.fd = fd;
        sys::epoll_ctl(md, EPOLL_CTL_MOD, fd, event);
    }
    static void erase(int md, int fd)
    {
        // In kernel versions before 2.6.9, the EPOLL_CTL_DEL operation required a non-null pointer
        // in event, even though this argument is ignored.
        Event event{};
        sys::epoll_ctl(md, EPOLL_CTL_DEL, fd, event);
    }
    static int wait(int md, Event* buf, std::size_t size)
    {
        return sys::epoll_wait(md, buf, size, -1);
    }
    static int wait(int md, Event* buf, std::size_t size, std::chrono::milliseconds timeout)
    {
        return sys::epoll_wait(md, buf, size, timeout.count());
    }
};

using EpollMuxer = BasicMuxer<EpollPolicy>;

#endif

#if defined(__linux__)
using Muxer = EpollMuxer;
#else
using Muxer = PollMuxer;
#endif

} // namespace swirly

#endif // SWIRLY_SYS_MUXER_HPP
