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

#include <dbr/side.h>

/**
 * @addtogroup Book
 * @{
 */

struct DbrBook {
    struct DbrRec* crec;
    DbrDate settl_date;
    struct DbrSide bid_side;
    struct DbrSide ask_side;
    struct DbrRbNode serv_node_;
};

struct DbrBest {
    DbrTicks bid_ticks;
    DbrLots bid_resd;
    DbrTicks ask_ticks;
    DbrLots ask_resd;
};

DBR_API void
dbr_book_init(struct DbrBook* book, struct DbrRec* crec, DbrDate settl_date, DbrPool pool);

DBR_API void
dbr_book_term(struct DbrBook* book);

static inline struct DbrSide*
dbr_book_side(struct DbrBook* book, int action)
{
    return action == DBR_BUY ? &book->bid_side : &book->ask_side;
}

static inline DbrBool
dbr_book_insert(struct DbrBook* book, struct DbrOrder* order)
{
    return dbr_side_insert_order(dbr_book_side(book, order->action), order);
}

static inline void
dbr_book_remove(struct DbrBook* book, struct DbrOrder* order)
{
    assert(order);
    dbr_side_remove_order(dbr_book_side(book, order->action), order);
}

static inline void
dbr_book_take(struct DbrBook* book, struct DbrOrder* order, DbrLots lots, DbrMillis now)
{
    dbr_side_take_order(dbr_book_side(book, order->action), order, lots, now);
}

static inline void
dbr_book_revise(struct DbrBook* book, struct DbrOrder* order, DbrLots lots, DbrMillis now)
{
    dbr_side_revise_order(dbr_book_side(book, order->action), order, lots, now);
}

static inline void
dbr_book_cancel(struct DbrBook* book, struct DbrOrder* order, DbrMillis now)
{
    assert(order);
    dbr_side_cancel_order(dbr_book_side(book, order->action), order, now);
}

static inline DbrKey
dbr_book_key(struct DbrBook* book)
{
    // Synthetic key from contract and settlment date.
    return book->crec->id * 100000000L + book->settl_date;
}

static inline struct DbrRec*
dbr_book_crec(struct DbrBook* book)
{
    return book->crec;
}

static inline DbrDate
dbr_book_settl_date(struct DbrBook* book)
{
    return book->settl_date;
}

static inline struct DbrSide*
dbr_book_bid_side(struct DbrBook* book)
{
    return &book->bid_side;
}

static inline struct DbrSide*
dbr_book_ask_side(struct DbrBook* book)
{
    return &book->ask_side;
}

DBR_API struct DbrBest*
dbr_book_best(struct DbrBook* book, struct DbrBest* best);

/** @} */

#endif // DBR_BOOK_H
