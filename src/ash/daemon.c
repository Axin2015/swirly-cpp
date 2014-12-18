/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/ash/daemon.h>

#include <sc/ash/err.h>

#include <errno.h>
#include <fcntl.h>    // open()
#include <string.h>   // strerror()
#include <unistd.h>   // fork()

#include <sys/stat.h> // umask()

SC_API ScBool
sc_daemon(const char* wd, mode_t mask)
{
    pid_t pid = fork();
    if (pid < 0) {
        sc_err_setf(SC_ESYSTEM, "fork() failed: %s", strerror(errno));
        goto fail1;
    }
    if (pid != 0) {
        // Exit parent process using system version of exit() to avoid flushing standard streams.
        _exit(0);
    }

    // Detach from controlling terminal by making process a session leader.
    if (setsid() < 0) {
        sc_err_setf(SC_ESYSTEM, "setsid() failed: %s", strerror(errno));
        goto fail1;
    }

    // Forking again ensures that the daemon process is not a session leader, and therefore cannot
    // regain access to a controlling terminal.
    pid = fork();
    if (pid < 0) {
        sc_err_setf(SC_ESYSTEM, "fork() failed: %s", strerror(errno));
        goto fail1;
    }
    if (pid != 0)
        _exit(0);

    // Change the current working directory.
    if (chdir(wd) < 0) {
        sc_err_setf(SC_EIO, "chdir() failed: %s", strerror(errno));
        goto fail1;
    }

    // Restrict file creation mode. This function is always successful.
    umask(mask);

    // Re-open standard input.
    close(STDIN_FILENO);
    if (open("/dev/null", O_RDONLY) < 0) {
        sc_err_setf(SC_EIO, "open() failed: %s", strerror(errno));
        goto fail1;
    }

    // Close all non-standard file handles.
    const int fds = getdtablesize();
    for (int fd = STDERR_FILENO + 1; fd < fds; ++fd)
        close(fd);

    // Note that the standard output handles are unchanged.
    return SC_TRUE;
 fail1:
    return SC_FALSE;
}
