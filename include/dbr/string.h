/**
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
#ifndef DBR_STRING_H
#define DBR_STRING_H

#include <dbr/types.h>
#include <dbr/util.h>

#include <stdarg.h>

struct DbrErr;

/**
 * @addtogroup String
 * @{
 */

struct DbrPriceString {
    // 10 decimal digits + 3 nulls + dp + sign + pad
    char buf[16];
    char* small;
    char* pips;
    char* big;
};

DBR_API void
dbr_format_price(double price, int price_dp, int pip_dp, struct DbrPriceString* ps);

// Not null terminated.

DBR_API char*
dbr_vformat(char* buf, const char* format, va_list args);

// Not null terminated.

DBR_API char*
dbr_format(char* buf, const char* format, ...);

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

DBR_API char*
dbr_instrcpy(char* buf, const struct DbrRec* rec);

DBR_API size_t
dbr_instrlen(const struct DbrRec* rec);

DBR_API char*
dbr_marketcpy(char* buf, struct DbrRec* rec);

DBR_API size_t
dbr_marketlen(struct DbrRec* rec);

DBR_API char*
dbr_tradercpy(char* buf, const struct DbrRec* rec);

DBR_API size_t
dbr_traderlen(const struct DbrRec* rec);

DBR_API char*
dbr_accntcpy(char* buf, const struct DbrRec* rec);

DBR_API size_t
dbr_accntlen(const struct DbrRec* rec);

DBR_API char*
dbr_sidecpy(char* buf, DbrSide side);

DBR_API size_t
dbr_sidelen(DbrSide side);

DBR_API char*
dbr_ordercpy(char* buf, const struct DbrOrder* order);

DBR_API size_t
dbr_orderlen(const struct DbrOrder* order);

DBR_API char*
dbr_tradecpy(char* buf, const struct DbrTrade* trade);

DBR_API size_t
dbr_tradelen(const struct DbrTrade* trade);

DBR_API char*
dbr_posncpy(char* buf, const struct DbrPosn* posn);

DBR_API size_t
dbr_posnlen(const struct DbrPosn* posn);

DBR_API char*
dbr_errcpy(char* buf, const struct DbrErr* err);

DBR_API size_t
dbr_errlen(const struct DbrErr* err);

/** @} */

#endif // DBR_STRING_H
