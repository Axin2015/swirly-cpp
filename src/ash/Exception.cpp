/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
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
#include <swirly/ash/Exception.hpp>

#include <cstdio> // vsnprintf()

using namespace std;

namespace swirly {

Exception::~Exception() noexcept = default;

const char* Exception::what() const noexcept
{
    return msg_;
}

void format(Exception& e, const char* fmt, ...) noexcept
{
    va_list args;
    va_start(args, fmt);
    format(e, fmt, args);
    va_end(args);
}

void format(Exception& e, const char* fmt, std::va_list args) noexcept
{
    const auto ret = std::vsnprintf(e.msg_, sizeof(e.msg_), fmt, args);
    if (ret < 0)
        std::terminate();
}

} // swirly
