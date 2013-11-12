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
#include <dbr/err.h>
#include <dbr/serv.h>
#include <dbr/journ.h>
#include <dbr/queue.h>
#include <dbr/util.h>

#include <stdbool.h>
#include <stdlib.h> // malloc()
#include <string.h> // strncpy()

struct FigServ {
    DbrJourn journ;
    DbrModel model;
    DbrPool pool;
    struct FigCache cache;
    struct DbrTree books;
    struct FigIndex index;
};

static inline struct DbrBook*
serv_book_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct DbrBook, serv_node_, node);
}

static void
term_state(struct DbrRec* rec)
{
    switch (rec->type) {
    case DBR_TRADER:
        fig_trader_term(rec);
        break;
    case DBR_ACCNT:
        fig_accnt_term(rec);
        break;
    }
}

static inline struct DbrRec*
get_id(struct FigCache* cache, int type, DbrIden id)
{
    struct DbrSlNode* node = fig_cache_find_rec_id(cache, type, id);
    assert(node != FIG_CACHE_END_REC);
    return dbr_rec_entry(node);
}

static inline struct DbrOrder*
enrich_order(struct FigCache* cache, struct DbrOrder* order)
{
    order->trader.rec = get_id(cache, DBR_TRADER, order->trader.id_only);
    order->accnt.rec = get_id(cache, DBR_ACCNT, order->accnt.id_only);
    order->contr.rec = get_id(cache, DBR_CONTR, order->contr.id_only);
    return order;
}

static inline struct DbrExec*
enrich_trade(struct FigCache* cache, struct DbrExec* exec)
{
    exec->trader.rec = get_id(cache, DBR_TRADER, exec->trader.id_only);
    exec->accnt.rec = get_id(cache, DBR_ACCNT, exec->accnt.id_only);
    exec->contr.rec = get_id(cache, DBR_CONTR, exec->contr.id_only);
    exec->cpty.rec = get_id(cache, DBR_ACCNT, exec->cpty.id_only);
    return exec;
}

static inline struct DbrMemb*
enrich_memb(struct FigCache* cache, struct DbrMemb* memb)
{
    memb->trader.rec = get_id(cache, DBR_TRADER, memb->trader.id_only);
    memb->accnt.rec = get_id(cache, DBR_ACCNT, memb->accnt.id_only);
    return memb;
}

static inline struct DbrPosn*
enrich_posn(struct FigCache* cache, struct DbrPosn* posn)
{
    posn->accnt.rec = get_id(cache, DBR_ACCNT, posn->accnt.id_only);
    posn->contr.rec = get_id(cache, DBR_CONTR, posn->contr.id_only);
    return posn;
}

static inline struct DbrBook*
get_book(DbrServ serv, struct DbrRec* crec, DbrDate settl_date)
{
    assert(crec);
    assert(crec->type == DBR_CONTR);

    // Synthetic key from contract and settlment date.
    const DbrIden key = crec->id * 100000000L + settl_date;

    struct DbrBook* book;
	struct DbrRbNode* node = dbr_tree_pfind(&serv->books, key);
    if (!node || node->key != key) {
        book = malloc(sizeof(struct DbrBook));
        if (dbr_unlikely(!book)) {
            dbr_err_set(DBR_ENOMEM, "out of memory");
            return NULL;
        }
        dbr_book_init(book, crec, settl_date, serv->pool);
        struct DbrRbNode* parent = node;
        dbr_tree_pinsert(&serv->books, key, &book->serv_node_, parent);
    } else
        book = serv_book_entry(node);
    return book;
}

static void
free_books(struct DbrTree* books)
{
    assert(books);
    struct DbrRbNode* node;
    while ((node = books->root)) {
        struct DbrBook* book = serv_book_entry(node);
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
        dbr_pool_free_exec(pool, match->taker_exec);
        dbr_pool_free_exec(pool, match->maker_exec);
        dbr_pool_free_match(pool, match);
    }
}

