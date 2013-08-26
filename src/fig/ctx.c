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
#include "book.h"
#include "match.h"
#include "trader.h"

#include <dbr/ctx.h>
#include <dbr/err.h>
#include <dbr/journ.h>
#include <dbr/sess.h>
#include <dbr/util.h>

#include <stdbool.h>
#include <stdlib.h> // malloc()
#include <string.h> // strncpy()

struct DbrCtx_ {
    DbrPool pool;
    DbrJourn journ;
    DbrModel model;
    struct DbrTree books;
    struct FigCache cache;
    struct FigIndex index;
};

static inline struct FigBook*
ctx_book_entry(struct DbrRbNode* node)
{
    return dbr_implof(struct FigBook, ctx_node_, node);
}

static void
free_books(struct DbrTree* books)
{
    assert(books);
    struct DbrRbNode* node;
    while ((node = books->root)) {
        struct FigBook* book = ctx_book_entry(node);
        dbr_tree_remove(books, node);
        fig_book_term(book);
        free(book);
    }
}

static void
free_matches(DbrPool pool, struct DbrSlNode* first)
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
            || !dbr_journ_insert_trade(journ, match->taker_trade))
            goto fail1;

        // Maker revision.
        const struct DbrOrder* maker = match->maker_order;
        const int maker_rev = maker->rev + 1;
        const DbrLots maker_resd = maker->resd - match->lots;
        const DbrLots maker_exec = maker->exec + match->lots;
        const int maker_status = maker_resd == 0 ? DBR_FILLED : DBR_PARTIAL;
        if (!dbr_journ_update_order(journ, maker->id, maker_rev, maker_status, maker_resd,
                                    maker_exec, maker->lots, now)
            || !dbr_journ_insert_trade(journ, match->maker_trade))
            goto fail1;

    } while ((node = node->next));
    return true;
 fail1:
    return false;
}

// Assumes that maker lots have not been reduced since matching took place.

static void
apply_trades(DbrCtx ctx, struct FigBook* book, const struct DbrTrans* trans, DbrMillis now)
{
    const struct DbrOrder* taker_order = trans->new_order;
    // Must succeed because new_posn exists.
    struct FigAccnt* taker_accnt = fig_accnt_lazy(taker_order->accnt.rec, ctx->pool);
    assert(taker_accnt);

    for (struct DbrSlNode* node = trans->first_match; node; node = node->next) {

        struct DbrMatch* match = dbr_trans_match_entry(node);
        struct DbrOrder* maker_order = match->maker_order;

        // Reduce maker. Maker's revision will be incremented by this call.
        fig_book_take(book, maker_order, match->lots, now);

        // Must succeed because maker_posn exists.
        struct FigAccnt* maker_accnt = fig_accnt_lazy(maker_order->accnt.rec, ctx->pool);
        assert(maker_accnt);

        // Update taker.
        fig_accnt_emplace_trade(taker_accnt, match->taker_trade);
        apply_posn(trans->new_posn, match->taker_trade);

        // Update maker.
        fig_accnt_emplace_trade(maker_accnt, match->maker_trade);
        apply_posn(match->maker_posn, match->maker_trade);
        // Async trader callback.
        dbr_accnt_sess_trade(fig_accnt_sess(maker_accnt), maker_order, match->maker_trade,
                             match->maker_posn);
    }
}

static inline struct DbrRec*
get_id(DbrCtx ctx, int type, DbrIden id)
{
    struct DbrSlNode* node = fig_cache_find_rec_id(&ctx->cache, type, id);
    assert(node != fig_cache_end_rec(&ctx->cache));
    return dbr_rec_entry(node);
}

static inline DbrBook
get_book(DbrCtx ctx, struct DbrRec* crec, DbrDate settl_date)
{
    assert(crec);
    assert(crec->type == DBR_CONTR);

    // Synthetic key from contract and settlment date.
    const DbrIden key = crec->id * 100000000L + settl_date;

    struct FigBook* book;
	struct DbrRbNode* node = dbr_tree_pfind(&ctx->books, key);
    if (!node || node->key != key) {
        book = malloc(sizeof(struct FigBook));
        if (dbr_unlikely(!book)) {
            dbr_err_set(DBR_ENOMEM, "out of memory");
            return NULL;
        }
        fig_book_init(book, ctx->pool, crec, settl_date);
        struct DbrRbNode* parent = node;
        dbr_tree_pinsert(&ctx->books, &book->ctx_node_, parent);
    } else
        book = ctx_book_entry(node);
    return book;
}

