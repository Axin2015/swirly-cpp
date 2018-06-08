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
#ifndef SWIRLY_SYS_MMAP_HPP
#define SWIRLY_SYS_MMAP_HPP

#include <swirly/sys/Error.hpp>

#include <swirly/Config.h>

#include <memory>
#include <system_error>

#include <sys/mman.h>

namespace swirly {
inline namespace sys {

/**
 * Memory-mapped addressed may be zero (in rare situations), but can never be MAP_FAILED.
 */
class MMapPointer {
  public:
    MMapPointer(std::nullptr_t = nullptr) noexcept {}
    MMapPointer(void* ptr, std::size_t size) noexcept
    : ptr_{ptr}
    , size_{size}
    {
    }
    void* get() const noexcept { return ptr_; }
    void* data() const noexcept { return ptr_; }
    std::size_t size() const noexcept { return size_; }
    explicit operator bool() const noexcept { return ptr_ != MAP_FAILED; }

  private:
    void* ptr_{MAP_FAILED};
    std::size_t size_{0};
};

inline bool operator==(MMapPointer lhs, MMapPointer rhs)
{
    return lhs.get() == rhs.get() && lhs.size() == rhs.size();
}

inline bool operator!=(MMapPointer lhs, MMapPointer rhs)
{
    return !(lhs == rhs);
}

struct MMapDeleter {
    using pointer = MMapPointer;
    void operator()(MMapPointer p) const noexcept
    {
        if (p) {
            munmap(p.get(), p.size());
        }
    }
};

using MMap = std::unique_ptr<MMapPointer, MMapDeleter>;

namespace os {

/**
 * Map files or devices into memory.
 */
inline MMap mmap(void* addr, size_t len, int prot, int flags, int fd, off_t off,
                 std::error_code& ec) noexcept
{
    const MMapPointer p{::mmap(addr, len, prot, flags, fd, off), len};
    if (!p) {
        ec = make_error(errno);
    }
    return MMap{p};
}

inline MMap mmap(void* addr, size_t len, int prot, int flags, int fd, off_t off)
{
    const MMapPointer p{::mmap(addr, len, prot, flags, fd, off), len};
    if (!p) {
        throw std::system_error{make_error(errno), "mmap"};
    }
    return MMap{p};
}

} // namespace os
} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_MMAP_HPP
