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
#ifndef SWIRLY_UTIL_MEMMAP_HPP
#define SWIRLY_UTIL_MEMMAP_HPP

#include <swirly/util/File.hpp>

#include <sys/mman.h>

namespace swirly {

/**
 * Memory-mapped addressed may be zero (in rare situations), but can never be MAP_FAILED.
 */
class SWIRLY_API MemMapHandle {
  public:
    MemMapHandle(std::nullptr_t = nullptr) noexcept {}
    MemMapHandle(void* ptr, std::size_t size) noexcept : ptr_{ptr}, size_{size} {}
    void* get() const noexcept { return ptr_; }
    void* data() const noexcept { return ptr_; }
    std::size_t size() const noexcept { return size_; }
    explicit operator bool() const noexcept { return ptr_ != MAP_FAILED; }

  private:
    void* ptr_{MAP_FAILED};
    std::size_t size_{0};
};

inline bool operator==(MemMapHandle lhs, MemMapHandle rhs)
{
    return lhs.get() == rhs.get() && lhs.size() == rhs.size();
}

inline bool operator!=(MemMapHandle lhs, MemMapHandle rhs)
{
    return !(lhs == rhs);
}

struct SWIRLY_API MemMapDeleter {
    using pointer = MemMapHandle;
    void operator()(MemMapHandle h) noexcept;
};

using MemMap = std::unique_ptr<MemMapHandle, MemMapDeleter>;

SWIRLY_API MemMap openMemMap(void* addr, std::size_t len, int prot, int flags, FileHandle fh,
                             off_t off);

} // swirly

#endif // SWIRLY_UTIL_MEMMAP_HPP