static DbrBool
emplace_recs(DbrCtx ctx, int type)
{
    struct DbrSlNode* node;
    ssize_t size = dbr_model_read_entity(ctx->model, type, &node);
    if (size == -1)
        return false;

    fig_cache_emplace_recs(&ctx->cache, type, node, size);
    return true;
}

static DbrBool
emplace_orders(DbrCtx ctx)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(ctx->model, DBR_ORDER, &node) == -1)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrOrder* order = dbr_order_entry(node);

        // Enrich.
        order->trader.rec = get_id(ctx, DBR_TRADER, order->trader.id);
        order->accnt.rec = get_id(ctx, DBR_ACCNT, order->accnt.id);
        order->contr.rec = get_id(ctx, DBR_CONTR, order->contr.id);

        struct FigBook* book;
        if (!dbr_order_done(order)) {

            book = get_book(ctx, order->contr.rec, order->settl_date);
            if (dbr_unlikely(!book))
                goto fail2;

            if (dbr_unlikely(!fig_book_insert(book, order)))
                goto fail2;
        } else
            book = NULL;

        struct FigTrader* trader = fig_trader_lazy(order->trader.rec, ctx->pool, &ctx->index);
        if (dbr_unlikely(!trader)) {
            if (book)
                fig_book_remove(book, order);
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
        dbr_pool_free_order(ctx->pool, order);
    } while (node);
 fail1:
    return false;
}

static DbrBool
emplace_membs(DbrCtx ctx)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(ctx->model, DBR_MEMB, &node) == -1)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrMemb* memb = dbr_memb_entry(node);

        // Enrich.
        memb->accnt.rec = get_id(ctx, DBR_ACCNT, memb->accnt.id);
        memb->trader.rec = get_id(ctx, DBR_TRADER, memb->trader.id);

        struct FigAccnt* accnt = fig_accnt_lazy(memb->accnt.rec, ctx->pool);
        if (dbr_unlikely(!accnt))
            goto fail2;

        // Transfer ownership.
        fig_accnt_emplace_memb(accnt, memb);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrMemb* memb = dbr_memb_entry(node);
        node = node->next;
        dbr_pool_free_memb(ctx->pool, memb);
    } while (node);
 fail1:
    return false;
}

static DbrBool
emplace_trades(DbrCtx ctx)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(ctx->model, DBR_TRADE, &node) == -1)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrTrade* trade = dbr_trade_entry(node);

        // Enrich.
        trade->trader.rec = get_id(ctx, DBR_TRADER, trade->trader.id);
        trade->accnt.rec = get_id(ctx, DBR_ACCNT, trade->accnt.id);
        trade->contr.rec = get_id(ctx, DBR_CONTR, trade->contr.id);
        trade->cpty.rec = get_id(ctx, DBR_ACCNT, trade->cpty.id);

        struct FigAccnt* accnt = fig_accnt_lazy(trade->accnt.rec, ctx->pool);
        if (dbr_unlikely(!accnt))
            goto fail2;

        // Transfer ownership.
        fig_accnt_emplace_trade(accnt, trade);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrTrade* trade = dbr_trade_entry(node);
        node = node->next;
        dbr_pool_free_trade(ctx->pool, trade);
    } while (node);
 fail1:
    return false;
}

static DbrBool
emplace_posns(DbrCtx ctx)
{
    struct DbrSlNode* node;
    if (dbr_model_read_entity(ctx->model, DBR_POSN, &node) == -1)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrPosn* posn = dbr_posn_entry(node);

        // Enrich.
        posn->accnt.rec = get_id(ctx, DBR_ACCNT, posn->accnt.id);
        posn->contr.rec = get_id(ctx, DBR_CONTR, posn->contr.id);

        struct FigAccnt* accnt = fig_accnt_lazy(posn->accnt.rec, ctx->pool);
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
        dbr_pool_free_posn(ctx->pool, posn);
    } while (node);
 fail1:
    return false;
}

