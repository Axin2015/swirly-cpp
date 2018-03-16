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
#ifndef SWIRLY_UTIL_FILE_HPP
#define SWIRLY_UTIL_FILE_HPP

#include <swirly/sys/File.hpp>

namespace swirly {
inline namespace util {

/**
 * Get file size.
 */
inline std::size_t fileSize(int fd)
{
    struct stat st;
    os::fstat(fd, st);
    return st.st_size;
}

/**
 * Get current file mode.
 */
inline mode_t fileMode() noexcept
{
    mode_t mode{umask(0)};
    umask(mode);
    return mode;
}

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_FILE_HPP
