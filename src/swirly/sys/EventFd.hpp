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
#ifndef SWIRLY_SYS_EVENTFD_HPP
#define SWIRLY_SYS_EVENTFD_HPP

#include <swirly/sys/File.hpp>

#include <sys/eventfd.h>

namespace swirly {
inline namespace sys {
namespace os {

/**
 * Create a file descriptor for event notification.
 */
inline FileHandle eventfd(unsigned intval, int flags, std::error_code& ec) noexcept
{
    const auto fd = ::eventfd(intval, flags);
    if (fd < 0) {
        ec = make_error(errno);
    }
    return fd;
}

/**
 * Create a file descriptor for event notification.
 */
inline FileHandle eventfd(unsigned intval, int flags)
{
    const auto fd = ::eventfd(intval, flags);
    if (fd < 0) {
        throw std::system_error{make_error(errno), "eventfd"};
    }
    return fd;
}

} // namespace os

class EventFd {
  public:
    EventFd(unsigned intval, int flags)
    : fh_{os::eventfd(intval, flags)}
    {
    }
    ~EventFd() = default;

    // Copy.
    EventFd(const EventFd&) = delete;
    EventFd& operator=(const EventFd&) = delete;

    // Move.
    EventFd(EventFd&&) = default;
    EventFd& operator=(EventFd&&) = default;

    int fd() const noexcept { return fh_.get(); }
    std::int64_t read()
    {
        union {
            char buf[sizeof(std::int64_t)];
            std::int64_t val;
        } u;
        os::read(*fh_, u.buf, sizeof(u.buf));
        return u.val;
    }
    void write(std::int64_t val, std::error_code& ec) noexcept
    {
        // Adds the 8-byte integer value supplied in its buffer to the counter.
        union {
            char buf[sizeof(std::int64_t)];
            std::int64_t val;
        } u;
        u.val = val;
        os::write(*fh_, u.buf, sizeof(u.buf), ec);
    }
    void write(std::int64_t val)
    {
        // Adds the 8-byte integer value supplied in its buffer to the counter.
        union {
            char buf[sizeof(std::int64_t)];
            std::int64_t val;
        } u;
        u.val = val;
        os::write(*fh_, u.buf, sizeof(u.buf));
    }

  private:
    FileHandle fh_;
};

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_EVENTFD_HPP