DBR_API DbrCtx
dbr_ctx_create(DbrPool pool, DbrJourn journ, DbrModel model)
{
    DbrCtx ctx = malloc(sizeof(struct DbrCtx_));
    if (dbr_unlikely(!ctx)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    ctx->pool = pool;
    ctx->journ = journ;
    ctx->model = model;
    dbr_tree_init(&ctx->books);
    fig_cache_init(&ctx->cache, pool);
    fig_index_init(&ctx->index);

    // Data structures are fully initialised at this point.

    if (!emplace_recs(ctx, DBR_CONTR)
        || !emplace_recs(ctx, DBR_TRADER)
        || !emplace_recs(ctx, DBR_ACCNT)
        || !emplace_orders(ctx)
        || !emplace_membs(ctx)
        || !emplace_trades(ctx)
        || !emplace_posns(ctx)) {
        dbr_ctx_destroy(ctx);
        goto fail1;
    }

    return ctx;
 fail1:
    return NULL;
}

DBR_API void
dbr_ctx_destroy(DbrCtx ctx)
{
    if (ctx) {
        fig_cache_term(&ctx->cache);
        free_books(&ctx->books);
        free(ctx);
    }
}

// Cache

DBR_API struct DbrSlNode*
dbr_ctx_first_rec(DbrCtx ctx, int type, size_t* size)
{
    return fig_cache_first_rec(&ctx->cache, type, size);
}

DBR_API struct DbrSlNode*
dbr_ctx_find_rec_id(DbrCtx ctx, int type, DbrIden id)
{
    return fig_cache_find_rec_id(&ctx->cache, type, id);
}

DBR_API struct DbrSlNode*
dbr_ctx_find_rec_mnem(DbrCtx ctx, int type, const char* mnem)
{
    return fig_cache_find_rec_mnem(&ctx->cache, type, mnem);
}

DBR_API struct DbrSlNode*
dbr_ctx_end_rec(DbrCtx ctx)
{
    return fig_cache_end_rec(&ctx->cache);
}

// Pool

DBR_API DbrBook
dbr_ctx_book(DbrCtx ctx, struct DbrRec* crec, DbrDate settl_date)
{
    return get_book(ctx, crec, settl_date);
}

DBR_API DbrTrader
dbr_ctx_trader(DbrCtx ctx, struct DbrRec* trec)
{
    return fig_trader_lazy(trec, ctx->pool, &ctx->index);
}

DBR_API DbrAccnt
dbr_ctx_accnt(DbrCtx ctx, struct DbrRec* arec)
{
    return fig_accnt_lazy(arec, ctx->pool);
}

// Exec

DBR_API struct DbrOrder*
dbr_ctx_submit(DbrCtx ctx, struct DbrRec* trec, struct DbrRec* arec, DbrBook book,
               const char* ref, int action, DbrTicks ticks, DbrLots lots, DbrLots min,
               DbrFlags flags, struct DbrTrans* trans)
{
    struct FigTrader* trader = fig_trader_lazy(trec, ctx->pool, &ctx->index);
    if (!trader)
        goto fail1;

    const DbrIden id = dbr_journ_alloc_id(ctx->journ);
    struct DbrOrder* new_order = dbr_pool_alloc_order(ctx->pool, id);
    if (!new_order)
        goto fail1;

    new_order->id = id;
    new_order->level = NULL;
    new_order->rev = 1;
    new_order->status = DBR_NEW;
    new_order->trader.rec = trec;
    new_order->accnt.rec = arec;
    if (ref)
        strncpy(new_order->ref, ref, DBR_REF_MAX);
    else
        new_order->ref[0] = '\0';

    new_order->contr.rec = book->crec;
    new_order->settl_date = book->settl_date;
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

    trans->new_order = new_order;
    trans->new_posn = NULL;

    if (!dbr_journ_begin_trans(ctx->journ))
        goto fail2;

    if (!dbr_journ_insert_order(ctx->journ, new_order)
        || !fig_match_orders(ctx->pool, ctx->journ, book, new_order, trans))
        goto fail3;

    if (trans->count > 0) {

        // Orders were matched.
        if (!insert_trans(ctx->journ, trans, now))
            goto fail4;

        // Commit taker order.
        new_order->rev += trans->count;
        new_order->resd -= trans->taken;
        new_order->exec += trans->taken;

        new_order->status = dbr_order_done(new_order) ? DBR_FILLED : DBR_PARTIAL;
    }

    // Place incomplete order in book.
    if (!dbr_order_done(new_order) && !fig_book_insert(book, new_order))
        goto fail4;

    // Commit phase cannot fail.
    fig_trader_emplace_order(trader, new_order);
    apply_trades(ctx, book, trans, now);
    dbr_journ_commit_trans(ctx->journ);
    return new_order;
 fail4:
    free_matches(ctx->pool, trans->first_match);
    memset(trans, 0, sizeof(*trans));
 fail3:
    dbr_journ_rollback_trans(ctx->journ);
 fail2:
    dbr_pool_free_order(ctx->pool, new_order);
 fail1:
    memset(trans, 0, sizeof(*trans));
    return NULL;
}

DBR_API struct DbrOrder*
dbr_ctx_revise_id(DbrCtx ctx, DbrTrader trader, DbrIden id, DbrLots lots)
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

    if (!dbr_journ_begin_trans(ctx->journ))
        goto fail1;

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_update_order(ctx->journ, id, order->rev + 1, DBR_REVISED, order->resd,
                                order->exec, lots, now))
        goto fail2;

    // Must succeed because order exists.
    struct FigBook* book = get_book(ctx, order->contr.rec, order->settl_date);
    assert(book);

    if (!fig_book_revise(book, order, lots, now))
        goto fail2;

    dbr_journ_commit_trans(ctx->journ);
    return order;
 fail2:
    dbr_journ_rollback_trans(ctx->journ);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_ctx_revise_ref(DbrCtx ctx, DbrTrader trader, const char* ref, DbrLots lots)
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

    if (!dbr_journ_begin_trans(ctx->journ))
        goto fail1;

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_update_order(ctx->journ, order->id, order->rev + 1, DBR_REVISED, order->resd,
                                order->exec, lots, now))
        goto fail2;

    struct FigBook* book = get_book(ctx, order->contr.rec, order->settl_date);
    assert(book);
    if (!fig_book_revise(book, order, lots, now))
        goto fail2;

    dbr_journ_commit_trans(ctx->journ);
    return order;
 fail2:
    dbr_journ_rollback_trans(ctx->journ);
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_ctx_cancel_id(DbrCtx ctx, DbrTrader trader, DbrIden id)
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
    if (!dbr_journ_update_order(ctx->journ, id, order->rev + 1, DBR_CANCELLED, 0,
                                order->exec, order->lots, now))
        goto fail1;

    struct FigBook* book = get_book(ctx, order->contr.rec, order->settl_date);
    assert(book);
    fig_book_cancel(book, order, now);
    return order;
 fail1:
    return NULL;
}

