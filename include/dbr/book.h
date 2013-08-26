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
#ifndef DBR_BOOK_H
#define DBR_BOOK_H

#include <dbr/defs.h>
#include <dbr/types.h>

/**
 * @addtogroup Book
 * @{
 */

DBR_API DbrKey
dbr_book_key(DbrBook book);

DBR_API struct DbrRec*
dbr_book_crec(DbrBook book);

DBR_API DbrDate
dbr_book_settl_date(DbrBook book);

DBR_API DbrSide
dbr_book_bid_side(DbrBook book);

DBR_API DbrSide
dbr_book_ask_side(DbrBook book);

DBR_API struct DbrBest*
dbr_book_best(struct DbrRec* mrec, struct DbrBest* best);

/** @} */

#endif // DBR_BOOK_H
