/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_CONV_H
#define SC_ASH_CONV_H

#include <sc/elm/types.h>

#include <assert.h>
#include <math.h> // round()

/**
 * @addtogroup Conv
 * @{
 */

static inline double
sc_fract_to_real(int numer, int denom)
{
    return (double)numer / (double)denom;
}

static inline ScIncs
sc_real_to_incs(double real, double inc_size)
{
    return round(real / inc_size);
}

static inline double
sc_incs_to_real(ScIncs incs, double inc_size)
{
    return incs * inc_size;
}

/**
 * Convert quantity to lots.
 */

static inline ScLots
sc_qty_to_lots(double qty, struct ScRec* crec)
{
    assert(crec->type == SC_ENTITY_CONTR);
    return sc_real_to_incs(qty, crec->contr.qty_inc);
}

/**
 * Convert lots to quantity.
 */

static inline double
sc_lots_to_qty(ScLots lots, struct ScRec* crec)
{
    assert(crec->type == SC_ENTITY_CONTR);
    return sc_incs_to_real(lots, crec->contr.qty_inc);
}

/**
 * Convert price to ticks.
 */

static inline ScTicks
sc_price_to_ticks(double price, struct ScRec* crec)
{
    assert(crec->type == SC_ENTITY_CONTR);
    return sc_real_to_incs(price, crec->contr.price_inc);
}

/**
 * Convert ticks to price.
 */

static inline double
sc_ticks_to_price(ScTicks ticks, struct ScRec* crec)
{
    assert(crec->type == SC_ENTITY_CONTR);
    return sc_incs_to_real(ticks, crec->contr.price_inc);
}

/**
 * Number of decimal places in real.
 */

static inline int
sc_real_to_dp(double d)
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
sc_dp_to_real(int dp)
{
    return pow(10, -dp);
}

/** @} */

#endif // SC_ASH_CONV_H
