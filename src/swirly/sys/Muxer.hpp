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
#include <swirly/sys/Types.hpp>

#include <swirly/Config.h>

#include <chrono>
#include <vector>

#include <unistd.h>

namespace swirly {

template <typename PolicyT>
class BasicMuxer {
  public:
    enum : IoEvents {
        In = PolicyT::In,
        Pri = PolicyT::Pri,
        Out = PolicyT::Out,
        Err = PolicyT::Err,
        Hup = PolicyT::Hup
    };
    using Id = typename PolicyT::Id;
    static constexpr Id invalid() noexcept { return PolicyT::invalid(); }

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
    int wait(Event* buf, std::size_t size, std::chrono::milliseconds timeout,
             std::error_code& ec) const
    {
        return PolicyT::wait(md_, buf, size, timeout, ec);
    }
    int wait(Event* buf, std::size_t size, std::error_code& ec) const
    {
        return wait(buf, size, std::chrono::milliseconds::max(), ec);
    }
    void attach(int sid, int fd, IoEvents mask) { PolicyT::attach(md_, sid, fd, mask); }
    void setMask(int fd, IoEvents mask) { PolicyT::setMask(md_, fd, mask); }
    void detach(int fd) noexcept { PolicyT::detach(md_, fd); }

  private:
    void close() noexcept
    {
        if (md_ != invalid()) {
            PolicyT::close(md_);
            md_ = invalid();
        }
    }
    Id md_{invalid()};
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
    enum : IoEvents { In = POLLIN, Pri = POLLPRI, Out = POLLOUT, Err = POLLERR, Hup = POLLHUP };
    using Id = Impl*;
    struct Event {
        IoEvents events;
        struct {
            std::uint64_t u64;
        } data;
    };
    static constexpr Impl* invalid() noexcept { return nullptr; }

    static Impl* create(std::size_t sizeHint) { return new Impl{sizeHint}; }
    static void destroy(Impl* md) noexcept { delete md; }
    static int wait(Impl* md, Event* buf, std::size_t size, std::chrono::milliseconds timeout,
                    std::error_code& ec)
    {
        return wait(md, buf, size,
                    timeout == std::chrono::milliseconds::max() ? -1 : timeout.count(), ec);
    }
    static void attach(Impl* md, int sid, int fd, IoEvents mask);
    static void setMask(Impl* md, int fd, IoEvents mask);
    static void detach(Impl* md, int fd) noexcept;

  private:
    static int wait(Impl* md, Event* buf, std::size_t size, int timeout, std::error_code& ec);
};

using PollMuxer = BasicMuxer<PollPolicy>;

#if defined(__linux__)
struct EpollPolicy {
    enum : IoEvents {
        In = EPOLLIN,
        Pri = EPOLLPRI,
        Out = EPOLLOUT,
        Err = EPOLLERR,
        Hup = EPOLLHUP
    };
    using Id = int;
    using Event = epoll_event;
    static constexpr int invalid() noexcept { return -1; }

    static int create(int sizeHint) { return sys::epoll_create(sizeHint); }
    static void destroy(int md) noexcept { ::close(md); }
    static int wait(int md, Event* buf, std::size_t size, std::chrono::milliseconds timeout,
                    std::error_code& ec)
    {
        return sys::epoll_wait(
            md, buf, size, timeout == std::chrono::milliseconds::max() ? -1 : timeout.count(), ec);
    }
    static void attach(int md, int sid, int fd, IoEvents mask)
    {
        Event event;
        event.events = mask;
        event.data.u64 = static_cast<std::uint64_t>(sid) << 32 | fd;
        sys::epoll_ctl(md, EPOLL_CTL_ADD, fd, event);
    }
    static void setMask(int md, int fd, IoEvents mask)
    {
        Event event;
        event.events = mask;
        event.data.fd = fd;
        sys::epoll_ctl(md, EPOLL_CTL_MOD, fd, event);
    }
    static void detach(int md, int fd) noexcept
    {
        // In kernel versions before 2.6.9, the EPOLL_CTL_DEL operation required a non-null pointer
        // in event, even though this argument is ignored.
        Event event{};
        std::error_code ec;
        sys::epoll_ctl(md, EPOLL_CTL_DEL, fd, event, ec);
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
