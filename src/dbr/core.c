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
#include <dbr/core.h>

#include <dbr/util.h>

#include <elm/accnt.h>
#include <elm/cache.h>
#include <elm/err.h>
#include <elm/exec.h>
#include <elm/index.h>
#include <elm/market.h>
#include <elm/pool.h>
#include <elm/trader.h>

#include <stdbool.h>
#include <stdlib.h> // malloc()

struct DbrCtx_ {
    DbrPool pool;
    DbrModel model;
    struct ElmCache cache;
    struct ElmIndex index;
    struct ElmExec exec;
};

static inline struct DbrRec*
get_id(DbrCtx ctx, int type, DbrIden id)
{
    struct DbrSlNode* node = elm_cache_find_id(&ctx->cache, type, id);
    assert(node != elm_cache_end(&ctx->cache));
    return dbr_rec_entry(node);
}

static DbrBool
emplace_recs(DbrCtx ctx, int type)
{
    struct DbrSlNode* node;
    ssize_t size = dbr_model_select(ctx->model, type, &node);
    if (size == -1)
        return false;

    elm_cache_emplace(&ctx->cache, type, node, size);
    return true;
}

static DbrBool
emplace_orders(DbrCtx ctx)
{
    struct DbrSlNode* node;
    if (dbr_model_select(ctx->model, DBR_ORDER, &node) == -1)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrOrder* order = dbr_order_entry(node);
        order->trader.rec = get_id(ctx, DBR_TRADER, order->trader.id);
        order->accnt.rec = get_id(ctx, DBR_ACCNT, order->accnt.id);
        order->market.rec = get_id(ctx, DBR_MARKET, order->market.id);

        struct ElmMarket* market;
        if (!dbr_order_done(order)) {

            struct ElmMarket* market = elm_market_lazy(order->market.rec, ctx->pool);
            if (dbr_unlikely(!market))
                goto fail2;

            if (dbr_unlikely(!elm_market_insert(market, order)))
                goto fail2;
        } else
            market = NULL;

        struct ElmTrader* trader = elm_trader_lazy(order->trader.rec, ctx->pool, &ctx->index);
        if (dbr_unlikely(!trader)) {
            if (market)
                elm_market_remove(market, order);
            goto fail2;
        }

        // Transfer ownership.
        elm_trader_emplace_order(trader, order);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrOrder* order = dbr_order_entry(node);
        node = node->next;
        elm_pool_free_order(ctx->pool, order);
    } while (node);
 fail1:
    return false;
}

static DbrBool
emplace_membs(DbrCtx ctx)
{
    struct DbrSlNode* node;
    if (dbr_model_select(ctx->model, DBR_MEMB, &node) == -1)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrMemb* memb = dbr_memb_entry(node);
        memb->accnt.rec = get_id(ctx, DBR_ACCNT, memb->accnt.id);
        memb->trader.rec = get_id(ctx, DBR_TRADER, memb->trader.id);

        struct ElmAccnt* accnt = elm_accnt_lazy(memb->accnt.rec, ctx->pool);
        if (dbr_unlikely(!accnt))
            goto fail2;

        // Transfer ownership.
        elm_accnt_emplace_memb(accnt, memb);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrMemb* memb = dbr_memb_entry(node);
        node = node->next;
        elm_pool_free_memb(ctx->pool, memb);
    } while (node);
 fail1:
    return false;
}

static DbrBool
emplace_trades(DbrCtx ctx)
{
    struct DbrSlNode* node;
    if (dbr_model_select(ctx->model, DBR_TRADE, &node) == -1)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrTrade* trade = dbr_trade_entry(node);
        trade->trader.rec = get_id(ctx, DBR_TRADER, trade->trader.id);
        trade->accnt.rec = get_id(ctx, DBR_ACCNT, trade->accnt.id);
        trade->market.rec = get_id(ctx, DBR_MARKET, trade->market.id);
        trade->cpty.rec = get_id(ctx, DBR_ACCNT, trade->cpty.id);

        struct ElmAccnt* accnt = elm_accnt_lazy(trade->accnt.rec, ctx->pool);
        if (dbr_unlikely(!accnt))
            goto fail2;

        // Transfer ownership.
        elm_accnt_emplace_trade(accnt, trade);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrTrade* trade = dbr_trade_entry(node);
        node = node->next;
        elm_pool_free_trade(ctx->pool, trade);
    } while (node);
 fail1:
    return false;
}

