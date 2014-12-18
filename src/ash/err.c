/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/ash/err.h>

#include <sc/ash/log.h>

#include <limits.h> // PATH_MAX
#include <stdio.h>
#include <stdlib.h> // abort()
#include <string.h> // strcpy()

static __thread struct {
    int num;
    char file[PATH_MAX + 1];
    int line;
    char msg[SC_ERRMSG_MAX + 1];
} err;

SC_API void
sc_err_clear(void)
{
    err.num = 0;
    err.file[0] = '\0';
    err.line = 0;
    err.msg[0] = '\0';
}

SC_API void
sc_err_perror(const char* s)
{
    sc_log_error("%s:%d: %s: %s (%d)", err.file, err.line, s, err.msg, err.num);
}

SC_API void
sc_err_set_(int num, const char* file, int line, const char* msg)
{
    err.num = num;
    strncpy(err.file, file, PATH_MAX);
    err.file[PATH_MAX] = '\0';
    err.line = line;
    strncpy(err.msg, msg, SC_ERRMSG_MAX);
    err.msg[SC_ERRMSG_MAX] = '\0';
}

SC_API void
sc_err_setf_(int num, const char* file, int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    sc_err_vsetf_(num, file, line, format, args);
    va_end(args);
}

SC_API void
sc_err_vsetf_(int num, const char* file, int line, const char* format, va_list args)
{
    err.num = num;
    strncpy(err.file, file, PATH_MAX);
    err.file[PATH_MAX] = '\0';
    err.line = line;
    const int ret = vsnprintf(err.msg, SC_ERRMSG_MAX, format, args);
    // Defensive. Null termination is _not_ guaranteed by snprintf().
    err.msg[SC_ERRMSG_MAX] = '\0';
    if (ret < 0)
        abort();
}

SC_API void
sc_err_print_(int num, const char* file, int line, const char* msg)
{
    sc_err_set_(num, file, line, msg);
    sc_log_error("%s:%d: %s (%d)", err.file, err.line, err.msg, err.num);
}

SC_API void
sc_err_printf_(int num, const char* file, int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    sc_err_vsetf_(num, file, line, format, args);
    va_end(args);
    sc_log_error("%s:%d: %s (%d)", err.file, err.line, err.msg, err.num);
}

SC_API void
sc_err_vprintf_(int num, const char* file, int line, const char* format, va_list args)
{
    sc_err_vsetf_(num, file, line, format, args);
    sc_log_error("%s:%d: %s (%d)", err.file, err.line, err.msg, err.num);
}

SC_API int
sc_err_num(void)
{
    return err.num;
}

SC_API const char*
sc_err_file(void)
{
    return err.file;
}

SC_API int
sc_err_line(void)
{
    return err.line;
}

SC_API const char*
sc_err_msg(void)
{
    return err.msg;
}
