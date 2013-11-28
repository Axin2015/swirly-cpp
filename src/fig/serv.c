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
    struct DbrQueue execs;
    struct DbrTree posns;
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
    return dbr_entity_rec_entry(node);
}

static inline struct DbrOrder*
enrich_order(struct FigCache* cache, struct DbrOrder* order)
{
    order->c.trader.rec = get_id(cache, DBR_TRADER, order->c.trader.id_only);
    order->c.accnt.rec = get_id(cache, DBR_ACCNT, order->c.accnt.id_only);
    order->c.contr.rec = get_id(cache, DBR_CONTR, order->c.contr.id_only);
    return order;
}

static inline struct DbrExec*
enrich_trade(struct FigCache* cache, struct DbrExec* exec)
{
    exec->c.trader.rec = get_id(cache, DBR_TRADER, exec->c.trader.id_only);
    exec->c.accnt.rec = get_id(cache, DBR_ACCNT, exec->c.accnt.id_only);
    exec->c.contr.rec = get_id(cache, DBR_CONTR, exec->c.contr.id_only);
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
free_trans(struct DbrTrans* trans, DbrPool pool)
{
    struct DbrSlNode* node = trans->first_match;
    while (node) {
        struct DbrMatch* match = dbr_trans_match_entry(node);
        node = node->next;
        // Not committed so match object still owns the trades.
        dbr_pool_free_exec(pool, match->taker_exec);
        dbr_pool_free_exec(pool, match->maker_exec);
        dbr_pool_free_match(pool, match);
    }
    dbr_pool_free_exec(pool, trans->new_exec);
}

static struct DbrExec*
new_exec(DbrServ serv, struct DbrOrder* order, DbrMillis now)
{
    struct DbrExec* exec = dbr_pool_alloc_exec(serv->pool);
    if (!exec)
        return NULL;

    dbr_exec_init(exec);
    exec->id = dbr_journ_alloc_id(serv->journ);
    exec->order = order->id;
    __builtin_memcpy(&exec->c, &order->c, sizeof(struct DbrCommon));
    exec->match = 0;
    exec->role = 0;
    exec->cpty.id_only = 0;
    exec->created = now;
    return exec;
}

static inline void
apply_posn(struct DbrPosn* posn, const struct DbrExec* exec)
{
    const double licks = exec->c.lots * exec->c.ticks;
    if (exec->c.action == DBR_BUY) {
        posn->buy_licks += licks;
        posn->buy_lots += exec->c.lots;
    } else {
        assert(exec->c.action == DBR_SELL);
        posn->sell_licks += licks;
        posn->sell_lots += exec->c.lots;
    }
}

static DbrBool
insert_matches(DbrJourn journ, struct DbrSlNode* first)
{
    struct DbrSlNode* node = first;
    assert(node);
    do {
        const struct DbrMatch* match = dbr_trans_match_entry(node);

        if (!dbr_journ_insert_exec(journ, match->taker_exec))
            goto fail1;

        if (!dbr_journ_insert_exec(journ, match->maker_exec))
            goto fail1;

    } while ((node = node->next));
    return true;
 fail1:
    return false;
}

static inline struct DbrRbNode*
tree_insert(struct DbrTree* tree, struct DbrRbNode* node)
{
    return dbr_tree_insert(tree, (DbrKey)node, node);
}

// Assumes that maker lots have not been reduced since matching took place.

static void
commit_cycle(DbrServ serv, struct FigTrader* taker, struct DbrBook* book,
              const struct DbrTrans* trans, DbrMillis now)
{
    dbr_queue_insert_back(&serv->execs, &trans->new_exec->serv_node_);
    if (trans->taker_posn)
        tree_insert(&serv->posns, &trans->taker_posn->serv_node_);

    struct DbrSlNode* node = trans->first_match;
    while (node) {

        struct DbrMatch* match = dbr_trans_match_entry(node);
        struct DbrOrder* maker_order = match->maker_order;

        // Reduce maker. Maker's revision will be incremented by this call.
        dbr_book_take(book, maker_order, match->lots, now);
        // Insert maker accnt if taker is a member.
        tree_insert(&serv->posns, &match->maker_posn->serv_node_);

        // Must succeed because maker order exists.
        struct FigTrader* maker = fig_trader_lazy(maker_order->c.trader.rec, &serv->index,
                                                  serv->pool);
        assert(maker);

        // Maker updated first because this is consistent with last-look semantics.

        // Update maker.
        fig_trader_emplace_trade(maker, match->maker_exec);
        apply_posn(match->maker_posn, match->maker_exec);
        dbr_queue_insert_back(&serv->execs, &match->maker_exec->serv_node_);

        // Update taker.
        fig_trader_emplace_trade(taker, match->taker_exec);
        apply_posn(trans->taker_posn, match->taker_exec);
        dbr_queue_insert_back(&serv->execs, &match->taker_exec->serv_node_);

        // Advance node to next before current node is freed.
        node = node->next;
        dbr_pool_free_match(serv->pool, match);
    }
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
        struct DbrOrder* order = enrich_order(&serv->cache, dbr_entity_order_entry(node));
        struct DbrBook* book;
        if (!dbr_order_done(order)) {

            book = get_book(serv, order->c.contr.rec, order->c.settl_date);
            if (dbr_unlikely(!book))
                goto fail2;

            if (dbr_unlikely(!dbr_book_insert(book, order)))
                goto fail2;
        } else
            book = NULL;

        struct FigTrader* trader = fig_trader_lazy(order->c.trader.rec, &serv->index, serv->pool);
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
        struct DbrOrder* order = dbr_entity_order_entry(node);
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
    if (dbr_model_read_entity(serv->model, DBR_EXEC, serv->pool, &node) < 0)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrExec* exec = enrich_trade(&serv->cache, dbr_entity_exec_entry(node));
        struct FigTrader* trader = fig_trader_lazy(exec->c.trader.rec, &serv->index, serv->pool);
        if (dbr_unlikely(!trader))
            goto fail2;

        // Transfer ownership.
        fig_trader_emplace_trade(trader, exec);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrExec* exec = dbr_entity_exec_entry(node);
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
        struct DbrMemb* memb = enrich_memb(&serv->cache, dbr_entity_memb_entry(node));
        struct FigTrader* trader = fig_trader_lazy(memb->trader.rec, &serv->index, serv->pool);
        if (dbr_unlikely(!trader))
            goto fail2;

        struct FigAccnt* accnt = fig_accnt_lazy(memb->accnt.rec, serv->pool);
        if (dbr_unlikely(!accnt))
            goto fail2;

        // Transfer ownership.
        fig_trader_emplace_memb(trader, memb);
        fig_accnt_insert_memb(accnt, memb);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrMemb* memb = dbr_entity_memb_entry(node);
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
        struct DbrPosn* posn = enrich_posn(&serv->cache, dbr_entity_posn_entry(node));
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
        struct DbrPosn* posn = dbr_entity_posn_entry(node);
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
    dbr_queue_init(&serv->execs);
    dbr_tree_init(&serv->posns);

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
              const char* ref, int action, DbrTicks ticks, DbrLots lots, DbrLots min_lots)
{
    if (lots == 0 || lots < min_lots) {
        dbr_err_setf(DBR_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    struct DbrOrder* new_order = dbr_pool_alloc_order(serv->pool);
    if (!new_order)
        goto fail1;
    dbr_order_init(new_order);

    new_order->level = NULL;
    new_order->id = dbr_journ_alloc_id(serv->journ);
    new_order->c.trader.rec = fig_trader_rec(trader);
    new_order->c.accnt.rec = fig_accnt_rec(accnt);
    new_order->c.contr.rec = book->crec;
    new_order->c.settl_date = book->settl_date;
    if (ref)
        strncpy(new_order->c.ref, ref, DBR_REF_MAX);
    else
        new_order->c.ref[0] = '\0';
    new_order->c.status = DBR_NEW;
    new_order->c.action = action;
    new_order->c.ticks = ticks;
    new_order->c.lots = lots;
    new_order->c.resd = lots;
    new_order->c.exec = 0;
    new_order->c.last_ticks = 0;
    new_order->c.last_lots = 0;
    new_order->c.min_lots = min_lots;
    new_order->created = now;
    new_order->modified = now;

    struct DbrTrans trans;
    dbr_trans_init(&trans);

    trans.new_exec = new_exec(serv, new_order, now);
    if (!trans.new_exec)
        goto fail2;

    if (!fig_match_orders(book, new_order, serv->journ, serv->pool, &trans))
        goto fail3;

    if (!dbr_journ_begin_trans(serv->journ))
        goto fail3;

    if (!dbr_journ_insert_exec(serv->journ, trans.new_exec)) {
        dbr_journ_rollback_trans(serv->journ);
        goto fail3;
    }

    if (trans.first_match) {

        // Orders were matched.
        if (!insert_matches(serv->journ, trans.first_match)) {
            dbr_journ_rollback_trans(serv->journ);
            goto fail3;
        }

        // TODO: IOC orders would need an additional revision for the unsolicited cancellation of
        // any unfilled quantity.

        // Commit taker order.
        new_order->c.status = DBR_TRADE;
        new_order->c.resd -= trans.taken;
        new_order->c.exec += trans.taken;
        new_order->c.last_ticks = trans.last_ticks;
        new_order->c.last_lots = trans.last_lots;
    }

    // Place incomplete order in book.
    if (!dbr_order_done(new_order) && !dbr_book_insert(book, new_order)) {
        dbr_journ_rollback_trans(serv->journ);
        goto fail3;
    }

    // Journal commit can still fail.
    if (!dbr_journ_commit_trans(serv->journ))
        goto fail4;

    // Final commit phase cannot fail.
    fig_trader_emplace_order(trader, new_order);
    // Commit trans to cycle and free matches.
    commit_cycle(serv, trader, book, &trans, now);
    // FIXME
    dbr_pool_free_exec(serv->pool, trans.new_exec);
    return new_order;
 fail4:
    if (!dbr_order_done(new_order))
        dbr_book_remove(book, new_order);
 fail3:
    free_trans(&trans, serv->pool);
 fail2:
    dbr_pool_free_order(serv->pool, new_order);
 fail1:
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

    if (lots == 0 || lots < order->c.min_lots || lots < order->c.exec || lots > order->c.lots) {
        dbr_err_setf(DBR_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    struct DbrExec* exec = new_exec(serv, order, now);
    if (!exec)
        goto fail1;

    // Revise.
    exec->c.status = DBR_REVISE;
    exec->c.lots = lots;

    if (!dbr_journ_begin_trans(serv->journ))
        goto fail2;

    if (!dbr_journ_insert_exec(serv->journ, exec)) {
        dbr_journ_rollback_trans(serv->journ);
        goto fail2;
    }

    // Journal commit can still fail.
    if (!dbr_journ_commit_trans(serv->journ))
        goto fail2;

    // Must succeed because order exists.
    struct DbrBook* book = get_book(serv, order->c.contr.rec, order->c.settl_date);
    assert(book);
    dbr_book_revise(book, order, lots, now);
    // FIXME
    dbr_pool_free_exec(serv->pool, exec);
    return order;
 fail2:
    dbr_pool_free_exec(serv->pool, exec);
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

    if (lots == 0 || lots < order->c.min_lots || lots < order->c.exec || lots > order->c.lots) {
        dbr_err_setf(DBR_EINVAL, "invalid lots '%ld'", lots);
        goto fail1;
    }

    const DbrMillis now = dbr_millis();
    struct DbrExec* exec = new_exec(serv, order, now);
    if (!exec)
        goto fail1;

    // Revise.
    exec->c.status = DBR_REVISE;
    exec->c.lots = lots;

    if (!dbr_journ_begin_trans(serv->journ))
        goto fail2;

    if (!dbr_journ_insert_exec(serv->journ, exec)) {
        dbr_journ_rollback_trans(serv->journ);
        goto fail2;
    }

    // Journal commit can still fail.
    if (!dbr_journ_commit_trans(serv->journ))
        goto fail2;

    struct DbrBook* book = get_book(serv, order->c.contr.rec, order->c.settl_date);
    assert(book);
    dbr_book_revise(book, order, lots, now);
    // FIXME
    dbr_pool_free_exec(serv->pool, exec);
    return order;
 fail2:
    dbr_pool_free_exec(serv->pool, exec);
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
    struct DbrExec* exec = new_exec(serv, order, now);
    if (!exec)
        goto fail1;

    // Cancel.
    exec->c.status = DBR_CANCEL;
    exec->c.resd = 0;

    if (!dbr_journ_begin_trans(serv->journ))
        goto fail2;

    if (!dbr_journ_insert_exec(serv->journ, exec)) {
        dbr_journ_rollback_trans(serv->journ);
        goto fail2;
    }

    // Journal commit can still fail.
    if (!dbr_journ_commit_trans(serv->journ))
        goto fail2;

    struct DbrBook* book = get_book(serv, order->c.contr.rec, order->c.settl_date);
    assert(book);
    dbr_book_cancel(book, order, now);
    // FIXME
    dbr_pool_free_exec(serv->pool, exec);
    return order;
 fail2:
    dbr_pool_free_exec(serv->pool, exec);
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
    struct DbrExec* exec = new_exec(serv, order, now);
    if (!exec)
        goto fail1;

    // Cancel.
    exec->c.status = DBR_CANCEL;
    exec->c.resd = 0;

    if (!dbr_journ_begin_trans(serv->journ))
        goto fail2;

    if (!dbr_journ_insert_exec(serv->journ, exec)) {
        dbr_journ_rollback_trans(serv->journ);
        goto fail2;
    }

    // Journal commit can still fail.
    if (!dbr_journ_commit_trans(serv->journ))
        goto fail2;

    struct DbrBook* book = get_book(serv, order->c.contr.rec, order->c.settl_date);
    assert(book);
    dbr_book_cancel(book, order, now);
    // FIXME
    dbr_pool_free_exec(serv->pool, exec);
    return order;
 fail2:
    dbr_pool_free_exec(serv->pool, exec);
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
    if (!dbr_journ_begin_trans(serv->journ))
        goto fail1;

    if (!dbr_journ_archive_order(serv->journ, node->key, now)) {
        dbr_journ_rollback_trans(serv->journ);
        goto fail1;
    }

    // Journal commit can still fail.
    if (!dbr_journ_commit_trans(serv->journ))
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
    if (!dbr_journ_begin_trans(serv->journ))
        goto fail1;

    if (!dbr_journ_archive_trade(serv->journ, node->key, now)) {
        dbr_journ_rollback_trans(serv->journ);
        goto fail1;
    }

    // Journal commit can still fail.
    if (!dbr_journ_commit_trans(serv->journ))
        goto fail1;

    // No need to update timestamps on trade because it is immediately freed.

    struct DbrExec* exec = dbr_trader_trade_entry(node);
    fig_trader_release_trade(trader, exec);
    dbr_pool_free_exec(serv->pool, exec);
    return true;
 fail1:
    return false;
}

DBR_API void
dbr_serv_clear(DbrServ serv)
{
    dbr_queue_init(&serv->execs);
    dbr_tree_init(&serv->posns);
}

DBR_API struct DbrSlNode*
dbr_serv_first_exec(DbrServ serv)
{
    return dbr_queue_first(&serv->execs);
}

DBR_API DbrBool
dbr_serv_empty_exec(DbrServ serv)
{
    return dbr_queue_empty(&serv->execs);
}

DBR_API struct DbrRbNode*
dbr_serv_first_posn(DbrServ serv)
{
    return dbr_tree_first(&serv->posns);
}

DBR_API DbrBool
dbr_serv_empty_posn(DbrServ serv)
{
    return dbr_tree_empty(&serv->posns);
}