static DbrBool
emplace_posns(DbrCtx ctx)
{
    struct DbrSlNode* node;
    if (dbr_model_select(ctx->model, DBR_POSN, &node) == -1)
        goto fail1;

    for (; node; node = node->next) {
        struct DbrPosn* posn = dbr_posn_entry(node);
        posn->accnt.rec = get_id(ctx, DBR_ACCNT, posn->accnt.id);
        posn->instr.rec = get_id(ctx, DBR_INSTR, posn->instr.id);

        struct ElmAccnt* accnt = elm_accnt_lazy(posn->accnt.rec, ctx->pool);
        if (dbr_unlikely(!accnt))
            goto fail2;

        // Transfer ownership.
        elm_accnt_emplace_posn(accnt, posn);
    }
    return true;
 fail2:
    // Free tail.
    do {
        struct DbrPosn* posn = dbr_posn_entry(node);
        node = node->next;
        elm_pool_free_posn(ctx->pool, posn);
    } while (node);
 fail1:
    return false;
}

// Ctxironment

DBR_API DbrCtx
dbr_ctx_create(DbrPool pool, DbrModel model)
{
    DbrCtx ctx = malloc(sizeof(struct DbrCtx_));
    if (dbr_unlikely(!ctx)) {
        elm_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }

    ctx->pool = pool;
    ctx->model = model;
    elm_cache_init(&ctx->cache, pool);
    elm_index_init(&ctx->index);
    elm_exec_init(&ctx->exec, pool, model, &ctx->index);

    // Data structures are fully initialised at this point.

    if (!emplace_recs(ctx, DBR_INSTR)
        || !emplace_recs(ctx, DBR_MARKET)
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
        elm_cache_term(&ctx->cache);
        free(ctx);
    }
}

// Rec

DBR_API struct DbrSlNode*
dbr_rec_entries(DbrCtx ctx, int type, size_t* size)
{
    return elm_cache_entries(&ctx->cache, type, size);
}

DBR_API struct DbrSlNode*
dbr_rec_find_id(DbrCtx ctx, int type, DbrIden id)
{
    return elm_cache_find_id(&ctx->cache, type, id);
}

DBR_API struct DbrSlNode*
dbr_rec_find_mnem(DbrCtx ctx, int type, const char* mnem)
{
    return elm_cache_find_mnem(&ctx->cache, type, mnem);
}

DBR_API struct DbrSlNode*
dbr_rec_end(DbrCtx ctx)
{
    return elm_cache_end(&ctx->cache);
}

// Market

DBR_API DbrMarket
dbr_market_lazy(DbrCtx ctx, struct DbrRec* mrec)
{
    return elm_market_lazy(mrec, ctx->pool);
}

DBR_API DbrIden
dbr_market_id(DbrMarket market)
{
    return elm_market_id(market);
}

DBR_API DbrSide
dbr_market_bid_side(DbrMarket market)
{
    return elm_market_bid_side(market);
}

DBR_API DbrSide
dbr_market_ask_side(DbrMarket market)
{
    return elm_market_ask_side(market);
}

DBR_API struct DbrBest*
dbr_market_best(struct DbrRec* mrec, struct DbrBest* best)
{
    return elm_market_best(mrec, best);
}

// Side

// SideOrder

DBR_API struct DbrDlNode*
dbr_side_first_order(DbrSide side)
{
    return elm_side_first_order(side);
}

