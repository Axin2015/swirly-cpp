/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
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
#include "err.h"

#include <dbr/types.h>

#include <string.h> // strcpy()

static __thread struct {
    int num;
    char msg[DBR_ERROR_MAX + 1];
} err;

DBR_EXTERN void
elm_err_clear(void)
{
    err.num = 0;
    err.msg[0] = '\0';
}

DBR_EXTERN void
elm_err_print(FILE* stream, const char* s)
{
    fprintf(stream, "%s: %s (%d)\n", s, err.msg, err.num);
}

DBR_EXTERN void
elm_err_vset(int num, const char* format, va_list args)
{
    err.num = num;
    const int ret = vsnprintf(err.msg, DBR_ERROR_MAX, format, args);
    // Null termination is _not_ guaranteed by snprintf().
    err.msg[DBR_ERROR_MAX] = '\0';
    if (ret < 0)
        strcpy(err.msg, "bad format");
}

DBR_EXTERN void
elm_err_set(int num, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    elm_err_vset(num, format, args);
    va_end(args);
}

DBR_EXTERN int
elm_err_num(void)
{
    return err.num;
}

DBR_EXTERN const char*
elm_err_msg(void)
{
    return err.msg;
}
