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
#include "Daemon.hpp"

#include <swirly/sys/File.hpp>
#include <swirly/sys/System.hpp>

using namespace std;

namespace swirly {

void daemon()
{
    pid_t pid{sys::fork()};
    if (pid != 0) {
        // Exit parent process using system version of exit() to avoid flushing standard streams.
        // FIXME: use quick_exit() when available on OSX.
        _exit(0);
    }

    // Detach from controlling terminal by making process a session leader.
    sys::setsid();

    // Forking again ensures that the daemon process is not a session leader, and therefore cannot
    // regain access to a controlling terminal.
    pid = sys::fork();
    if (pid != 0) {
        // FIXME: use quick_exit() when available on OSX.
        _exit(0);
    }

    // Re-open standard input.
    close(STDIN_FILENO);
    sys::open("/dev/null", O_RDONLY);

    // Close all non-standard file handles.
    const int fds{getdtablesize()};
    for (int fd{STDERR_FILENO + 1}; fd < fds; ++fd) {
        close(fd);
    }

    // Note that the standard output handles are unchanged.
}

} // namespace swirly