DBR_API struct DbrDlNode*
dbr_side_last_order(DbrSide side)
{
    return elm_side_last_order(side);
}

DBR_API struct DbrDlNode*
dbr_side_end_order(DbrSide side)
{
    return elm_side_end_order(side);
}

DBR_API DbrBool
dbr_side_empty_order(DbrSide side)
{
    return elm_side_empty_order(side);
}

// SideLevel

DBR_API struct DbrRbNode*
dbr_side_find_level(DbrSide side, DbrTicks ticks)
{
    return elm_side_find_level(side, ticks);
}

DBR_API struct DbrRbNode*
dbr_side_first_level(DbrSide side)
{
    return elm_side_first_level(side);
}

DBR_API struct DbrRbNode*
dbr_side_last_level(DbrSide side)
{
    return elm_side_last_level(side);
}

DBR_API struct DbrRbNode*
dbr_side_end_level(DbrSide side)
{
    return elm_side_end_level(side);
}

DBR_API DbrBool
dbr_side_empty_level(DbrSide side)
{
    return elm_side_empty_level(side);
}

// SideLast

DBR_API DbrTicks
dbr_side_last_ticks(DbrSide side)
{
    return elm_side_last_ticks(side);
}

DBR_API DbrLots
dbr_side_last_lots(DbrSide side)
{
    return elm_side_last_lots(side);
}

DBR_API DbrMillis
dbr_side_last_time(DbrSide side)
{
    return elm_side_last_time(side);
}

// Trader

DBR_API DbrTrader
dbr_trader_lazy(DbrCtx ctx, struct DbrRec* trec)
{
    return elm_trader_lazy(trec, ctx->pool, &ctx->index);
}

DBR_API DbrIden
dbr_trader_id(DbrTrader trader)
{
    return elm_trader_id(trader);
}

// TraderOrder

DBR_API struct DbrRbNode*
dbr_trader_find_order_id(DbrTrader trader, DbrIden id)
{
    return elm_trader_find_order_id(trader, id);
}

DBR_API struct DbrOrder*
dbr_trader_find_order_ref(DbrTrader trader, const char* ref)
{
    return elm_trader_find_order_ref(trader, ref);
}

DBR_API struct DbrRbNode*
dbr_trader_first_order(DbrTrader trader)
{
    return elm_trader_first_order(trader);
}

DBR_API struct DbrRbNode*
dbr_trader_last_order(DbrTrader trader)
{
    return elm_trader_last_order(trader);
}

DBR_API struct DbrRbNode*
dbr_trader_end_order(DbrTrader trader)
{
    return elm_trader_end_order(trader);
}

DBR_API DbrBool
dbr_trader_empty_order(DbrTrader trader)
{
    return elm_trader_empty_order(trader);
}

// TraderSub

DBR_API DbrBool
dbr_trader_sub(DbrTrader trader, DbrMarket market)
{
    return elm_trader_sub(trader, market);
}

DBR_API void
dbr_trader_unsub(DbrTrader trader, DbrIden mrid)
{
    return elm_trader_unsub(trader, mrid);
}

// TraderSess

DBR_API void
dbr_trader_set_sess(DbrTrader trader, DbrTraderSess sess)
{
    elm_trader_set_sess(trader, sess);
}

DBR_API DbrTraderSess
dbr_trader_sess(DbrTrader trader)
{
    return elm_trader_sess(trader);
}

// Accnt

DBR_API DbrAccnt
dbr_accnt_lazy(DbrCtx ctx, struct DbrRec* arec)
{
    return elm_accnt_lazy(arec, ctx->pool);
}

DBR_API DbrIden
dbr_accnt_id(DbrAccnt accnt)
{
    return elm_accnt_id(accnt);
}

// AccntTrade

DBR_API struct DbrRbNode*
dbr_accnt_find_trade_id(DbrAccnt accnt, DbrIden id)
{
    return elm_accnt_find_trade_id(accnt, id);
}