static inline void
apply_posn(struct DbrPosn* posn, const struct DbrExec* exec)
{
    const double licks = exec->lots * exec->ticks;
    if (exec->action == DBR_BUY) {
        posn->buy_licks += licks;
        posn->buy_lots += exec->lots;
    } else {
        assert(exec->action == DBR_SELL);
        posn->sell_licks += licks;
        posn->sell_lots += exec->lots;
    }
}

static inline DbrBool
update_order(DbrJourn journ, struct DbrOrder* order, DbrMillis now)
{
    return dbr_journ_update_order(journ, order->id, order->rev, order->status, order->lots,
                                  order->resd, order->exec, order->last_ticks, order->last_lots,
                                  now);
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

        if (!dbr_journ_update_order(journ, taker_order->id, taker_rev, DBR_TRADED,
                                    taker_order->lots, taker_resd, taker_exec,
                                    match->ticks, match->lots, now)
            || !dbr_journ_insert_trade(journ, match->taker_exec->id,
                                       match->taker_exec->order,
                                       match->taker_exec->rev,
                                       match->taker_exec->match,
                                       match->taker_exec->role,
                                       match->taker_exec->cpty.rec->id,
                                       now))
            goto fail1;

        // Maker revision.
        const struct DbrOrder* maker = match->maker_order;
        const int maker_rev = maker->rev + 1;
        const DbrLots maker_resd = maker->resd - match->lots;
        const DbrLots maker_exec = maker->exec + match->lots;
        if (!dbr_journ_update_order(journ, maker->id, maker_rev, DBR_TRADED,
                                    maker->lots, maker_resd, maker_exec,
                                    match->ticks, match->lots, now)
            || !dbr_journ_insert_trade(journ, match->maker_exec->id,
                                       match->maker_exec->order,
                                       match->maker_exec->rev,
                                       match->maker_exec->match,
                                       match->maker_exec->role,
                                       match->maker_exec->cpty.rec->id,
                                       now))
            goto fail1;

    } while ((node = node->next));
    return true;
 fail1:
    return false;
}

static void
insert_unique_posn(struct DbrQueue* queue, struct DbrPosn* new_posn)
{
    for (struct DbrSlNode* node = queue->first; node; node = node->next) {
        struct DbrPosn* posn = dbr_result_posn_entry(node);
        if (posn->accnt.rec->id == new_posn->accnt.rec->id
            && posn->contr.rec->id == new_posn->contr.rec->id
            && posn->settl_date == new_posn->settl_date)
            return; // Exists.
    }
    dbr_queue_insert_back(queue, &new_posn->result_node_);
}

// Assumes that maker lots have not been reduced since matching took place.

static void
commit_result(DbrServ serv, struct FigTrader* taker, struct DbrBook* book,
              const struct DbrTrans* trans, DbrMillis now, struct DbrResult* result)
{
    struct DbrQueue pq, tq;
    dbr_queue_init(&pq);
    dbr_queue_init(&tq);

    if (trans->new_posn)
        dbr_queue_insert_back(&pq, &trans->new_posn->result_node_);

    struct DbrSlNode* node = trans->first_match;
    while (node) {

        struct DbrMatch* match = dbr_trans_match_entry(node);
        struct DbrOrder* maker_order = match->maker_order;

        // Reduce maker. Maker's revision will be incremented by this call.
        dbr_book_take(book, maker_order, match->lots, now);
        // Insert maker accnt if taker is a member.
        if (fig_trader_find_memb_id(taker, match->maker_posn->accnt.rec->id) == DBR_TRADER_END_MEMB)
            insert_unique_posn(&pq, match->maker_posn);

        // Must succeed because maker order exists.
        struct FigTrader* maker = fig_trader_lazy(maker_order->trader.rec, &serv->index,
                                                  serv->pool);
        assert(maker);

        // Maker updated first because this is consistent with last-look semantics.

        // Update maker.
        fig_trader_emplace_trade(maker, match->maker_exec);
        apply_posn(match->maker_posn, match->maker_exec);
        dbr_queue_insert_back(&tq, &match->maker_exec->result_node_);

        // Update taker.
        fig_trader_emplace_trade(taker, match->taker_exec);
        apply_posn(trans->new_posn, match->taker_exec);
        dbr_queue_insert_back(&tq, &match->taker_exec->result_node_);

        // Advance node to next before current node is freed.
        node = node->next;
        dbr_pool_free_match(serv->pool, match);
    }

    result->new_order = trans->new_order;
    result->first_posn = pq.first;
    result->first_trade = tq.first;
}

