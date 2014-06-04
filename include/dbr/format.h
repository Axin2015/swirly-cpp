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
#ifndef DBR_FORMAT_H
#define DBR_FORMAT_H

#include <dbr/defs.h>

#include <stdarg.h>

/**
 * @addtogroup Format
 * @{
 */

struct DbrPriceString {
    /**
     * 10 decimal digits + 3 nulls + dp + sign + pad
     */
    char buf[16];
    char* small;
    char* pips;
    char* big;
};

DBR_API void
dbr_format_price(double price, int price_dp, int pip_dp, struct DbrPriceString* ps);

DBR_API char*
dbr_vformat(char* buf, const char* format, va_list args);

DBR_API char*
dbr_format(char* buf, const char* format, ...);

/** @} */

#endif // DBR_FORMAT_H