DBR_API struct DbrRbNode*
dbr_accnt_first_trade(DbrAccnt accnt)
{
    return elm_accnt_first_trade(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_last_trade(DbrAccnt accnt)
{
    return elm_accnt_last_trade(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_end_trade(DbrAccnt accnt)
{
    return elm_accnt_end_trade(accnt);
}

DBR_API DbrBool
dbr_accnt_empty_trade(DbrAccnt accnt)
{
    return elm_accnt_empty_trade(accnt);
}

// AccntPosn.

DBR_API struct DbrRbNode*
dbr_accnt_find_posn_id(DbrAccnt accnt, DbrIden id)
{
    return elm_accnt_find_posn_id(accnt, id);
}

DBR_API struct DbrRbNode*
dbr_accnt_first_posn(DbrAccnt accnt)
{
    return elm_accnt_first_posn(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_last_posn(DbrAccnt accnt)
{
    return elm_accnt_last_posn(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_end_posn(DbrAccnt accnt)
{
    return elm_accnt_end_posn(accnt);
}

DBR_API DbrBool
dbr_accnt_empty_posn(DbrAccnt accnt)
{
    return elm_accnt_empty_posn(accnt);
}

DBR_API void
dbr_accnt_set_sess(DbrAccnt accnt, DbrAccntSess sess)
{
    elm_accnt_set_sess(accnt, sess);
}

DBR_API DbrAccntSess
dbr_accnt_sess(DbrAccnt accnt)
{
    return elm_accnt_sess(accnt);
}

// Exec

DBR_API struct DbrOrder*
dbr_exec_submit(DbrCtx ctx, struct DbrRec* trec, struct DbrRec* arec, const char* ref,
                struct DbrRec* mrec, int action, DbrTicks ticks, DbrLots lots, DbrLots min,
                DbrFlags flags, struct DbrTrans* trans)
{
    const DbrMillis now = dbr_millis();
    return elm_exec_submit(&ctx->exec, trec, arec, ref, mrec, action, ticks, lots, min, flags,
                           now, trans);
}

DBR_API struct DbrOrder*
dbr_exec_revise_id(DbrCtx ctx, DbrTrader trader, DbrIden id, DbrLots lots)
{
    const DbrMillis now = dbr_millis();
    return elm_exec_revise_id(&ctx->exec, trader, id, lots, now);
}

DBR_API struct DbrOrder*
dbr_exec_revise_ref(DbrCtx ctx, DbrTrader trader, const char* ref, DbrLots lots)
{
    const DbrMillis now = dbr_millis();
    return elm_exec_revise_ref(&ctx->exec, trader, ref, lots, now);
}

DBR_API struct DbrOrder*
dbr_exec_cancel_id(DbrCtx ctx, DbrTrader trader, DbrIden id)
{
    const DbrMillis now = dbr_millis();
    return elm_exec_cancel_id(&ctx->exec, trader, id, now);
}

DBR_API struct DbrOrder*
dbr_exec_cancel_ref(DbrCtx ctx, DbrTrader trader, const char* ref)
{
    const DbrMillis now = dbr_millis();
    return elm_exec_cancel_ref(&ctx->exec, trader, ref, now);
}

DBR_API DbrBool
dbr_exec_archive_order(DbrCtx ctx, DbrTrader trader, DbrIden id)
{
    const DbrMillis now = dbr_millis();
    return elm_exec_archive_order(&ctx->exec, trader, id, now);
}

DBR_API DbrBool
dbr_exec_archive_trade(DbrCtx ctx, DbrAccnt accnt, DbrIden id)
{
    const DbrMillis now = dbr_millis();
    return elm_exec_archive_trade(&ctx->exec, accnt, id, now);
}

// Trans

DBR_API void
dbr_exec_free_matches(DbrCtx ctx, struct DbrSlNode* first)
{
    elm_exec_free_matches(&ctx->exec, first);
}
