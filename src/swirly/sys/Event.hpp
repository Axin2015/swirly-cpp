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
#ifndef SWIRLY_SYS_EVENT_HPP
#define SWIRLY_SYS_EVENT_HPP

#include "File.hpp"

namespace swirly {
inline namespace sys {

enum : unsigned {
    EventIn = 1 << 0,
    EventPri = 1 << 1,
    EventOut = 1 << 2,
    EventErr = 1 << 3,
    EventHup = 1 << 4,
    EventEt = 1u << 31
};

struct MsgEvent {
    int type;
    char data[1524];
};
static_assert(std::is_pod_v<MsgEvent>);
static_assert(sizeof(MsgEvent) + sizeof(std::int64_t) == 1536);

template <typename DataT>
void emplace_event(MsgEvent& ev, int type) noexcept
{
    static_assert(alignof(DataT) <= 8);
    static_assert(std::is_nothrow_default_constructible_v<DataT>);
    static_assert(std::is_trivially_copyable_v<DataT>);
    ev.type = type;
    ::new (ev.data) DataT{};
}

template <typename DataT, typename... ArgsT>
void emplace_event(MsgEvent& ev, int type, ArgsT&&... args) noexcept
{
    static_assert(alignof(DataT) <= 8);
    static_assert(std::is_nothrow_constructible_v<DataT, ArgsT...>);
    static_assert(std::is_trivially_copyable_v<DataT>);
    ev.type = type;
    ::new (ev.data) DataT{std::forward<ArgsT>(args)...};
}

template <typename DataT>
const DataT& data(const MsgEvent& ev) noexcept
{
    return *reinterpret_cast<const DataT*>(ev.data);
}

template <typename DataT>
DataT& data(MsgEvent& ev) noexcept
{
    return *reinterpret_cast<DataT*>(ev.data);
}

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

#endif // SWIRLY_SYS_EVENT_HPP
