/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/fig/book.h>

#include <sc/ash/err.h>

#include <stdlib.h>

SC_API void
sc_book_init(struct ScBook* book, struct ScRec* crec, ScJd settl_day, ScPool pool)
{
    book->crec = crec;
    book->settl_day = settl_day;
    sc_side_init(&book->bid_side, pool);
    sc_side_init(&book->offer_side, pool);
    sc_rbnode_init(&book->serv_node_);
    sc_rbnode_init(&book->update_node_);
}

SC_API void
sc_book_term(struct ScBook* book)
{
    sc_side_term(&book->bid_side);
    sc_side_term(&book->offer_side);
}

SC_API struct ScView*
sc_book_view(struct ScBook* book, struct ScView* view, ScMillis now)
{
    view->contr.rec = book->crec;
    view->settl_day = book->settl_day;

    struct ScSide* bid_side = &book->bid_side;
    struct ScSide* offer_side = &book->offer_side;

    struct ScRbNode* bid_it = sc_side_first_level(bid_side);
    struct ScRbNode* offer_it = sc_side_first_level(offer_side);

    for (size_t i = 0; i < SC_LEVEL_MAX; ++i) {
        if (bid_it != SC_SIDE_END_LEVEL) {
            struct ScLevel* level = sc_side_level_entry(bid_it);
            view->bid_ticks[i] = level->ticks;
            view->bid_lots[i] = level->lots;
            view->bid_count[i] = level->count;
            bid_it = sc_rbnode_next(bid_it);
        } else {
            view->bid_ticks[i] = 0;
            view->bid_lots[i] = 0;
            view->bid_count[i] = 0;
        }
        if (offer_it != SC_SIDE_END_LEVEL) {
            struct ScLevel* level = sc_side_level_entry(offer_it);
            view->offer_ticks[i] = level->ticks;
            view->offer_lots[i] = level->lots;
            view->offer_count[i] = level->count;
            offer_it = sc_rbnode_next(offer_it);
        } else {
            view->offer_ticks[i] = 0;
            view->offer_lots[i] = 0;
            view->offer_count[i] = 0;
        }
    }

    view->created = now;
    return view;
}
