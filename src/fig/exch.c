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
#include "accnt.h"
#include "cache.h"
#include "index.h"
#include "match.h"
#include "trader.h"

#include <dbr/book.h>
#include <dbr/exch.h>
#include <dbr/err.h>
#include <dbr/journ.h>
#include <dbr/queue.h>
#include <dbr/util.h>

#include <stdbool.h>
#include <stdlib.h> // malloc()
#include <string.h> // strncpy()

struct DbrExch_ {
    DbrJourn journ;
    DbrModel model;
    DbrPool pool;
    struct DbrTree books;
    struct FigCache cache;
    struct FigIndex index;
};

static inline struct DbrBook*
exch_book_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrBook, exch_node_, node);
}

static void
free_books(struct DbrTree* books)
{
    assert(books);
    struct DbrRbNode* node;
    while ((node = books->root)) {
        struct DbrBook* book = exch_book_entry(node);
        dbr_tree_remove(books, node);
        dbr_book_term(book);
        free(book);
    }
}

static void
free_matches(struct DbrSlNode* first, DbrPool pool)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrMatch* match = dbr_trans_match_entry(node);
        node = node->next;
        // Not committed so match object still owns the trades.
        dbr_pool_free_trade(pool, match->taker_trade);
        dbr_pool_free_trade(pool, match->maker_trade);
        dbr_pool_free_match(pool, match);
    }
}

static inline void
apply_posn(struct DbrPosn* posn, const struct DbrTrade* trade)
{
    const double licks = trade->lots * trade->ticks;
    if (trade->action == DBR_BUY) {
        posn->buy_licks += licks;
        posn->buy_lots += trade->lots;
    } else {
        assert(trade->action == DBR_SELL);
        posn->sell_licks += licks;
        posn->sell_lots += trade->lots;
    }
}

static inline DbrBool
update_order(DbrJourn journ, struct DbrOrder* order, DbrMillis now)
{
    return dbr_journ_update_order(journ, order->id, order->rev, order->status, order->resd,
                                  order->exec, order->lots, now);
}

static DbrBool
insert_trans(DbrJourn journ, const struct DbrTrans* trans, DbrMillis now)
{
    struct DbrSlNode* node = trans->first_match;
    assert(node);

    const struct DbrOrder* taker_order = trans->new_order;
    int taker_rev = taker_order->rev;
    DbrLots taker_resd = taker_order->resd;
    DbrLots taker_exec = taker_order->exec;

    do {
        const struct DbrMatch* match = dbr_trans_match_entry(node);

        // Taker revision.
        ++taker_rev;
        taker_resd -= match->lots;
        taker_exec += match->lots;

        const int taker_status = taker_resd == 0 ? DBR_FILLED : DBR_PARTIAL;
        if (!dbr_journ_update_order(journ, taker_order->id, taker_rev, taker_status,
                                    taker_resd, taker_exec, taker_order->lots, now)
            || !dbr_journ_insert_trade(journ, match->taker_trade->id,
                                       match->taker_trade->match,
                                       match->taker_trade->order,
                                       match->taker_trade->order_rev,
                                       match->taker_trade->trader.rec->id,
                                       match->taker_trade->accnt.rec->id,
                                       match->taker_trade->accnt.rec->id,
                                       match->taker_trade->settl_date,
                                       match->taker_trade->ref,
                                       match->taker_trade->cpty.rec->id,
                                       match->taker_trade->role,
                                       match->taker_trade->action,
                                       match->taker_trade->ticks,
                                       match->taker_trade->resd,
                                       match->taker_trade->exec,
                                       match->taker_trade->lots,
                                       now))
            goto fail1;

        // Maker revision.
        const struct DbrOrder* maker = match->maker_order;
        const int maker_rev = maker->rev + 1;
        const DbrLots maker_resd = maker->resd - match->lots;
        const DbrLots maker_exec = maker->exec + match->lots;
        const int maker_status = maker_resd == 0 ? DBR_FILLED : DBR_PARTIAL;
        if (!dbr_journ_update_order(journ, maker->id, maker_rev, maker_status, maker_resd,
                                    maker_exec, maker->lots, now)
            || !dbr_journ_insert_trade(journ, match->maker_trade->id,
                                       match->maker_trade->match,
                                       match->maker_trade->order,
                                       match->maker_trade->order_rev,
                                       match->maker_trade->trader.rec->id,
                                       match->maker_trade->accnt.rec->id,
                                       match->maker_trade->accnt.rec->id,
                                       match->maker_trade->settl_date,
                                       match->maker_trade->ref,
                                       match->maker_trade->cpty.rec->id,
                                       match->maker_trade->role,
                                       match->maker_trade->action,
                                       match->maker_trade->ticks,
                                       match->maker_trade->resd,
                                       match->maker_trade->exec,
                                       match->maker_trade->lots,
                                       now))
            goto fail1;

    } while ((node = node->next));
    return true;
 fail1:
    return false;
}

