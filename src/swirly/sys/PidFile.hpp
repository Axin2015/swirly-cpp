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
#ifndef SWIRLY_SYS_PIDFILE_HPP
#define SWIRLY_SYS_PIDFILE_HPP

#include <swirly/Config.h>

#include <memory>

struct pidfh;

namespace swirly {
namespace detail {
struct SWIRLY_API PidFileDeleter {
    void operator()(pidfh* pfh) const noexcept;
};
} // namespace detail

using PidFile = std::unique_ptr<pidfh, detail::PidFileDeleter>;

/**
 * Create pidfile and obtain lock. An exception is thrown if the pidfile cannot be locked, because a
 * daemon is already running. If the path argument is null, then /var/run/{progname}.pid is used as
 * the pidfile location.
 */
SWIRLY_API PidFile openPidFile(const char* path, mode_t mode);

/**
 * Close pidfile without removing it. This function should be used when forking daemon processes.
 */
SWIRLY_API void closePidFile(PidFile& pf) noexcept;

/**
 * Write process' PID into pidfile.
 */
SWIRLY_API void writePidFile(PidFile& pf);

} // namespace swirly

#endif // SWIRLY_SYS_PIDFILE_HPP
