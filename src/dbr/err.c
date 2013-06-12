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
#include <dbr/err.h>

#include <elm/err.h>

DBR_API void
dbr_err_clear(void)
{
    elm_err_clear();
}

DBR_API void
dbr_err_print(FILE* stream, const char* s)
{
    elm_err_print(stream, s);
}

DBR_API void
dbr_err_vset(int num, const char* format, va_list args)
{
    elm_err_vset(num, format, args);
}

DBR_API void
dbr_err_set(int num, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    elm_err_vset(num, format, args);
    va_end(args);
}

DBR_API int
dbr_err_num(void)
{
    return elm_err_num();
}

DBR_API const char*
dbr_err_msg(void)
{
    return elm_err_msg();
}
