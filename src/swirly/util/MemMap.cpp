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
#include "MemMap.hpp"

#include <system_error>

using namespace std;

namespace swirly {

void MemMapDeleter::operator()(MemMapHandle h) noexcept
{
    if (h) {
        munmap(h.get(), h.size());
    }
}

MemMap openMemMap(void* addr, size_t len, int prot, int flags, FileHandle fh, off_t off)
{
    const MemMapHandle h{mmap(addr, len, prot, flags, fh.get(), off), len};
    if (!h) {
        throw system_error{errno, system_category(), "mmap failed"};
    }
    return MemMap{h};
}
}