static DbrBool
emplace_recs(DbrServ serv, int type)
{
    struct DbrSlNode* node;
    ssize_t size = dbr_model_read_entity(serv->model, type, serv->pool, &node);
    if (size < 0)
        return false;

    fig_cache_emplace_recs(&serv->cache, type, node, size);
    return true;
}

static DbrBool
emplace_orders(DbrServ serv)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(serv->model, DBR_ORDER, serv->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrOrder* order = enrich_order(&serv->cache, dbr_order_entry(node));
        struct DbrBook* book;
        if (!dbr_order_done(order)) {

            book = get_book(serv, order->contr.rec, order->settl_date);
            if (dbr_unlikely(!book))
                goto fail2;

            if (dbr_unlikely(!dbr_book_insert(book, order)))
                goto fail2;
        } else
            book = NULL;

        struct FigTrader* trader = fig_trader_lazy(order->trader.rec, &serv->index, serv->pool);
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
        struct DbrOrder* order = dbr_order_entry(node);
        node = node->next;
        dbr_pool_free_order(serv->pool, order);
    } while (node);
 fail1:
    return false;
}

static DbrBool
emplace_trades(DbrServ serv)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(serv->model, DBR_TRADE, serv->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrExec* exec = enrich_trade(&serv->cache, dbr_exec_entry(node));
        struct FigTrader* trader = fig_trader_lazy(exec->trader.rec, &serv->index, serv->pool);
        if (dbr_unlikely(!trader))
            goto fail2;

        // Transfer ownership.
        fig_trader_emplace_trade(trader, exec);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrExec* exec = dbr_exec_entry(node);
        node = node->next;
        dbr_pool_free_exec(serv->pool, exec);
    } while (node);
 fail1:
    return false;
}

static DbrBool
emplace_membs(DbrServ serv)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(serv->model, DBR_MEMB, serv->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrMemb* memb = enrich_memb(&serv->cache, dbr_memb_entry(node));
        struct FigTrader* trader = fig_trader_lazy(memb->trader.rec, &serv->index, serv->pool);
        if (dbr_unlikely(!trader))
            goto fail2;

        // Transfer ownership.
        fig_trader_emplace_memb(trader, memb);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrMemb* memb = dbr_memb_entry(node);
        node = node->next;
        dbr_pool_free_memb(serv->pool, memb);
    } while (node);
 fail1:
    return false;
}

static DbrBool
emplace_posns(DbrServ serv)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(serv->model, DBR_POSN, serv->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrPosn* posn = enrich_posn(&serv->cache, dbr_posn_entry(node));
        struct FigAccnt* accnt = fig_accnt_lazy(posn->accnt.rec, serv->pool);
        if (dbr_unlikely(!accnt))
            goto fail2;

        // Transfer ownership.
        fig_accnt_emplace_posn(accnt, posn);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrPosn* posn = dbr_posn_entry(node);
        node = node->next;
        dbr_pool_free_posn(serv->pool, posn);
    } while (node);
 fail1:
    return false;
}