// Assumes that maker lots have not been reduced since matching took place.

static void
commit_result(DbrExch exch, struct DbrBook* book, const struct DbrTrans* trans, DbrMillis now,
              struct DbrResult* result)
{
    struct DbrQueue oq, tq;
    dbr_queue_init(&oq);
    dbr_queue_init(&tq);

    struct DbrOrder* taker_order = trans->new_order;
    struct DbrRec* trec = taker_order->trader.rec;

    // Must succeed because new_posn exists.
    struct FigAccnt* taker_accnt = fig_accnt_lazy(taker_order->accnt.rec, exch->pool);
    assert(taker_accnt);

    dbr_queue_insert_back(&oq, &taker_order->result_node_);

    struct DbrSlNode* node = trans->first_match;
    while (node) {

        struct DbrMatch* match = dbr_trans_match_entry(node);
        struct DbrOrder* maker_order = match->maker_order;

        // Reduce maker. Maker's revision will be incremented by this call.
        dbr_book_take(book, maker_order, match->lots, now);

        // Must succeed because maker_posn exists.
        struct FigAccnt* maker_accnt = fig_accnt_lazy(maker_order->accnt.rec, exch->pool);
        assert(maker_accnt);

        // Update taker.
        fig_accnt_emplace_trade(taker_accnt, match->taker_trade);
        apply_posn(trans->new_posn, match->taker_trade);

        // Update maker.
        fig_accnt_emplace_trade(maker_accnt, match->maker_trade);
        apply_posn(match->maker_posn, match->maker_trade);

        // Copy elements to result.

        // Maker order.
        if (maker_order->trader.rec == trec)
            dbr_queue_insert_back(&oq, &maker_order->result_node_);

        // Taker trade.
        dbr_queue_insert_back(&tq, &match->taker_trade->result_node_);

        // Maker trade.
        if (match->maker_trade->trader.rec == trec)
            dbr_queue_insert_back(&tq, &match->maker_trade->result_node_);

        // Advance node to next before current node is freed.
        node = node->next;
        dbr_pool_free_match(exch->pool, match);
    }

    result->first_order = oq.first;
    result->first_trade = tq.first;
}

static inline struct DbrRec*
get_id(DbrExch exch, int type, DbrIden id)
{
    struct DbrSlNode* node = fig_cache_find_rec_id(&exch->cache, type, id);
    assert(node != fig_cache_end_rec(&exch->cache));
    return dbr_model_rec_entry(node);
}

static inline struct DbrBook*
get_book(DbrExch exch, struct DbrRec* crec, DbrDate settl_date)
{
    assert(crec);
    assert(crec->type == DBR_CONTR);

    // Synthetic key from contract and settlment date.
    const DbrIden key = crec->id * 100000000L + settl_date;

    struct DbrBook* book;
	struct DbrRbNode* node = dbr_tree_pfind(&exch->books, key);
    if (!node || node->key != key) {
        book = malloc(sizeof(struct DbrBook));
        if (dbr_unlikely(!book)) {
            dbr_err_set(DBR_ENOMEM, "out of memory");
            return NULL;
        }
        dbr_book_init(book, crec, settl_date, exch->pool);
        struct DbrRbNode* parent = node;
        dbr_tree_pinsert(&exch->books, key, &book->exch_node_, parent);
    } else
        book = exch_book_entry(node);
    return book;
}

