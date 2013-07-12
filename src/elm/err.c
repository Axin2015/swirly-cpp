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

#include <ash/err.h>

DBR_API void
dbr_err_clear(void)
{
    ash_err_clear();
}

DBR_API void
dbr_err_print(FILE* stream, const char* s)
{
    ash_err_print(stream, s);
}

DBR_API void
dbr_err_vset_(int num, const char* file, int line, const char* format, va_list args)
{
    ash_err_vset_(num, file, line, format, args);
}

DBR_API void
dbr_err_set_(int num, const char* file, int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    ash_err_vset_(num, file, line, format, args);
    va_end(args);
}

DBR_API int
dbr_err_num(void)
{
    return ash_err_num();
}

DBR_API const char*
dbr_err_file(void)
{
    return ash_err_file();
}

DBR_API int
dbr_err_line(void)
{
    return ash_err_line();
}

DBR_API const char*
dbr_err_msg(void)
{
    return ash_err_msg();
}
