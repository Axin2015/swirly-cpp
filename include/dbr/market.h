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
#ifndef DBR_MARKET_H
#define DBR_MARKET_H

#if !defined(DBR_HIDE_API)

#include <dbr/defs.h>
#include <dbr/types.h>

/**
 * @addtogroup Market
 * @{
 */

// Returns the market-id for state.

DBR_API DbrIden
dbr_market_id(DbrMarket market);

DBR_API DbrSide
dbr_market_bid_side(DbrMarket market);

DBR_API DbrSide
dbr_market_ask_side(DbrMarket market);

DBR_API struct DbrBest*
dbr_market_best(struct DbrRec* mrec, struct DbrBest* best);

/** @} */

#endif // !defined(DBR_HIDE_API)

#endif // DBR_MARKET_H
