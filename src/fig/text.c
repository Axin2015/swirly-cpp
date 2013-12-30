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
#include <dbr/text.h>

#include <math.h>

static const char DIGITS[] = "0123456789";

DBR_API void
dbr_price_text(double price, int price_dp, int pip_dp, struct DbrPriceText* pt)
{
    static const char DIGITS[] = "0123456789";

    // Digits in price as integer.
    int digits = fabs(price) * pow(10, price_dp) + 0.5;
    char* cp = pt->buf + sizeof(pt->buf) - 1;

    *cp-- = '\0';
    while (price_dp > pip_dp) {
        *cp-- = DIGITS[digits % 10];
        digits /= 10;
        if (--price_dp == 0)
            *cp-- = '.';
    }
    pt->small = cp + 1;

    // Lower-order pip digit.
    *cp-- = '\0';
    *cp-- = DIGITS[digits % 10];
    digits /= 10;

    if (--price_dp == 0)
        *cp-- = '.';

    // Higher-order pip digit.
    *cp-- = DIGITS[digits % 10];
    digits /= 10;
    pt->pips = cp + 1;

    *cp-- = '\0';
    while (digits > 0 || price_dp > 0) {
        if (--price_dp == 0)
            *cp-- = '.';
        *cp-- = DIGITS[digits % 10];
        digits /= 10;
    }
    // Signedness.
    if (price < 0)
        *cp-- = '-';

    pt->big = cp + 1;
}
