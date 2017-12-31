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
#ifndef SWIRLY_UTIL_LOG_HPP
#define SWIRLY_UTIL_LOG_HPP

#include <swirly/util/Limits.hpp>
#include <swirly/util/Stream.hpp>

#include <swirly/sys/Log.hpp>

namespace swirly {

/**
 * Logger callback function.
 */
using LogMsg = StringBuilder<MaxLogMsg>;

/**
 * Thread-local log message. This thread-local instance of StringBuilder can be used to format log
 * messages before writing to the log. Note that the StringBuilder is reset each time this function
 * is called.
 */
SWIRLY_API LogMsg& logMsg() noexcept;

} // namespace swirly

#endif // SWIRLY_UTIL_LOG_HPP
