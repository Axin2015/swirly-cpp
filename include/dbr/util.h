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
#ifndef DBR_UTIL_H
#define DBR_UTIL_H

#include <dbr/defs.h>

/**
 * @addtogroup Util
 * @{
 */

DBR_API int
dbr_intdig(int i);

DBR_API int
dbr_longdig(long l);

static inline size_t
dbr_intlen(int i)
{
    return dbr_intdig(i) + (i < 0 ? 1 : 0);
}

static inline size_t
dbr_longlen(long l)
{
    return dbr_longdig(l) + (l < 0 ? 1 : 0);
}

/**
 * @brief Milliseconds since Unix epoch.
 */

DBR_API long
dbr_millis();

/** @} */

#endif // DBR_UTIL_H
