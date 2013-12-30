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
#ifndef DBR_CONV_H
#define DBR_CONV_H

#include <dbr/defs.h>
#include <dbr/types.h>

#include <assert.h>
#include <math.h> // round()

/**
 * @addtogroup Conv
 * @{
 */

static inline double
dbr_fract_to_real(int numer, int denom)
{
    return (double)numer / (double)denom;
}

static inline DbrIncs
dbr_real_to_incs(double real, double inc_size)
{
    return round(real / inc_size);
}

static inline double
dbr_incs_to_real(DbrIncs incs, double inc_size)
{
    return incs * inc_size;
}

/**
 * Convert quantity to lots.
 */

static inline DbrLots
dbr_qty_to_lots(double qty, struct DbrRec* crec)
{
    assert(crec->type == DBR_CONTR);
    return dbr_real_to_incs(qty, crec->contr.qty_inc);
}

/**
 * Convert lots to quantity.
 */

static inline double
dbr_lots_to_qty(DbrLots lots, struct DbrRec* crec)
{
    assert(crec->type == DBR_CONTR);
    return dbr_incs_to_real(lots, crec->contr.qty_inc);
}

/**
 * Convert price to ticks.
 */

static inline DbrTicks
dbr_price_to_ticks(double price, struct DbrRec* crec)
{
    assert(crec->type == DBR_CONTR);
    return dbr_real_to_incs(price, crec->contr.price_inc);
}

/**
 * Convert ticks to price.
 */

static inline double
dbr_ticks_to_price(DbrTicks ticks, struct DbrRec* crec)
{
    assert(crec->type == DBR_CONTR);
    return dbr_incs_to_real(ticks, crec->contr.price_inc);
}

/**
 * Number of decimal places in real.
 */

static inline int
dbr_real_to_dp(double d)
{
    int dp;
    for (dp = 0; dp < 9; ++dp) {
        double ip;
        const double fp = modf(d, &ip);
        if (fp < 0.000000001)
            break;
        d *= 10;
    }
    return dp;
}

/**
 * Decimal places as real.
 */

static inline double
dbr_dp_to_real(int dp)
{
    return pow(10, -dp);
}

/**
 * Synthetic book key from contract and settlment date.
 */

static inline DbrKey
dbr_book_key(DbrIden cid, DbrDate settl_date)
{
    return cid * 100000000L + settl_date;
}

/** @} */

#endif // DBR_CONV_H
