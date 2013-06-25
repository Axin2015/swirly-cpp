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
#include "model.h"
#include "test.h"

#include <dbr/conv.h>

#include <ash/queue.h>

#include <stdbool.h>
#include <stdlib.h> // malloc()

static void
set_instr(struct DbrRec* rec, DbrIden id, const char* mnem, const char* display,
          const char* asset_type, const char* instr_type, const char* asset, const char* ccy,
          int tick_numer, int tick_denom, int lot_numer, int lot_denom, int pip_dp,
          DbrLots min_lots, DbrLots max_lots)
{
    rec->type = DBR_INSTR;
    rec->id = id;
    strncpy(rec->mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec->instr.display, display, DBR_DISPLAY_MAX);
    strncpy(rec->instr.asset_type, asset_type, DBR_MNEM_MAX);
    strncpy(rec->instr.instr_type, instr_type, DBR_MNEM_MAX);
    strncpy(rec->instr.asset, asset, DBR_MNEM_MAX);
    strncpy(rec->instr.ccy, ccy, DBR_MNEM_MAX);
    rec->instr.price_inc = dbr_fract_to_real(tick_numer, tick_denom);
    rec->instr.qty_inc = dbr_fract_to_real(lot_numer, lot_denom);
    rec->instr.price_dp = dbr_real_to_dp(rec->instr.price_inc);
    rec->instr.pip_dp = pip_dp;
    rec->instr.qty_dp = dbr_real_to_dp(rec->instr.qty_inc);
    rec->instr.min_lots = min_lots;
    rec->instr.max_lots = max_lots;
    rec->instr.state = NULL;
}

