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
#include <dbr/err.h>

#include <dbr/log.h>

#include <limits.h> // PATH_MAX
#include <stdio.h>
#include <stdlib.h> // abort()
#include <string.h> // strcpy()

static __thread struct {
    int num;
    char file[PATH_MAX + 1];
    int line;
    char msg[DBR_ERRMSG_MAX + 1];
} err;

DBR_API void
dbr_err_clear(void)
{
    err.num = 0;
    err.file[0] = '\0';
    err.line = 0;
    err.msg[0] = '\0';
}

DBR_API void
dbr_err_perror(const char* s)
{
    dbr_log_error("%s:%d: %s: %s (%d)", err.file, err.line, s, err.msg, err.num);
}

DBR_API void
dbr_err_set_(int num, const char* file, int line, const char* msg)
{
    err.num = num;
    strncpy(err.file, file, PATH_MAX);
    err.file[PATH_MAX] = '\0';
    err.line = line;
    strncpy(err.msg, msg, DBR_ERRMSG_MAX);
    err.msg[DBR_ERRMSG_MAX] = '\0';
}

DBR_API void
dbr_err_setf_(int num, const char* file, int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    dbr_err_vsetf_(num, file, line, format, args);
    va_end(args);
}

DBR_API void
dbr_err_vsetf_(int num, const char* file, int line, const char* format, va_list args)
{
    err.num = num;
    strncpy(err.file, file, PATH_MAX);
    err.file[PATH_MAX] = '\0';
    err.line = line;
    const int ret = vsnprintf(err.msg, DBR_ERRMSG_MAX, format, args);
    // Defensive. Null termination is _not_ guaranteed by snprintf().
    err.msg[DBR_ERRMSG_MAX] = '\0';
    if (ret < 0)
        abort();
}

DBR_API void
dbr_err_print_(int num, const char* file, int line, const char* msg)
{
    dbr_err_set_(num, file, line, msg);
    dbr_log_error("%s:%d: %s (%d)", err.file, err.line, err.msg, err.num);
}

DBR_API void
dbr_err_printf_(int num, const char* file, int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    dbr_err_vsetf_(num, file, line, format, args);
    va_end(args);
    dbr_log_error("%s:%d: %s (%d)", err.file, err.line, err.msg, err.num);
}

DBR_API void
dbr_err_vprintf_(int num, const char* file, int line, const char* format, va_list args)
{
    dbr_err_vsetf_(num, file, line, format, args);
    dbr_log_error("%s:%d: %s (%d)", err.file, err.line, err.msg, err.num);
}

DBR_API int
dbr_err_num(void)
{
    return err.num;
}

DBR_API const char*
dbr_err_file(void)
{
    return err.file;
}

DBR_API int
dbr_err_line(void)
{
    return err.line;
}

DBR_API const char*
dbr_err_msg(void)
{
    return err.msg;
}
