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
#ifndef ASH_ERR_H
#define ASH_ERR_H

#include <dbr/defs.h>

#include <stdarg.h> // va_list
#include <stdio.h>

DBR_EXTERN void
ash_err_clear(void);

DBR_EXTERN void
ash_err_print(FILE* stream, const char* s);

DBR_EXTERN void
ash_err_vset(int num, const char* format, va_list args);

DBR_EXTERN void
ash_err_set(int num, const char* format, ...);

DBR_EXTERN int
ash_err_num(void);

DBR_EXTERN const char*
ash_err_msg(void);

#endif // ASH_ERR_H
