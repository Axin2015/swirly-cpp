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
#ifndef DBR_PACK_H
#define DBR_PACK_H

#include <dbr/defs.h>

#include <stdarg.h>
#include <stdint.h>

#include <sys/types.h> // ssize_t

/**
 * @addtogroup Util
 * @{
 */

DBR_API char*
dbr_packi(char* buf, int i);

DBR_API char*
dbr_packl(char* buf, long l);

DBR_API const char*
dbr_unpacki(const char* buf, int* i);

DBR_API const char*
dbr_unpackl(const char* buf, long* l);

DBR_API char*
dbr_packf(char* buf, const char* format, ...);

DBR_API char*
dbr_vpackf(char* buf, const char* format, va_list args);

DBR_API const char*
dbr_unpackf(const char* buf, const char* format, ...);

DBR_API const char*
dbr_vunpackf(const char* buf, const char* format, va_list args);

/** @} */

#endif // DBR_PACK_H
