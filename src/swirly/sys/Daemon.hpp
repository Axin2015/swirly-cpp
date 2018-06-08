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
#ifndef SWIRLY_SYS_DAEMON_HPP
#define SWIRLY_SYS_DAEMON_HPP

#include <swirly/Config.h>

#include <sys/types.h>

namespace swirly {
inline namespace sys {

/**
 * Close all non-standard file handles.
 */
SWIRLY_API void close_all() noexcept;

/**
 * Daemonise process. Detach from controlling terminal and run in the background as a system daemon.
 */
SWIRLY_API void daemon();

} // namespace sys
} // namespace swirly

#endif // SWIRLY_SYS_DAEMON_HPP
