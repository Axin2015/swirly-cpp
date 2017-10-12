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

template <typename PolicyT>
class BasicMuxer {
  public:
    enum : std::uint32_t {
        In = PolicyT::In,
        Pri = PolicyT::Pri,
        Out = PolicyT::Out,
        Err = PolicyT::Err,
        Hup = PolicyT::Hup,
        RdHup = PolicyT::RdHup
    };
    using Descriptor = typename PolicyT::Descriptor;
    using Event = typename PolicyT::Event;
    using Data = decltype(Event{}.data);
    static constexpr Descriptor invalid() noexcept { return PolicyT::invalid(); }
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

    void insert(int fd, Event event) { PolicyT::insert(md_, fd, event); }
    void update(int fd, Event event) { PolicyT::update(md_, fd, event); }
    void erase(int fd) { PolicyT::erase(md_, fd); }
    int wait(Event* buf, std::size_t size) { return PolicyT::wait(md_, buf, size); }
    int wait(Event* buf, std::size_t size, std::chrono::milliseconds timeout)
    {
        return PolicyT::wait(md_, buf, size, timeout.count());
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
    union Data {
        void* ptr;
        int fd;
        std::uint32_t u32;
        std::uint64_t u64;
    };
    struct Event {
        std::uint32_t events;
        Data data;
    };
    struct Impl {
        explicit Impl(std::size_t sizeHint)
        {
            pfds.reserve(sizeHint);
            data.reserve(sizeHint);
        }
        std::vector<pollfd> pfds;
        std::vector<Data> data;
    };
    enum : std::uint32_t {
        In = POLLIN,
        Pri = POLLPRI,
        Out = POLLOUT,
        Err = POLLERR,
        Hup = POLLHUP
    };
    using Descriptor = Impl*;
    static constexpr Impl* invalid() noexcept { return nullptr; }
    static void destroy(Impl* md) noexcept { delete md; }
    static Impl* create(std::size_t sizeHint) { return new Impl{sizeHint}; }
    static void insert(Impl* md, int fd, Event event);
    static void update(Impl* md, int fd, Event event);
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
    enum : std::uint32_t {
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
    static void insert(int md, int fd, Event event)
    {
        sys::epoll_ctl(md, EPOLL_CTL_ADD, fd, event);
    }
    static void update(int md, int fd, Event event)
    {
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

} // namespace swirly

#endif // SWIRLY_SYS_MUXER_HPP
