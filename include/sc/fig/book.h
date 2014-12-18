/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_FIG_BOOK_H
#define SC_FIG_BOOK_H

#include <sc/elm/side.h>

/**
 * @addtogroup Book
 * @{
 */

SC_API void
sc_book_init(struct ScBook* book, struct ScRec* crec, ScJd settl_day, ScPool pool);

SC_API void
sc_book_term(struct ScBook* book);

static inline struct ScSide*
sc_book_side(struct ScBook* book, int action)
{
    return action == SC_ACTION_BUY ? &book->bid_side : &book->offer_side;
}

static inline ScBool
sc_book_insert(struct ScBook* book, struct ScOrder* order)
{
    return sc_side_insert_order(sc_book_side(book, order->i.action), order);
}

static inline void
sc_book_remove(struct ScBook* book, struct ScOrder* order)
{
    assert(order);
    sc_side_remove_order(sc_book_side(book, order->i.action), order);
}

static inline void
sc_book_take(struct ScBook* book, struct ScOrder* order, ScLots lots, ScMillis now)
{
    sc_side_take_order(sc_book_side(book, order->i.action), order, lots, now);
}

static inline void
sc_book_revise(struct ScBook* book, struct ScOrder* order, ScLots lots, ScMillis now)
{
    sc_side_revise_order(sc_book_side(book, order->i.action), order, lots, now);
}

static inline void
sc_book_cancel(struct ScBook* book, struct ScOrder* order, ScMillis now)
{
    assert(order);
    sc_side_cancel_order(sc_book_side(book, order->i.action), order, now);
}

static inline struct ScRec*
sc_book_crec(struct ScBook* book)
{
    return book->crec;
}

static inline ScJd
sc_book_settl_day(struct ScBook* book)
{
    return book->settl_day;
}

static inline struct ScSide*
sc_book_bid_side(struct ScBook* book)
{
    return &book->bid_side;
}

static inline struct ScSide*
sc_book_offer_side(struct ScBook* book)
{
    return &book->offer_side;
}

SC_API struct ScView*
sc_book_view(struct ScBook* book, struct ScView* view, ScMillis now);

/** @} */

#endif // SC_FIG_BOOK_H
