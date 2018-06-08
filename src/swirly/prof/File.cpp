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
#include "File.hpp"

#include <system_error>

namespace swirly {
inline namespace prof {
using namespace std;
namespace {
inline error_code make_error(int err)
{
    return error_code{err, system_category()};
}
} // namespace

FilePtr open_file(const char* path, const char* mode)
{
    FilePtr fp{fopen(path, mode), fclose};
    if (!fp) {
        throw system_error{make_error(errno), "fopen"};
    }
    return fp;
}

} // namespace prof
} // namespace swirly
