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
#include "File.hpp"

#include <system_error>

#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

namespace swirly {

void FileDeleter::operator()(FileHandle h) noexcept
{
    if (h) {
        close(h.get());
    }
}

File openFile(const char* path, int flags, mode_t mode)
{
    const FileHandle h = open(path, flags, mode);
    if (!h) {
        throw system_error{errno, system_category(), "open failed"};
    }
    return File{h};
}

File openFile(const char* path, int flags)
{
    const FileHandle h = open(path, flags);
    if (!h) {
        throw system_error{errno, system_category(), "open failed"};
    }
    return File{h};
}

void resize(FileHandle h, size_t size)
{
    if (ftruncate(h.get(), size) < 0) {
        throw system_error{errno, system_category(), "ftruncate failed"};
    }
}

size_t size(FileHandle h)
{
    struct stat st;
    if (fstat(h.get(), &st) < 0) {
        throw system_error{errno, system_category(), "fstat failed"};
    }
    return st.st_size;
}
}