DBR_API struct DbrOrder*
dbr_ctx_cancel_ref(DbrCtx ctx, DbrTrader trader, const char* ref)
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
    if (!dbr_journ_update_order(ctx->journ, order->id, order->rev + 1, DBR_CANCELLED, 0,
                                order->exec, order->lots, now))
        goto fail1;

    struct FigBook* book = get_book(ctx, order->contr.rec, order->settl_date);
    assert(book);
    fig_book_cancel(book, order, now);
    return order;
 fail1:
    return NULL;
}

DBR_API DbrBool
dbr_ctx_archive_order(DbrCtx ctx, DbrTrader trader, DbrIden id)
{
    struct DbrRbNode* node = fig_trader_find_order_id(trader, id);
    if (!node)
        goto fail1;

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_archive_order(ctx->journ, node->key, now))
        goto fail1;

    // No need to update timestamps on trade because it is immediately freed.

    struct DbrOrder* order = dbr_trader_order_entry(node);
    fig_trader_release_order(trader, order);
    dbr_pool_free_order(ctx->pool, order);
    return true;
 fail1:
    return false;
}

DBR_API DbrBool
dbr_ctx_archive_trade(DbrCtx ctx, DbrAccnt accnt, DbrIden id)
{
    struct DbrRbNode* node = fig_accnt_find_trade_id(accnt, id);
    if (!node)
        goto fail1;

    const DbrMillis now = dbr_millis();
    if (!dbr_journ_archive_trade(ctx->journ, node->key, now))
        goto fail1;

    // No need to update timestamps on trade because it is immediately freed.

    struct DbrTrade* trade = dbr_accnt_trade_entry(node);
    fig_accnt_release_trade(accnt, trade);
    dbr_pool_free_trade(ctx->pool, trade);
    return true;
 fail1:
    return false;
}

DBR_API void
dbr_ctx_free_matches(DbrCtx ctx, struct DbrSlNode* first)
{
    struct DbrSlNode* node = first;
    while (node) {
        struct DbrMatch* match = dbr_trans_match_entry(node);
        node = node->next;
        dbr_pool_free_match(ctx->pool, match);
    }
}
