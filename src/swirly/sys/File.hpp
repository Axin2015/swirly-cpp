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

#include <swirly/sys/Buffer.hpp>
#include <swirly/sys/Error.hpp>
#include <swirly/sys/Handle.hpp>

#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

namespace swirly {

struct FilePolicy {
    using Id = int;
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
        ec = makeError(errno);
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
        throw std::system_error{makeError(errno), "open"};
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
        ec = makeError(errno);
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
        throw std::system_error{makeError(errno), "open"};
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
        ec = makeError(errno);
    }
}

/**
 * Get file status.
 */
inline void fstat(int fd, struct stat& statbuf)
{
    const auto ret = ::fstat(fd, &statbuf);
    if (ret < 0) {
        throw std::system_error{makeError(errno), "fstat"};
    }
}

/**
 * Truncate a file to a specified length.
 */
inline void ftruncate(int fd, off_t length, std::error_code& ec) noexcept
{
    const auto ret = ::ftruncate(fd, length);
    if (ret < 0) {
        ec = makeError(errno);
    }
}

/**
 * Truncate a file to a specified length.
 */
inline void ftruncate(int fd, off_t length)
{
    const auto ret = ::ftruncate(fd, length);
    if (ret < 0) {
        throw std::system_error{makeError(errno), "ftruncate"};
    }
}

/**
 * Read from a file descriptor.
 */
inline ssize_t read(int fd, void* buf, std::size_t len, std::error_code& ec) noexcept
{
    const auto ret = ::read(fd, buf, len);
    if (ret < 0) {
        ec = makeError(errno);
    }
    return ret;
}

/**
 * Read from a file descriptor.
 */
inline std::size_t read(int fd, void* buf, std::size_t len)
{
    const auto ret = ::read(fd, buf, len);
    if (ret < 0) {
        throw std::system_error{makeError(errno), "read"};
    }
    return ret;
}

/**
 * Read from a file descriptor.
 */
inline ssize_t read(int fd, const MutableBuffer& buf, std::error_code& ec) noexcept
{
    return read(fd, buffer_cast<void*>(buf), buffer_size(buf), ec);
}

/**
 * Read from a file descriptor.
 */
inline std::size_t read(int fd, const MutableBuffer& buf) noexcept
{
    return read(fd, buffer_cast<void*>(buf), buffer_size(buf));
}

/**
 * Write to a file descriptor.
 */
inline ssize_t write(int fd, const void* buf, std::size_t len, std::error_code& ec) noexcept
{
    const auto ret = ::write(fd, buf, len);
    if (ret < 0) {
        ec = makeError(errno);
    }
    return ret;
}

/**
 * Write to a file descriptor.
 */
inline std::size_t write(int fd, const void* buf, std::size_t len)
{
    const auto ret = ::write(fd, buf, len);
    if (ret < 0) {
        throw std::system_error{makeError(errno), "write"};
    }
    return ret;
}

/**
 * Write to a file descriptor.
 */
inline ssize_t write(int fd, const ConstBuffer& buf, std::error_code& ec) noexcept
{
    return write(fd, buffer_cast<const void*>(buf), buffer_size(buf), ec);
}

/**
 * Write to a file descriptor.
 */
inline std::size_t write(int fd, const ConstBuffer& buf)
{
    return write(fd, buffer_cast<const void*>(buf), buffer_size(buf));
}

/**
 * File control.
 */
inline int fcntl(int fd, int cmd, std::error_code& ec) noexcept
{
    const auto ret = ::fcntl(fd, cmd);
    if (ret < 0) {
        ec = makeError(errno);
    }
    return ret;
}

/**
 * File control.
 */
template <typename ArgT>
inline int fcntl(int fd, int cmd, ArgT arg, std::error_code& ec) noexcept
{
    const auto ret = ::fcntl(fd, cmd, arg);
    if (ret < 0) {
        ec = makeError(errno);
    }
    return ret;
}

/**
 * File control.
 */
inline int fcntl(int fd, int cmd)
{
    const auto ret = ::fcntl(fd, cmd);
    if (ret < 0) {
        throw std::system_error{makeError(errno), "fcntl"};
    }
    return ret;
}

/**
 * File control.
 */
template <typename ArgT>
inline int fcntl(int fd, int cmd, ArgT arg)
{
    const auto ret = ::fcntl(fd, cmd, arg);
    if (ret < 0) {
        throw std::system_error{makeError(errno), "fcntl"};
    }
    return ret;
}

} // namespace sys

inline void setNonBlock(int fd, std::error_code& ec) noexcept
{
    fcntl(fd, F_SETFL, O_NONBLOCK, ec);
}

inline void setNonBlock(int fd)
{
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

} // namespace swirly

#endif // SWIRLY_SYS_FILE_HPP
