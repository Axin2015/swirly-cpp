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

    struct DbrSide* side = &book->bid_side;
    struct DbrRbNode* it = dbr_side_first_level(side);
    if (it != DBR_SIDE_END_LEVEL) {
        struct DbrLevel* level = dbr_side_level_entry(it);
        view->bid_ticks = level->ticks;
        view->bid_lots = level->lots;
        view->bid_count = level->count;
    } else {
        view->bid_ticks = 0;
        view->bid_lots = 0;
        view->bid_count = 0;
    }

    side = &book->ask_side;
    it = dbr_side_first_level(side);
    if (it != DBR_SIDE_END_LEVEL) {
        struct DbrLevel* level = dbr_side_level_entry(it);
        view->ask_ticks = level->ticks;
        view->ask_lots = level->lots;
        view->ask_count = level->count;
    } else {
        view->ask_ticks = 0;
        view->ask_lots = 0;
        view->ask_count = 0;
    }
    return view;
}
