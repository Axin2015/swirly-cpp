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
#include <dbr/log.h>

#include <assert.h>
#include <stdio.h>
#include <syslog.h>

static void
stdio_logger(int level, const char* format, va_list args)
{
    FILE* stream = level > DBR_LOG_WARN ? stdout : stderr;
    flockfile(stream);
    vfprintf(stream, format, args);
    fputc('\n', stream);
    funlockfile(stream);
}

static void
syslog_logger(int level, const char* format, va_list args)
{
    int priority;
    switch (level) {
    case DBR_LOG_CRIT:
        priority = LOG_CRIT;
        break;
    case DBR_LOG_ERROR:
        priority = LOG_ERR;
        break;
    case DBR_LOG_WARN:
        priority = LOG_WARNING;
        break;
    case DBR_LOG_NOTICE:
        priority = LOG_NOTICE;
        break;
    case DBR_LOG_INFO:
        priority = LOG_INFO;
        break;
    default:
        priority = LOG_DEBUG;
    }
    vsyslog(priority, format, args);
}

static __thread DbrLogger logger = stdio_logger;

DBR_API DbrLogger
dbr_log_setlogger(DbrLogger new_logger)
{
    assert(new_logger);
    DbrLogger prev = logger;
    logger = new_logger;
    return prev;
}

DBR_API DbrLogger
dbr_log_stdio()
{
    return stdio_logger;
}

DBR_API DbrLogger
dbr_log_syslog()
{
    return syslog_logger;
}

DBR_API void
dbr_log_printf(int level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    logger(level, format, args);
    va_end(args);
}

DBR_API void
dbr_log_vprintf(int level, const char* format, va_list args)
{
    logger(level, format, args);
}
