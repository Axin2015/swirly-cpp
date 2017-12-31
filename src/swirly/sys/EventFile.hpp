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
#ifndef SWIRLY_SYS_EVENTFILE_HPP
#define SWIRLY_SYS_EVENTFILE_HPP

#include <swirly/sys/File.hpp>

namespace swirly {
#if defined(__linux__)
class EventFd {
  public:
    explicit EventFd(int flags = 0)
      : file_{sys::eventfd(0, eventFlags(flags) | EFD_NONBLOCK)}
    {
    }
    ~EventFd() noexcept = default;

    // Copy.
    EventFd(const EventFd&) = delete;
    EventFd& operator=(const EventFd&) = delete;

    // Move.
    EventFd(EventFd&&) = default;
    EventFd& operator=(EventFd&&) = default;

    int waitfd() const { return file_.get(); }
    void flush()
    {
        // Adds the 8-byte integer value supplied in its buffer to the counter.
        char buf[sizeof(std::int64_t)];
        sys::read(*file_, buf, sizeof(buf));
    }
    void notify()
    {
        // Adds the 8-byte integer value supplied in its buffer to the counter.
        union {
            char buf[sizeof(std::int64_t)];
            std::int64_t val;
        } u;
        u.val = 1;
        sys::write(*file_, u.buf, sizeof(u.buf));
    }

  private:
    static int eventFlags(int in) noexcept
    {
        int out{};
        if (in & O_CLOEXEC) {
            out |= EFD_CLOEXEC;
        }
        return out;
    }
    File file_;
};
#endif

// Emulate using a Unix pipe.
class EventPipe {
  public:
    explicit EventPipe(int flags = 0)
      : pipe_{sys::pipe2(flags | O_NONBLOCK)}
    {
    }
    ~EventPipe() noexcept = default;

    // Copy.
    EventPipe(const EventPipe&) = delete;
    EventPipe& operator=(const EventPipe&) = delete;

    // Move.
    EventPipe(EventPipe&&) = default;
    EventPipe& operator=(EventPipe&&) = default;

    int waitfd() const { return pipe_.first.get(); }
    void flush()
    {
        // Drain block of bytes.
        char buf[1 << 10];
        sys::read(*pipe_.first, buf, sizeof(buf));
    }
    void notify()
    {
        // Post a single charactor.
        const char ch{'\0'};
        sys::write(*pipe_.second, &ch, 1);
    }

  private:
    std::pair<File, File> pipe_;
};

#if defined(__linux__)
using EventFile = EventFd;
#else
using EventFile = EventPipe;
#endif

} // namespace swirly

#endif // SWIRLY_SYS_EVENTFILE_HPP
