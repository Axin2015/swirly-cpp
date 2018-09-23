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
#ifndef SWIRLY_SYS_TIMERFD_HPP
#define SWIRLY_SYS_TIMERFD_HPP

#include <swirly/sys/File.hpp>

#include <swirly/util/Time.hpp>

#include <sys/timerfd.h>

namespace swirly {
inline namespace sys {
namespace os {

/**
 * Create a file descriptor for timer notification.
 */
inline FileHandle timerfd_create(int clockid, int flags, std::error_code& ec) noexcept
{
    const auto fd = ::timerfd_create(clockid, flags);
    if (fd < 0) {
        ec = make_error(errno);
    }
    return fd;
}

/**
 * Create a file descriptor for timer notification.
 */
inline FileHandle timerfd_create(int clockid, int flags)
{
    const auto fd = ::timerfd_create(clockid, flags);
    if (fd < 0) {
        throw std::system_error{make_error(errno), "timerfd_create"};
    }
    return fd;
}

/**
 * Arm or disarm timer.
 */
inline void timerfd_settime(int fd, int flags, const itimerspec& new_value, itimerspec& old_value,
                            std::error_code& ec) noexcept
{
    const auto ret = ::timerfd_settime(fd, flags, &new_value, &old_value);
    if (ret < 0) {
        ec = make_error(errno);
    }
}

/**
 * Arm or disarm timer.
 */
inline void timerfd_settime(int fd, int flags, const itimerspec& new_value, itimerspec& old_value)
{
    const auto ret = ::timerfd_settime(fd, flags, &new_value, &old_value);
    if (ret < 0) {
        throw std::system_error{make_error(errno), "timerfd_settime"};
    }
}

/**
 * Arm or disarm timer.
 */
inline void timerfd_settime(int fd, int flags, const itimerspec& new_value,
                            std::error_code& ec) noexcept
{
    const auto ret = ::timerfd_settime(fd, flags, &new_value, nullptr);
    if (ret < 0) {
        ec = make_error(errno);
    }
}

/**
 * Arm or disarm timer.
 */
inline void timerfd_settime(int fd, int flags, const itimerspec& new_value)
{
    const auto ret = ::timerfd_settime(fd, flags, &new_value, nullptr);
    if (ret < 0) {
        throw std::system_error{make_error(errno), "timerfd_settime"};
    }
}

/**
 * Arm or disarm timer.
 */
inline void timerfd_settime(int fd, int flags, Time expiry, Duration interval)
{
    return timerfd_settime(fd, flags | TFD_TIMER_ABSTIME,
                           {to_timespec(interval), to_timespec(expiry)});
}

/**
 * Arm or disarm timer.
 */
inline void timerfd_settime(int fd, int flags, Time expiry)
{
    return timerfd_settime(fd, flags | TFD_TIMER_ABSTIME, {{}, to_timespec(expiry)});
}

} // namespace os

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_TIMERFD_HPP
