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
#ifndef SWIRLY_UTIL_FILE_HPP
#define SWIRLY_UTIL_FILE_HPP

#include <swirly/util/Defs.hpp>

#include <cassert>
#include <cstddef> // nullptr_t
#include <memory>

namespace swirly {

/**
 * Custom Handle type that satisfies the NullablePointer concept. This is intended to be used with
 * unique_ptr<>.
 */
class SWIRLY_API FileHandle {
  public:
    FileHandle(std::nullptr_t = nullptr) {}
    FileHandle(int fd) : fd_{fd} { assert(fd >= -1); }
    int get() const noexcept { return fd_; }
    explicit operator bool() const noexcept { return fd_ != -1; }

  private:
    int fd_{-1};
};

inline bool operator==(FileHandle lhs, FileHandle rhs)
{
    return lhs.get() == rhs.get();
}

inline bool operator!=(FileHandle lhs, FileHandle rhs)
{
    return !(lhs == rhs);
}

struct SWIRLY_API FileDeleter {
    using pointer = FileHandle;
    void operator()(FileHandle h) noexcept;
};

using File = std::unique_ptr<FileHandle, FileDeleter>;

SWIRLY_API File openFile(const char* path, int flags, mode_t mode);

SWIRLY_API File openFile(const char* path, int flags);

SWIRLY_API void resize(FileHandle h, std::size_t size);

SWIRLY_API std::size_t size(FileHandle h);

} // swirly

#endif // SWIRLY_UTIL_FILE_HPP