DBR_API DbrServ
dbr_serv_create(DbrJourn journ, DbrModel model, DbrPool pool)
{
    DbrServ serv = malloc(sizeof(struct FigServ));
    if (dbr_unlikely(!serv)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    serv->journ = journ;
    serv->model = model;
    serv->pool = pool;
    fig_cache_init(&serv->cache, term_state, pool);
    dbr_tree_init(&serv->books);
    fig_index_init(&serv->index);

    // Data structures are fully initialised at this point.

    if (!emplace_recs(serv, DBR_TRADER)
        || !emplace_recs(serv, DBR_ACCNT)
        || !emplace_recs(serv, DBR_CONTR)
        || !emplace_orders(serv)
        || !emplace_trades(serv)
        || !emplace_membs(serv)
        || !emplace_posns(serv)) {
        // Use destroy since fully initialised.
        dbr_serv_destroy(serv);
        goto fail1;
    }

    return serv;
 fail1:
    return NULL;
}

DBR_API void
dbr_serv_destroy(DbrServ serv)
{
    if (serv) {
        free_books(&serv->books);
        fig_cache_term(&serv->cache);
        free(serv);
    }
}

// Cache

DBR_API struct DbrSlNode*
dbr_serv_first_rec(DbrServ serv, int type, size_t* size)
{
    return fig_cache_first_rec(&serv->cache, type, size);
}

DBR_API struct DbrSlNode*
dbr_serv_find_rec_id(DbrServ serv, int type, DbrIden id)
{
    return fig_cache_find_rec_id(&serv->cache, type, id);
}

DBR_API struct DbrSlNode*
dbr_serv_find_rec_mnem(DbrServ serv, int type, const char* mnem)
{
    return fig_cache_find_rec_mnem(&serv->cache, type, mnem);
}

// Pool

DBR_API DbrTrader
dbr_serv_trader(DbrServ serv, struct DbrRec* trec)
{
    return fig_trader_lazy(trec, &serv->index, serv->pool);
}

DBR_API DbrAccnt
dbr_serv_accnt(DbrServ serv, struct DbrRec* arec)
{
    return fig_accnt_lazy(arec, serv->pool);
}

DBR_API struct DbrBook*
dbr_serv_book(DbrServ serv, struct DbrRec* crec, DbrDate settl_date)
{
    return get_book(serv, crec, settl_date);
}

// Exec

DBR_API struct DbrOrder*
dbr_serv_place(DbrServ serv, DbrTrader trader, DbrAccnt accnt, struct DbrBook* book,
              const char* ref, int action, DbrTicks ticks, DbrLots lots, DbrLots min,
              DbrFlags flags, struct DbrResult* result)
{
    if (lots == 0 || lots < min) {
        dbr_err_setf(DBR_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    const DbrIden id = dbr_journ_alloc_id(serv->journ);
    struct DbrOrder* new_order = dbr_pool_alloc_order(serv->pool);
    if (!new_order)
        goto fail1;
    dbr_order_init(new_order);

    new_order->level = NULL;
    new_order->id = id;
    new_order->rev = 1;
    new_order->status = DBR_NEW;
    new_order->trader.rec = fig_trader_rec(trader);
    new_order->accnt.rec = fig_accnt_rec(accnt);
    new_order->contr.rec = book->crec;
    new_order->settl_date = book->settl_date;
    if (ref)
        strncpy(new_order->ref, ref, DBR_REF_MAX);
    else
        new_order->ref[0] = '\0';

    new_order->action = action;
    new_order->ticks = ticks;
    new_order->lots = lots;
    new_order->resd = lots;
    new_order->exec = 0;
    new_order->last_ticks = 0;
    new_order->last_lots = 0;
    new_order->min = min;
    new_order->flags = flags;
    const DbrMillis now = dbr_millis();
    new_order->created = now;
    new_order->modified = now;

    struct DbrTrans trans;
    dbr_trans_init(&trans);
    trans.new_order = new_order;

    if (!dbr_journ_begin_trans(serv->journ))
        goto fail2;

    if (!dbr_journ_insert_order(serv->journ, new_order->id, new_order->rev,
                                new_order->status, new_order->trader.rec->id,
                                new_order->accnt.rec->id, new_order->contr.rec->id,
                                new_order->settl_date, new_order->ref, new_order->action,
                                new_order->ticks, new_order->lots, new_order->resd,
                                new_order->exec, new_order->last_ticks, new_order->last_lots,
                                new_order->min, new_order->flags, now)
        || !fig_match_orders(book, new_order, serv->journ, serv->pool, &trans))
        goto fail3;

    if (trans.count > 0) {

        // Orders were matched.
        if (!insert_trans(serv->journ, &trans, now))
            goto fail4;

        // TODO: IOC orders would need an additional revision for the unsolicited cancellation of
        // any unfilled quantity.

        // Commit taker order.
        new_order->rev += trans.count;
        new_order->status = DBR_TRADED;
        new_order->resd -= trans.taken;
        new_order->exec += trans.taken;
        new_order->last_ticks = trans.last_ticks;
        new_order->last_lots = trans.last_lots;
    }

    // Place incomplete order in book.
    if (!dbr_order_done(new_order) && !dbr_book_insert(book, new_order))
        goto fail4;

    // Journal commit can still fail.
    if (!dbr_journ_commit_trans(serv->journ))
        goto fail5;

    // Final commit phase cannot fail.
    fig_trader_emplace_order(trader, new_order);
    // Commit trans to result and free matches.
    commit_result(serv, trader, book, &trans, now, result);
    return new_order;
 fail5:
    if (!dbr_order_done(new_order))
        dbr_book_remove(book, new_order);
 fail4:
    free_matches(trans.first_match, serv->pool);
    memset(&trans, 0, sizeof(trans));
 fail3:
    dbr_journ_rollback_trans(serv->journ);
 fail2:
    dbr_pool_free_order(serv->pool, new_order);
 fail1:
    memset(&trans, 0, sizeof(trans));
    return NULL;
}

DBR_API struct DbrOrder*
dbr_serv_revise_id(DbrServ serv, DbrTrader trader, DbrIden id, DbrLots lots)
{
    struct DbrRbNode* node = fig_trader_find_order_id(trader, id);
    if (!node) {
        dbr_err_setf(DBR_EINVAL, "no such order '%ld'", id);
        goto fail1;
    }

    struct DbrOrder* order = dbr_trader_order_entry(node);
    if (dbr_order_done(order)) {
        dbr_err_setf(DBR_EINVAL, "order complete '%ld'", id);
        goto fail1;
    }

    // Revised lots must not be:
    // 1. less than min lots;
    // 2. less than executed lots;
    // 3. greater than original lots.

    if (lots == 0 || lots < order->min || lots < order->exec || lots > order->lots) {
        dbr_err_setf(DBR_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    if (!dbr_journ_begin_trans(serv->journ))
        goto fail1;

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_update_order(serv->journ, id, order->rev + 1, DBR_REVISED,
                                lots, order->resd, order->exec, order->last_ticks,
                                order->last_lots, now))
        goto fail2;

    // Must succeed because order exists.
    struct DbrBook* book = get_book(serv, order->contr.rec, order->settl_date);
    assert(book);
    dbr_book_revise(book, order, lots, now);
    dbr_journ_commit_trans(serv->journ);
    return order;
 fail2:
    dbr_journ_rollback_trans(serv->journ);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_serv_revise_ref(DbrServ serv, DbrTrader trader, const char* ref, DbrLots lots)
{
    struct DbrOrder* order = fig_trader_find_order_ref(trader, ref);
    if (!order) {
        dbr_err_setf(DBR_EINVAL, "no such order '%.64s'", ref);
        goto fail1;
    }

    if (dbr_order_done(order)) {
        dbr_err_setf(DBR_EINVAL, "order complete '%.64s'", ref);
        goto fail1;
    }

    // Revised lots must not be:
    // 1. less than min lots;
    // 2. less than executed lots;
    // 3. greater than original lots.

    if (lots == 0 || lots < order->min || lots < order->exec || lots > order->lots) {
        dbr_err_setf(DBR_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    if (!dbr_journ_begin_trans(serv->journ))
        goto fail1;

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_update_order(serv->journ, order->id, order->rev + 1, DBR_REVISED,
                                lots, order->resd, order->exec, order->last_ticks,
                                order->last_lots, now))
        goto fail2;

    struct DbrBook* book = get_book(serv, order->contr.rec, order->settl_date);
    assert(book);
    dbr_book_revise(book, order, lots, now);
    dbr_journ_commit_trans(serv->journ);
    return order;
 fail2:
    dbr_journ_rollback_trans(serv->journ);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_serv_cancel_id(DbrServ serv, DbrTrader trader, DbrIden id)
{
    struct DbrRbNode* node = fig_trader_find_order_id(trader, id);
    if (!node) {
        dbr_err_setf(DBR_EINVAL, "no such order '%ld'", id);
        goto fail1;
    }

    struct DbrOrder* order = dbr_trader_order_entry(node);
    if (dbr_order_done(order)) {
        dbr_err_setf(DBR_EINVAL, "order complete '%ld'", id);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_update_order(serv->journ, id, order->rev + 1, DBR_CANCELLED,
                                order->lots, 0, order->exec, order->last_ticks,
                                order->last_lots, now))
        goto fail1;

    struct DbrBook* book = get_book(serv, order->contr.rec, order->settl_date);
    assert(book);
    dbr_book_cancel(book, order, now);
    return order;
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_serv_cancel_ref(DbrServ serv, DbrTrader trader, const char* ref)
{
    struct DbrOrder* order = fig_trader_find_order_ref(trader, ref);
    if (!order) {
        dbr_err_setf(DBR_EINVAL, "no such order '%.64s'", ref);
        goto fail1;
    }

    if (dbr_order_done(order)) {
        dbr_err_setf(DBR_EINVAL, "order complete '%.64s'", ref);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_update_order(serv->journ, order->id, order->rev + 1, DBR_CANCELLED,
                                order->lots, 0, order->exec, order->last_ticks,
                                order->last_lots, now))
        goto fail1;

    struct DbrBook* book = get_book(serv, order->contr.rec, order->settl_date);
    assert(book);
    dbr_book_cancel(book, order, now);
    return order;
 fail1:
    return NULL;
}

DBR_API DbrBool
dbr_serv_archive_order(DbrServ serv, DbrTrader trader, DbrIden id)
{
    struct DbrRbNode* node = fig_trader_find_order_id(trader, id);
    if (!node) {
        dbr_err_setf(DBR_EINVAL, "no such order '%ld'", id);
        goto fail1;
    }

    struct DbrOrder* order = dbr_trader_order_entry(node);
    if (!dbr_order_done(order)) {
        dbr_err_setf(DBR_EINVAL, "order '%ld' not done", id);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_archive_order(serv->journ, node->key, now))
        goto fail1;

    // No need to update timestamps on trade because it is immediately freed.

    fig_trader_release_order(trader, order);
    dbr_pool_free_order(serv->pool, order);
    return true;
 fail1:
    return false;
}

DBR_API DbrBool
dbr_serv_archive_trade(DbrServ serv, DbrTrader trader, DbrIden id)
{
    struct DbrRbNode* node = fig_trader_find_trade_id(trader, id);
    if (!node) {
        dbr_err_setf(DBR_EINVAL, "no such trade '%ld'", id);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_archive_trade(serv->journ, node->key, now))
        goto fail1;

    // No need to update timestamps on trade because it is immediately freed.

    struct DbrExec* exec = dbr_trader_trade_entry(node);
    fig_trader_release_trade(trader, exec);
    dbr_pool_free_exec(serv->pool, exec);
    return true;
 fail1:
    return false;
}
