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

#include <swirly/sys/Event.hpp>
#include <swirly/sys/Poll.hpp>

#include <swirly/Config.h>

#include <chrono>
#include <vector>

#include <unistd.h>

namespace swirly {

template <typename PolicyT>
class BasicMuxer {
  public:
    enum : FileEvents {
        In = PolicyT::In,
        Pri = PolicyT::Pri,
        Out = PolicyT::Out,
        Err = PolicyT::Err,
        Hup = PolicyT::Hup
    };
    using Id = typename PolicyT::Id;
    static constexpr Id invalid() noexcept { return PolicyT::invalid(); }

    using FileEvent = typename PolicyT::FileEvent;

    explicit BasicMuxer(std::size_t sizeHint)
      : md_{PolicyT::create(sizeHint)}
    {
    }
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
    BasicMuxer(BasicMuxer&& rhs)
      : md_{rhs.md_}
    {
        rhs.md_ = invalid();
    }
    BasicMuxer& operator=(BasicMuxer&& rhs)
    {
        close();
        std::swap(md_, rhs.md_);
        return *this;
    }

    void swap(BasicMuxer& rhs) noexcept { std::swap(md_, rhs.md_); }
    int wait(FileEvent* buf, std::size_t size, std::chrono::milliseconds timeout,
             std::error_code& ec) const
    {
        return PolicyT::wait(md_, buf, size, timeout, ec);
    }
    int wait(FileEvent* buf, std::size_t size, std::error_code& ec) const
    {
        return wait(buf, size, std::chrono::milliseconds::max(), ec);
    }
    void subscribe(int sid, int fd, FileEvents mask) { PolicyT::subscribe(md_, sid, fd, mask); }
    void unsubscribe(int fd) noexcept { PolicyT::unsubscribe(md_, fd); }
    void setMask(int fd, FileEvents mask) { PolicyT::setMask(md_, fd, mask); }

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
    enum : FileEvents { In = POLLIN, Pri = POLLPRI, Out = POLLOUT, Err = POLLERR, Hup = POLLHUP };
    using Id = Impl*;
    struct FileEvent {
        FileEvents events;
        struct {
            std::uint64_t u64;
        } data;
    };
    static constexpr Impl* invalid() noexcept { return nullptr; }

    static Impl* create(std::size_t sizeHint) { return new Impl{sizeHint}; }
    static void destroy(Impl* md) noexcept { delete md; }
    static int wait(Impl* md, FileEvent* buf, std::size_t size, std::chrono::milliseconds timeout,
                    std::error_code& ec)
    {
        return wait(md, buf, size,
                    timeout == std::chrono::milliseconds::max() ? -1 : timeout.count(), ec);
    }
    static void subscribe(Impl* md, int sid, int fd, FileEvents mask);
    static void unsubscribe(Impl* md, int fd) noexcept;
    static void setMask(Impl* md, int fd, FileEvents mask);

  private:
    static int wait(Impl* md, FileEvent* buf, std::size_t size, int timeout, std::error_code& ec);
};

using PollMuxer = BasicMuxer<PollPolicy>;

#if defined(__linux__)
struct EpollPolicy {
    enum : FileEvents {
        In = EPOLLIN,
        Pri = EPOLLPRI,
        Out = EPOLLOUT,
        Err = EPOLLERR,
        Hup = EPOLLHUP
    };
    using Id = int;
    using FileEvent = epoll_event;
    static constexpr int invalid() noexcept { return -1; }

    static int create(int sizeHint) { return sys::epoll_create(sizeHint); }
    static void destroy(int md) noexcept { ::close(md); }
    static int wait(int md, FileEvent* buf, std::size_t size, std::chrono::milliseconds timeout,
                    std::error_code& ec)
    {
        return sys::epoll_wait(
            md, buf, size, timeout == std::chrono::milliseconds::max() ? -1 : timeout.count(), ec);
    }
    static void subscribe(int md, int sid, int fd, FileEvents mask)
    {
        FileEvent ev;
        ev.events = mask;
        ev.data.u64 = static_cast<std::uint64_t>(sid) << 32 | fd;
        sys::epoll_ctl(md, EPOLL_CTL_ADD, fd, ev);
    }
    static void unsubscribe(int md, int fd) noexcept
    {
        // In kernel versions before 2.6.9, the EPOLL_CTL_DEL operation required a non-null pointer
        // in event, even though this argument is ignored.
        FileEvent ev{};
        std::error_code ec;
        sys::epoll_ctl(md, EPOLL_CTL_DEL, fd, ev, ec);
    }
    static void setMask(int md, int fd, FileEvents mask)
    {
        FileEvent ev;
        ev.events = mask;
        ev.data.fd = fd;
        sys::epoll_ctl(md, EPOLL_CTL_MOD, fd, ev);
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
