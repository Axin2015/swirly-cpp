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
#ifndef SWIRLY_SYS_FILE_HPP
#define SWIRLY_SYS_FILE_HPP

#include <swirly/sys/Error.hpp>
#include <swirly/sys/Handle.hpp>

#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

namespace swirly {

struct FilePolicy {
    using Descriptor = int;
    static constexpr int invalid() noexcept { return -1; }
    static void close(int d) noexcept { ::close(d); }
};

using File = Handle<FilePolicy>;

namespace sys {

/**
 * Open and possibly create a file.
 */
inline File open(const char* path, int flags, mode_t mode, std::error_code& ec) noexcept
{
    const auto fd = ::open(path, flags, mode);
    if (fd < 0) {
        ec = error(errno);
    }
    return fd;
}

/**
 * Open and possibly create a file.
 */
inline File open(const char* path, int flags, mode_t mode)
{
    const auto fd = ::open(path, flags, mode);
    if (fd < 0) {
        throw std::system_error{error(errno), "open"};
    }
    return fd;
}

/**
 * Open and possibly create a file.
 */
inline File open(const char* path, int flags, std::error_code& ec) noexcept
{
    const auto fd = ::open(path, flags);
    if (fd < 0) {
        ec = error(errno);
    }
    return fd;
}

/**
 * Open and possibly create a file.
 */
inline File open(const char* path, int flags)
{
    const auto fd = ::open(path, flags);
    if (fd < 0) {
        throw std::system_error{error(errno), "open"};
    }
    return fd;
}

/**
 * Get file status.
 */
inline void fstat(int fd, struct stat& statbuf, std::error_code& ec) noexcept
{
    const auto ret = ::fstat(fd, &statbuf);
    if (ret < 0) {
        ec = error(errno);
    }
}

/**
 * Get file status.
 */
inline void fstat(int fd, struct stat& statbuf)
{
    const auto ret = ::fstat(fd, &statbuf);
    if (ret < 0) {
        throw std::system_error{error(errno), "fstat"};
    }
}

/**
 * Truncate a file to a specified length.
 */
inline void ftruncate(int fd, off_t length, std::error_code& ec) noexcept
{
    const auto ret = ::ftruncate(fd, length);
    if (ret < 0) {
        ec = error(errno);
    }
}

/**
 * Truncate a file to a specified length.
 */
inline void ftruncate(int fd, off_t length)
{
    const auto ret = ::ftruncate(fd, length);
    if (ret < 0) {
        throw std::system_error{error(errno), "ftruncate"};
    }
}

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_FILE_HPP
