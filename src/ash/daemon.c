/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#include <dbr/daemon.h>

#include <dbr/err.h>

#include <errno.h>
#include <fcntl.h>    // open()
#include <signal.h>
#include <string.h>   // strerror()
#include <unistd.h>   // fork()

#include <sys/stat.h> // umask()

DBR_API DbrBool
dbr_daemon(const char* wd, mode_t mask)
{
    pid_t pid = fork();
    if (pid < 0) {
        dbr_err_setf(DBR_ESYSTEM, "fork() failed: %s", strerror(errno));
        goto fail1;
    }
    if (pid != 0) {
        // Exit parent process using system version of exit() to avoid flushing standard streams.
        _exit(0);
    }

    // Detach from controlling terminal by making process a session leader.
    if (setsid() < 0) {
        dbr_err_setf(DBR_ESYSTEM, "setsid() failed: %s", strerror(errno));
        goto fail1;
    }

    // Forking again ensures that the daemon process is not a session leader, and therefore cannot
    // regain access to a controlling terminal.
    signal(SIGHUP, SIG_IGN);
    pid = fork();
    if (pid < 0) {
        dbr_err_setf(DBR_ESYSTEM, "fork() failed: %s", strerror(errno));
        goto fail1;
    }
    if (pid != 0)
        _exit(0);

    // Change the current working directory.
    if (chdir(wd) < 0) {
        dbr_err_setf(DBR_EIO, "chdir() failed: %s", strerror(errno));
        goto fail1;
    }

    // Restrict file creation mode. This function is always successful.
    umask(mask);

    // Close all open file handles.
    const int fds = getdtablesize();
    for (int fd = 0; fd < fds; ++fd)
        close(fd);

    // Re-open standard file handles.
    if (open("/dev/null", O_RDONLY) < 0       // STDIN_FILENO
        || open("/dev/null", O_WRONLY) < 0    // STDOUT_FILENO
        || open("/dev/null", O_WRONLY) < 0) { // STDERR_FILENO
        dbr_err_setf(DBR_EIO, "open() failed: %s", strerror(errno));
        goto fail1;
    }

    return DBR_TRUE;
 fail1:
    return DBR_FALSE;
}
