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
#include <dbr/book.h>

#include <dbr/err.h>

#include <stdlib.h>

DBR_API void
dbr_book_init(struct DbrBook* book, struct DbrRec* crec, DbrJd settl_day, DbrPool pool)
{
    book->crec = crec;
    book->settl_day = settl_day;
    dbr_side_init(&book->bid_side, pool);
    dbr_side_init(&book->offer_side, pool);
    dbr_rbnode_init(&book->serv_node_);
    dbr_rbnode_init(&book->update_node_);
}

DBR_API void
dbr_book_term(struct DbrBook* book)
{
    dbr_side_term(&book->bid_side);
    dbr_side_term(&book->offer_side);
}

DBR_API struct DbrView*
dbr_book_view(struct DbrBook* book, struct DbrView* view, DbrMillis now)
{
    view->contr.rec = book->crec;
    view->settl_day = book->settl_day;

    struct DbrSide* bid_side = &book->bid_side;
    struct DbrSide* offer_side = &book->offer_side;

    struct DbrRbNode* bid_it = dbr_side_first_level(bid_side);
    struct DbrRbNode* offer_it = dbr_side_first_level(offer_side);

    for (size_t i = 0; i < DBR_LEVEL_MAX; ++i) {
        if (bid_it != DBR_SIDE_END_LEVEL) {
            struct DbrLevel* level = dbr_side_level_entry(bid_it);
            view->bid_ticks[i] = level->ticks;
            view->bid_lots[i] = level->lots;
            view->bid_count[i] = level->count;
            bid_it = dbr_rbnode_next(bid_it);
        } else {
            view->bid_ticks[i] = 0;
            view->bid_lots[i] = 0;
            view->bid_count[i] = 0;
        }
        if (offer_it != DBR_SIDE_END_LEVEL) {
            struct DbrLevel* level = dbr_side_level_entry(offer_it);
            view->offer_ticks[i] = level->ticks;
            view->offer_lots[i] = level->lots;
            view->offer_count[i] = level->count;
            offer_it = dbr_rbnode_next(offer_it);
        } else {
            view->offer_ticks[i] = 0;
            view->offer_lots[i] = 0;
            view->offer_count[i] = 0;
        }
    }

    view->created = now;
    return view;
}
