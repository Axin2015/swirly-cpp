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
#ifndef FIG_BOOK_H
#define FIG_BOOK_H

#include <dbr/side.h>

struct FigBook {
    struct DbrRec* crec;
    DbrDate settl_date;
    struct DbrSide bid_side;
    struct DbrSide ask_side;
    struct DbrList subs;
    struct DbrRbNode ctx_node_;
};

DBR_EXTERN void
fig_book_init(struct FigBook* book, DbrPool pool, struct DbrRec* crec, DbrDate settl_date);

DBR_EXTERN void
fig_book_term(struct FigBook* book);

static inline struct DbrSide*
fig_book_side(struct FigBook* book, int action)
{
    return action == DBR_BUY ? &book->bid_side : &book->ask_side;
}

static inline DbrBool
fig_book_insert(struct FigBook* book, struct DbrOrder* order)
{
    return dbr_side_insert_order(fig_book_side(book, order->action), order);
}

static inline void
fig_book_remove(struct FigBook* book, struct DbrOrder* order)
{
    assert(order);
    dbr_side_remove_order(fig_book_side(book, order->action), order);
}

static inline void
fig_book_take(struct FigBook* book, struct DbrOrder* order, DbrLots delta, DbrMillis now)
{
    dbr_side_take_order(fig_book_side(book, order->action), order, delta, now);
}

static inline DbrBool
fig_book_revise(struct FigBook* book, struct DbrOrder* order, DbrLots lots, DbrMillis now)
{
    return dbr_side_revise_order(fig_book_side(book, order->action), order, lots, now);
}

static inline void
fig_book_cancel(struct FigBook* book, struct DbrOrder* order, DbrMillis now)
{
    assert(order);
    dbr_side_cancel_order(fig_book_side(book, order->action), order, now);
}

static inline void
fig_book_sub(struct FigBook* book, struct DbrSub* sub)
{
    dbr_list_insert_back(&book->subs, &sub->book_node_);
}

static inline void
fig_book_unsub(struct DbrSub* sub)
{
    dbr_dlnode_remove(&sub->book_node_);
}

static inline DbrKey
fig_book_key(struct FigBook* book)
{
    // Synthetic key from contract and settlment date.
    return book->crec->id * 100000000L + book->settl_date;
}

static inline struct DbrRec*
fig_book_crec(struct FigBook* book)
{
    return book->crec;
}

static inline DbrDate
fig_book_settl_date(struct FigBook* book)
{
    return book->settl_date;
}

static inline struct DbrSide*
fig_book_bid_side(struct FigBook* book)
{
    return &book->bid_side;
}

static inline struct DbrSide*
fig_book_ask_side(struct FigBook* book)
{
    return &book->ask_side;
}

DBR_EXTERN struct DbrBest*
fig_book_best(DbrBook book, struct DbrBest* best);

#endif // FIG_BOOK_H
