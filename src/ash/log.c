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

#include <stdio.h>

static void
stdio_logger(int level, const char* format, va_list args)
{
    FILE* stream = level > DBR_WARN ? stdout : stderr;
    vfprintf(stream, format, args);
    fputc('\n', stream);
}

static __thread DbrLogger logger = stdio_logger;

DBR_API DbrLogger
dbr_log_set(DbrLogger new_logger)
{
    DbrLogger old_logger = logger;
    logger = new_logger ? new_logger : stdio_logger;
    return old_logger;
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
