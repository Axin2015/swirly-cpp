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
#include "PidFile.hpp"

#include <swirly/sys/File.hpp>

#include <swirly/contrib/flopen.c>
#include <swirly/contrib/pidfile.c>

// Note that it is safe to pass null to the pidfile_write(), pidfile_remove() and pidfile_close()
// functions.

namespace swirly {
inline namespace sys {
using namespace std;

void detail::PidFileDeleter::operator()(pidfh* pfh) const noexcept
{
    pidfile_remove(pfh);
}

PidFile open_pid_file(const char* path, mode_t mode)
{
    pid_t pid{};
    PidFile pf{pidfile_open(path, mode, &pid)};
    if (!pf) {
        if (errno == EEXIST) {
            using namespace string_literals;
            throw runtime_error{"daemon already running, pid: "s + to_string(pid)};
        }
        throw system_error{os::make_error(errno), "pidfile_open"};
    }
    return pf;
}

void close_pid_file(PidFile& pf) noexcept
{
    if (pf) {
        pidfile_close(pf.release());
    }
}

void write_pid_file(PidFile& pf)
{
    if (pf && pidfile_write(pf.get()) < 0) {
        throw system_error{os::make_error(errno), "pidfile_write"};
    }
}

} // namespace sys
} // namespace swirly