static ssize_t
select_instr(DbrPool pool, struct DbrSlNode** first)
{
    ssize_t size = 0;

    struct AshQueue rq;
    ash_queue_init(&rq);

    struct DbrRec* rec = dbr_pool_alloc_rec(pool);
    check(rec);
    set_instr(rec, 1, "EURUSD.SPOTFWD", "EURUSD.SPOTFWD", "CURRENCY", "SPOTFWD",
              "EUR", "USD", 1, 10000, 1000000, 1, 4, 1, 10);
    ash_queue_push(&rq, &rec->model_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    check(rec);
    set_instr(rec, 2, "GBPUSD.SPOTFWD", "GBPUSD.SPOTFWD", "CURRENCY", "SPOTFWD",
              "GBP", "USD", 1, 10000, 1000000, 1, 4, 1, 10);
    ash_queue_push(&rq, &rec->model_node_);
    ++size;

    *first = rq.first;
    return size;
}

static void
set_market(struct DbrRec* rec, DbrIden id, const char* mnem, DbrIden instr, const char* tenor,
           DbrDate settl_date)
{
    rec->type = DBR_MARKET;
    rec->id = id;
    strncpy(rec->mnem, mnem, DBR_MNEM_MAX);
    rec->market.instr.id = instr;
    strncpy(rec->market.tenor, tenor, DBR_TENOR_MAX);
    rec->market.settl_date = settl_date;
    rec->market.state = NULL;
}

static ssize_t
select_market(DbrPool pool, struct DbrSlNode** first)
{
    ssize_t size = 0;

    struct AshQueue rq;
    ash_queue_init(&rq);

    struct DbrRec* rec = dbr_pool_alloc_rec(pool);
    check(rec);
    set_market(rec, 1, "EURUSD", 1, "SP", 20130417);
    ash_queue_push(&rq, &rec->model_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    check(rec);
    set_market(rec, 2, "GBPUSD", 2, "SP", 20130417);
    ash_queue_push(&rq, &rec->model_node_);
    ++size;

    *first = rq.first;
    return size;
}

static void
set_trader(struct DbrRec* rec, DbrIden id, const char* mnem, const char* display, const char* email)
{
    rec->type = DBR_TRADER;
    rec->id = id;
    strncpy(rec->mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec->trader.display, display, DBR_DISPLAY_MAX);
    strncpy(rec->trader.email, email, DBR_EMAIL_MAX);
    rec->trader.state = NULL;
}

static ssize_t
select_trader(DbrPool pool, struct DbrSlNode** first)
{
    ssize_t size = 0;

    struct AshQueue rq;
    ash_queue_init(&rq);

    struct DbrRec* rec = dbr_pool_alloc_rec(pool);
    check(rec);
    set_trader(rec, 1, "WRAMIREZ", "Wayne Ramirez", "wayne.ramirez@doobry.org");
    ash_queue_push(&rq, &rec->model_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    check(rec);
    set_trader(rec, 2, "SFLORES", "Steven Flores", "steven.flores@doobry.org");
    ash_queue_push(&rq, &rec->model_node_);
    ++size;

    *first = rq.first;
    return size;
}

static void
set_accnt(struct DbrRec* rec, DbrIden id, const char* mnem, const char* display, const char* email)
{
    rec->type = DBR_ACCNT;
    rec->id = id;
    strncpy(rec->mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec->accnt.display, display, DBR_DISPLAY_MAX);
    strncpy(rec->accnt.email, email, DBR_EMAIL_MAX);
    rec->accnt.state = NULL;
}

static ssize_t
select_accnt(DbrPool pool, struct DbrSlNode** first)
{
    ssize_t size = 0;

    struct AshQueue rq;
    ash_queue_init(&rq);

    struct DbrRec* rec = dbr_pool_alloc_rec(pool);
    check(rec);
    set_accnt(rec, 1, "DBRA", "Account A", "dbra@doobry.org");
    ash_queue_push(&rq, &rec->model_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    check(rec);
    set_accnt(rec, 2, "DBRB", "Account B", "dbrb@doobry.org");
    ash_queue_push(&rq, &rec->model_node_);
    ++size;

    *first = rq.first;
    return size;
}

static ssize_t
select_order(DbrPool pool, struct DbrSlNode** first)
{
    *first = NULL;
    return 0;
}

static void
set_memb(struct DbrMemb* memb, DbrIden accnt, DbrIden trader)
{
    memb->accnt.id = accnt;
    memb->trader.id = trader;
}

static ssize_t
select_memb(DbrPool pool, struct DbrSlNode** first)
{
    ssize_t size = 0;

    struct AshQueue rq;
    ash_queue_init(&rq);

    struct DbrMemb* memb = dbr_pool_alloc_memb(pool, 1);
    check(memb);
    set_memb(memb, 1, 1);
    ash_queue_push(&rq, &memb->model_node_);
    ++size;

    memb = dbr_pool_alloc_memb(pool, 1);
    check(memb);
    set_memb(memb, 2, 1);
    ash_queue_push(&rq, &memb->model_node_);
    ++size;

    memb = dbr_pool_alloc_memb(pool, 2);
    check(memb);
    set_memb(memb, 2, 2);
    ash_queue_push(&rq, &memb->model_node_);
    ++size;

    *first = rq.first;
    return size;
}

static ssize_t
select_trade(DbrPool pool, struct DbrSlNode** first)
{
    *first = NULL;
    return 0;
}

static ssize_t
select_posn(DbrPool pool, struct DbrSlNode** first)
{
    *first = NULL;
    return 0;
}

struct TestImpl {
    DbrPool pool;
    DbrIden id;
    struct DbrIModel model_;
};

static inline struct TestImpl*
test_impl(DbrModel model)
{
    return dbr_implof(struct TestImpl, model_, model);
}

static DbrIden
alloc_id(DbrModel model)
{
    struct TestImpl* impl = test_impl(model);
    return impl->id++;
}

static DbrBool
begin_trans(DbrModel model)
{
    return true;
}

static DbrBool
commit_trans(DbrModel model)
{
    return true;
}

static DbrBool
rollback_trans(DbrModel model)
{
    return true;
}

static DbrBool
insert_order(DbrModel model, const struct DbrOrder* order)
{
    return true;
}

static DbrBool
update_order(DbrModel model, DbrIden id, int rev, int status, DbrLots resd, DbrLots exec,
             DbrLots lots, DbrMillis now)
{
    return true;
}

static DbrBool
archive_order(DbrModel model, DbrIden id, DbrMillis now)
{
    return true;
}

static DbrBool
insert_trade(DbrModel model, const struct DbrTrade* trade)
{
    return true;
}

static DbrBool
archive_trade(DbrModel model, DbrIden id, DbrMillis now)
{
    return true;
}

static ssize_t
select_entity(DbrModel model, int type, struct DbrSlNode** first)
{
    struct TestImpl* impl = test_impl(model);
    ssize_t ret;
    switch (type) {
    case DBR_INSTR:
        ret = select_instr(impl->pool, first);
        break;
    case DBR_MARKET:
        ret = select_market(impl->pool, first);
        break;
    case DBR_TRADER:
        ret = select_trader(impl->pool, first);
        break;
    case DBR_ACCNT:
        ret = select_accnt(impl->pool, first);
        break;
    case DBR_ORDER:
        ret = select_order(impl->pool, first);
        break;
    case DBR_MEMB:
        ret = select_memb(impl->pool, first);
        break;
    case DBR_TRADE:
        ret = select_trade(impl->pool, first);
        break;
    case DBR_POSN:
        ret = select_posn(impl->pool, first);
        break;
    default:
        assert(false);
        *first = NULL;
        ret = 0;
    }
    return ret;
}

static struct DbrSlNode*
end_entity(DbrModel model)
{
    return NULL;
}

static const struct DbrModelVtbl TEST_MODEL_VTBL = {
    .alloc_id = alloc_id,
    .begin_trans = begin_trans,
    .commit_trans = commit_trans,
    .rollback_trans = rollback_trans,
    .insert_order = insert_order,
    .update_order = update_order,
    .archive_order = archive_order,
    .insert_trade = insert_trade,
    .archive_trade = archive_trade,
    .select_entity = select_entity,
    .end_entity = end_entity
};

DBR_EXTERN DbrModel
model_create(DbrPool pool, DbrIden seed)
{
    struct TestImpl* impl = malloc(sizeof(struct TestImpl));
    if (dbr_unlikely(!impl))
        goto fail1;

    // Seed identity.
    impl->id = seed;

    impl->pool = pool;
    impl->model_.vtbl = &TEST_MODEL_VTBL;
    return &impl->model_;
 fail1:
    return NULL;
}

DBR_EXTERN void
model_destroy(DbrModel model)
{
    if (model) {
        struct TestImpl* impl = test_impl(model);
        free(impl);
    }
}
