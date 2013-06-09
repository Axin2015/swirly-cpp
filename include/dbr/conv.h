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
#ifndef DBR_CONV_H
#define DBR_CONV_H

#include <dbr/defs.h>
#include <dbr/types.h>

#include <assert.h>
#include <math.h> // round()

/**
 * @addtogroup Rec
 * @{
 */

static inline struct DbrRec*
dbr_rec_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrRec, model_node_, node);
}

/** @} */

/**
 * @addtogroup MarketSub
 * @{
 */

/**
 * @brief Subscription from market node.
 */

static inline struct DbrSub*
dbr_market_sub_entry(struct DbrDlNode* node)
{
    return dbr_implof(struct DbrSub, market_node_, node);
}

/** @} */

/**
 * @addtogroup SideLevel
 * @{
 */

static inline struct DbrLevel*
dbr_side_level_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrLevel, side_node_, node);
}

/** @} */

/**
 * @addtogroup SideOrder
 * @{
 */

static inline struct DbrOrder*
dbr_side_order_entry(struct DbrDlNode* node)
{
    return dbr_implof(struct DbrOrder, side_node_, node);
}

/** @} */

/**
 * @private
 */

static inline struct DbrOrder*
dbr_order_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrOrder, model_node_, node);
}

/**
 * @addtogroup Exec
 * @{
 */

static inline DbrBool
dbr_order_done(const struct DbrOrder* order)
{
    return order->resd == 0;
}

/** @} */

/**
 * @addtogroup Trans
 * @{
 */

static inline struct DbrMatch*
dbr_trans_match_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrMatch, trans_node_, node);
}

/** @} */

/**
 * @private
 */

static inline struct DbrMemb*
dbr_memb_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrMemb, model_node_, node);
}

/**
 * @addtogroup AccntMemb
 * @{
 */

static inline struct DbrMemb*
dbr_accnt_memb_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrMemb, accnt_node_, node);
}

/** @} */

/**
 * @private
 */

static inline struct DbrPosn*
dbr_posn_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrPosn, model_node_, node);
}

/**
 * @addtogroup AccntPosn
 * @{
 */

static inline struct DbrPosn*
dbr_accnt_posn_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrPosn, accnt_node_, node);
}

/** @} */

/**
 * @private
 */

static inline struct DbrTrade*
dbr_trade_entry(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrTrade, model_node_, node);
}

/**
 * @addtogroup AccntTrade
 * @{
 */

static inline struct DbrTrade*
dbr_accnt_trade_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrTrade, accnt_node_, node);
}

/** @} */

/**
 * @addtogroup TraderOrder
 * @{
 */

static inline struct DbrOrder*
dbr_trader_order_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrOrder, trader_node_, node);
}

/** @} */

/**
 * @addtogroup TraderSub
 * @{
 */

/**
 * @brief Subscription from trader node.
 */

static inline struct DbrSub*
dbr_trader_sub_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrSub, trader_node_, node);
}

/** @} */

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
 * @brief Convert quantity to lots.
 */

static inline DbrLots
dbr_qty_to_lots(double qty, struct DbrRec* irec)
{
    assert(irec->type == DBR_INSTR);
    return dbr_real_to_incs(qty, irec->instr.qty_inc);
}

/**
 * @brief Convert lots to quantity.
 */

static inline double
dbr_lots_to_qty(DbrLots lots, struct DbrRec* irec)
{
    assert(irec->type == DBR_INSTR);
    return dbr_incs_to_real(lots, irec->instr.qty_inc);
}

/**
 * @brief Convert price to ticks.
 */

static inline DbrTicks
dbr_price_to_ticks(double price, struct DbrRec* irec)
{
    assert(irec->type == DBR_INSTR);
    return dbr_real_to_incs(price, irec->instr.price_inc);
}

/**
 * @brief Convert ticks to price.
 */

static inline double
dbr_ticks_to_price(DbrTicks ticks, struct DbrRec* irec)
{
    assert(irec->type == DBR_INSTR);
    return dbr_incs_to_real(ticks, irec->instr.price_inc);
}

/**
 * @brief Number of decimal places in real.
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
 * @brief Decimal places as real.
 */

static inline double
dbr_dp_to_real(int dp)
{
    return pow(10, -dp);
}

/** @} */

#endif // DBR_CONV_H