static DbrBool
emplace_recs(DbrExch exch, int type)
{
    struct DbrSlNode* node;
    ssize_t size = dbr_model_read_entity(exch->model, type, exch->pool, &node);
    if (size == -1)
        return false;

    fig_cache_emplace_recs(&exch->cache, type, node, size);
    return true;
}

static DbrBool
emplace_orders(DbrExch exch)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(exch->model, DBR_ORDER, exch->pool, &node) == -1)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrOrder* order = dbr_model_order_entry(node);

        // Enrich.
        order->trader.rec = get_id(exch, DBR_TRADER, order->trader.id_only);
        order->accnt.rec = get_id(exch, DBR_ACCNT, order->accnt.id_only);
        order->contr.rec = get_id(exch, DBR_CONTR, order->contr.id_only);

        struct DbrBook* book;
        if (!dbr_order_done(order)) {

            book = get_book(exch, order->contr.rec, order->settl_date);
            if (dbr_unlikely(!book))
                goto fail2;

            if (dbr_unlikely(!dbr_book_insert(book, order)))
                goto fail2;
        } else
            book = NULL;

        struct FigTrader* trader = fig_trader_lazy(order->trader.rec, &exch->index, exch->pool);
        if (dbr_unlikely(!trader)) {
            if (book)
                dbr_book_remove(book, order);
            goto fail2;
        }

        // Transfer ownership.
        fig_trader_emplace_order(trader, order);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrOrder* order = dbr_model_order_entry(node);
        node = node->next;
        dbr_pool_free_order(exch->pool, order);
    } while (node);
 fail1:
    return false;
}

static DbrBool
emplace_membs(DbrExch exch)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(exch->model, DBR_MEMB, exch->pool, &node) == -1)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrMemb* memb = dbr_model_memb_entry(node);

        // Enrich.
        memb->accnt.rec = get_id(exch, DBR_ACCNT, memb->accnt.id_only);
        memb->trader.rec = get_id(exch, DBR_TRADER, memb->trader.id_only);

        struct FigAccnt* accnt = fig_accnt_lazy(memb->accnt.rec, exch->pool);
        if (dbr_unlikely(!accnt))
            goto fail2;

        // Transfer ownership.
        fig_accnt_emplace_memb(accnt, memb);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrMemb* memb = dbr_model_memb_entry(node);
        node = node->next;
        dbr_pool_free_memb(exch->pool, memb);
    } while (node);
 fail1:
    return false;
}

static DbrBool
emplace_trades(DbrExch exch)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(exch->model, DBR_TRADE, exch->pool, &node) == -1)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrTrade* trade = dbr_model_trade_entry(node);

        // Enrich.
        trade->trader.rec = get_id(exch, DBR_TRADER, trade->trader.id_only);
        trade->accnt.rec = get_id(exch, DBR_ACCNT, trade->accnt.id_only);
        trade->contr.rec = get_id(exch, DBR_CONTR, trade->contr.id_only);
        trade->cpty.rec = get_id(exch, DBR_ACCNT, trade->cpty.id_only);

        struct FigAccnt* accnt = fig_accnt_lazy(trade->accnt.rec, exch->pool);
        if (dbr_unlikely(!accnt))
            goto fail2;

        // Transfer ownership.
        fig_accnt_emplace_trade(accnt, trade);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrTrade* trade = dbr_model_trade_entry(node);
        node = node->next;
        dbr_pool_free_trade(exch->pool, trade);
    } while (node);
 fail1:
    return false;
}

