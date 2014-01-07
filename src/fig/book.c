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
dbr_book_init(struct DbrBook* book, struct DbrRec* crec, DbrDate settl_date, DbrPool pool)
{
    book->crec = crec;
    book->settl_date = settl_date;
    dbr_side_init(&book->bid_side, pool);
    dbr_side_init(&book->ask_side, pool);
    dbr_rbnode_init(&book->serv_node_);
    dbr_rbnode_init(&book->update_node_);
}

DBR_API void
dbr_book_term(struct DbrBook* book)
{
    dbr_side_term(&book->bid_side);
    dbr_side_term(&book->ask_side);
}

DBR_API struct DbrView*
dbr_book_view(struct DbrBook* book, struct DbrView* view)
{
    view->contr.rec = book->crec;
    view->settl_date = book->settl_date;

    struct DbrSide* bid_side = &book->bid_side;
    struct DbrSide* ask_side = &book->ask_side;

    struct DbrRbNode* bid_it = dbr_side_first_level(bid_side);
    struct DbrRbNode* ask_it = dbr_side_first_level(ask_side);

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
        if (ask_it != DBR_SIDE_END_LEVEL) {
            struct DbrLevel* level = dbr_side_level_entry(ask_it);
            view->ask_ticks[i] = level->ticks;
            view->ask_lots[i] = level->lots;
            view->ask_count[i] = level->count;
            ask_it = dbr_rbnode_next(ask_it);
        } else {
            view->ask_ticks[i] = 0;
            view->ask_lots[i] = 0;
            view->ask_count[i] = 0;
        }
    }

    return view;
}
