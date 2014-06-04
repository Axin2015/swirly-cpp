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
#ifndef DBR_UTIL_H
#define DBR_UTIL_H

#include <dbr/defs.h>

#include <ctype.h>  // isspace()
#include <stddef.h> // size_t

/**
 * @addtogroup Util
 * @{
 */

DBR_API int
dbr_int_dig(int i);

DBR_API int
dbr_long_dig(long l);

static inline size_t
dbr_int_len(int i)
{
    return dbr_int_dig(i) + (i < 0 ? 1 : 0);
}

static inline size_t
dbr_long_len(long l)
{
    return dbr_long_dig(l) + (l < 0 ? 1 : 0);
}

static inline const char*
dbr_ltrim(const char* begin, const char* end)
{
    // Skip leading whitespace.
    while (begin != end && isspace(*begin))
        ++begin;
    return begin;
}

static inline const char*
dbr_rtrim(const char* begin, const char* end)
{
    // Skip trailing whitespace.
    while (begin != end && isspace(*(end - 1)))
        --end;
    return end;
}

/**
 * Milliseconds since Unix epoch.
 */

DBR_API long
dbr_millis();

/** @} */

#endif // DBR_UTIL_H