static DbrBool
emplace_posns(DbrExch exch)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(exch->model, DBR_POSN, exch->pool, &node) == -1)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrPosn* posn = dbr_model_posn_entry(node);

        // Enrich.
        posn->accnt.rec = get_id(exch, DBR_ACCNT, posn->accnt.id_only);
        posn->contr.rec = get_id(exch, DBR_CONTR, posn->contr.id_only);

        struct FigAccnt* accnt = fig_accnt_lazy(posn->accnt.rec, exch->pool);
        if (dbr_unlikely(!accnt))
            goto fail2;

        // Transfer ownership.
        fig_accnt_emplace_posn(accnt, posn);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrPosn* posn = dbr_model_posn_entry(node);
        node = node->next;
        dbr_pool_free_posn(exch->pool, posn);
    } while (node);
 fail1:
    return false;
}

DBR_API DbrExch
dbr_exch_create(DbrJourn journ, DbrModel model, DbrPool pool)
{
    DbrExch exch = malloc(sizeof(struct DbrExch_));
    if (dbr_unlikely(!exch)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    exch->journ = journ;
    exch->model = model;
    exch->pool = pool;
    dbr_tree_init(&exch->books);
    fig_cache_init(&exch->cache, pool);
    fig_index_init(&exch->index);

    // Data structures are fully initialised at this point.

    if (!emplace_recs(exch, DBR_TRADER)
        || !emplace_recs(exch, DBR_ACCNT)
        || !emplace_recs(exch, DBR_CONTR)
        || !emplace_orders(exch)
        || !emplace_membs(exch)
        || !emplace_trades(exch)
        || !emplace_posns(exch)) {
        dbr_exch_destroy(exch);
        goto fail1;
    }

    return exch;
 fail1:
    return NULL;
}

DBR_API void
dbr_exch_destroy(DbrExch exch)
{
    if (exch) {
        fig_cache_term(&exch->cache);
        free_books(&exch->books);
        free(exch);
    }
}

// Cache

DBR_API struct DbrSlNode*
dbr_exch_first_rec(DbrExch exch, int type, size_t* size)
{
    return fig_cache_first_rec(&exch->cache, type, size);
}

DBR_API struct DbrSlNode*
dbr_exch_find_rec_id(DbrExch exch, int type, DbrIden id)
{
    return fig_cache_find_rec_id(&exch->cache, type, id);
}

DBR_API struct DbrSlNode*
dbr_exch_find_rec_mnem(DbrExch exch, int type, const char* mnem)
{
    return fig_cache_find_rec_mnem(&exch->cache, type, mnem);
}

DBR_API struct DbrSlNode*
dbr_exch_end_rec(DbrExch exch)
{
    return fig_cache_end_rec(&exch->cache);
}

// Pool

DBR_API struct DbrBook*
dbr_exch_book(DbrExch exch, struct DbrRec* crec, DbrDate settl_date)
{
    return get_book(exch, crec, settl_date);
}

DBR_API DbrTrader
dbr_exch_trader(DbrExch exch, struct DbrRec* trec)
{
    return fig_trader_lazy(trec, &exch->index, exch->pool);
}

DBR_API DbrAccnt
dbr_exch_accnt(DbrExch exch, struct DbrRec* arec)
{
    return fig_accnt_lazy(arec, exch->pool);
}

// Exec

DBR_API struct DbrOrder*
dbr_exch_place(DbrExch exch, struct DbrRec* trec, struct DbrRec* arec, struct DbrBook* book,
              const char* ref, int action, DbrTicks ticks, DbrLots lots, DbrLots min,
              DbrFlags flags, struct DbrResult* result)
{
    if (lots == 0 || lots < min) {
        dbr_err_set(DBR_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    struct FigTrader* trader = fig_trader_lazy(trec, &exch->index, exch->pool);
    if (!trader)
        goto fail1;

    const DbrIden id = dbr_journ_alloc_id(exch->journ);
    struct DbrOrder* new_order = dbr_pool_alloc_order(exch->pool);
    if (!new_order)
        goto fail1;
    dbr_order_init(new_order);

    new_order->id = id;
    new_order->level = NULL;
    new_order->rev = 1;
    new_order->status = DBR_PLACED;
    new_order->trader.rec = trec;
    new_order->accnt.rec = arec;
    new_order->contr.rec = book->crec;
    new_order->settl_date = book->settl_date;
    if (ref)
        strncpy(new_order->ref, ref, DBR_REF_MAX);
    else
        new_order->ref[0] = '\0';

    new_order->action = action;
    new_order->ticks = ticks;
    new_order->resd = lots;
    new_order->exec = 0;
    new_order->lots = lots;
    new_order->min = min;
    new_order->flags = flags;
    const DbrMillis now = dbr_millis();
    new_order->created = now;
    new_order->modified = now;

    struct DbrTrans trans = { .new_order = new_order, .new_posn = NULL };

    if (!dbr_journ_begin_trans(exch->journ))
        goto fail2;

    if (!dbr_journ_insert_order(exch->journ, new_order->id, new_order->rev,
                                new_order->status, new_order->trader.rec->id,
                                new_order->accnt.rec->id, new_order->contr.rec->id,
                                new_order->settl_date, new_order->ref, new_order->action,
                                new_order->ticks, new_order->resd, new_order->exec,
                                new_order->lots, new_order->min, new_order->flags, now)
        || !fig_match_orders(book, new_order, exch->journ, exch->pool, &trans))
        goto fail3;

    if (trans.count > 0) {

        // Orders were matched.
        if (!insert_trans(exch->journ, &trans, now))
            goto fail4;

        // Commit taker order.
        new_order->rev += trans.count;
        new_order->resd -= trans.taken;
        new_order->exec += trans.taken;

        new_order->status = dbr_order_done(new_order) ? DBR_FILLED : DBR_PARTIAL;
    }

    // Place incomplete order in book.
    if (!dbr_order_done(new_order) && !dbr_book_insert(book, new_order))
        goto fail4;

    // Journal commit can still fail.
    if (!dbr_journ_commit_trans(exch->journ))
        goto fail5;

    // Final commit phase cannot fail.
    fig_trader_emplace_order(trader, new_order);
    // Commit trans to result and free matches.
    commit_result(exch, book, &trans, now, result);
    return new_order;
 fail5:
    if (!dbr_order_done(new_order))
        dbr_book_remove(book, new_order);
 fail4:
    free_matches(trans.first_match, exch->pool);
    memset(&trans, 0, sizeof(trans));
 fail3:
    dbr_journ_rollback_trans(exch->journ);
 fail2:
    dbr_pool_free_order(exch->pool, new_order);
 fail1:
    memset(&trans, 0, sizeof(trans));
    return NULL;
}

DBR_API struct DbrOrder*
dbr_exch_revise_id(DbrExch exch, DbrTrader trader, DbrIden id, DbrLots lots)
{
    struct DbrRbNode* node = fig_trader_find_order_id(trader, id);
    if (!node) {
        dbr_err_set(DBR_EINVAL, "no such order '%ld'", id);
        goto fail1;
    }

    struct DbrOrder* order = dbr_trader_order_entry(node);
    if (dbr_order_done(order)) {
        dbr_err_set(DBR_EINVAL, "order complete '%ld'", id);
        goto fail1;
    }

    // Revised lots must not be:
    // 1. less than min lots;
    // 2. less than executed lots;
    // 3. greater than original lots.

    if (lots == 0 || lots < order->min || lots < order->exec || lots > order->lots) {
        dbr_err_set(DBR_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    if (!dbr_journ_begin_trans(exch->journ))
        goto fail1;

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_update_order(exch->journ, id, order->rev + 1, DBR_REVISED, order->resd,
                                order->exec, lots, now))
        goto fail2;

    // Must succeed because order exists.
    struct DbrBook* book = get_book(exch, order->contr.rec, order->settl_date);
    assert(book);
    dbr_book_revise(book, order, lots, now);
    dbr_journ_commit_trans(exch->journ);
    return order;
 fail2:
    dbr_journ_rollback_trans(exch->journ);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_exch_revise_ref(DbrExch exch, DbrTrader trader, const char* ref, DbrLots lots)
{
    struct DbrOrder* order = fig_trader_find_order_ref(trader, ref);
    if (!order) {
        dbr_err_set(DBR_EINVAL, "no such order '%.64s'", ref);
        goto fail1;
    }

    if (dbr_order_done(order)) {
        dbr_err_set(DBR_EINVAL, "order complete '%.64s'", ref);
        goto fail1;
    }

    // Revised lots must not be:
    // 1. less than min lots;
    // 2. less than executed lots;
    // 3. greater than original lots.

    if (lots == 0 || lots < order->min || lots < order->exec || lots > order->lots) {
        dbr_err_set(DBR_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    if (!dbr_journ_begin_trans(exch->journ))
        goto fail1;

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_update_order(exch->journ, order->id, order->rev + 1, DBR_REVISED, order->resd,
                                order->exec, lots, now))
        goto fail2;

    struct DbrBook* book = get_book(exch, order->contr.rec, order->settl_date);
    assert(book);
    dbr_book_revise(book, order, lots, now);
    dbr_journ_commit_trans(exch->journ);
    return order;
 fail2:
    dbr_journ_rollback_trans(exch->journ);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_exch_cancel_id(DbrExch exch, DbrTrader trader, DbrIden id)
{
    struct DbrRbNode* node = fig_trader_find_order_id(trader, id);
    if (!node) {
        dbr_err_set(DBR_EINVAL, "no such order '%ld'", id);
        goto fail1;
    }

    struct DbrOrder* order = dbr_trader_order_entry(node);
    if (dbr_order_done(order)) {
        dbr_err_set(DBR_EINVAL, "order complete '%ld'", id);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_update_order(exch->journ, id, order->rev + 1, DBR_CANCELLED, 0,
                                order->exec, order->lots, now))
        goto fail1;

    struct DbrBook* book = get_book(exch, order->contr.rec, order->settl_date);
    assert(book);
    dbr_book_cancel(book, order, now);
    return order;
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_exch_cancel_ref(DbrExch exch, DbrTrader trader, const char* ref)
{
    struct DbrOrder* order = fig_trader_find_order_ref(trader, ref);
    if (!order) {
        dbr_err_set(DBR_EINVAL, "no such order '%.64s'", ref);
        goto fail1;
    }

    if (dbr_order_done(order)) {
        dbr_err_set(DBR_EINVAL, "order complete '%.64s'", ref);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_update_order(exch->journ, order->id, order->rev + 1, DBR_CANCELLED, 0,
                                order->exec, order->lots, now))
        goto fail1;

    struct DbrBook* book = get_book(exch, order->contr.rec, order->settl_date);
    assert(book);
    dbr_book_cancel(book, order, now);
    return order;
 fail1:
    return NULL;
}

DBR_API DbrBool
dbr_exch_archive_order(DbrExch exch, DbrTrader trader, DbrIden id)
{
    struct DbrRbNode* node = fig_trader_find_order_id(trader, id);
    if (!node) {
        dbr_err_set(DBR_EINVAL, "no such order '%ld'", id);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_archive_order(exch->journ, node->key, now))
        goto fail1;

    // No need to update timestamps on trade because it is immediately freed.

    struct DbrOrder* order = dbr_trader_order_entry(node);
    fig_trader_release_order(trader, order);
    dbr_pool_free_order(exch->pool, order);
    return true;
 fail1:
    return false;
}

DBR_API DbrBool
dbr_exch_archive_trade(DbrExch exch, DbrAccnt accnt, DbrIden id)
{
    struct DbrRbNode* node = fig_accnt_find_trade_id(accnt, id);
    if (!node) {
        dbr_err_set(DBR_EINVAL, "no such trade '%ld'", id);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_archive_trade(exch->journ, node->key, now))
        goto fail1;

    // No need to update timestamps on trade because it is immediately freed.

    struct DbrTrade* trade = dbr_accnt_trade_entry(node);
    fig_accnt_release_trade(accnt, trade);
    dbr_pool_free_trade(exch->pool, trade);
    return true;
 fail1:
    return false;
}
