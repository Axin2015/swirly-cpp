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
#include "journ.hpp"
#include "model.hpp"
#include "test.hpp"

#include <dbr/conv.h>
#include <dbr/err.h>
#include <dbr/queue.h>

using namespace dbr;
using namespace std;

namespace {

void
set_instr(DbrRec& rec, DbrIden id, const char* mnem, const char* display,
          const char* asset_type, const char* instr_type, const char* asset,
          const char* ccy, int tick_numer, int tick_denom, int lot_numer,
          int lot_denom, int pip_dp, DbrLots min_lots, DbrLots max_lots) noexcept
{
    rec.type = DBR_INSTR;
    rec.id = id;
    strncpy(rec.mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec.instr.display, display, DBR_DISPLAY_MAX);
    strncpy(rec.instr.asset_type, asset_type, DBR_MNEM_MAX);
    strncpy(rec.instr.instr_type, instr_type, DBR_MNEM_MAX);
    strncpy(rec.instr.asset, asset, DBR_MNEM_MAX);
    strncpy(rec.instr.ccy, ccy, DBR_MNEM_MAX);
    rec.instr.price_inc = dbr_fract_to_real(tick_numer, tick_denom);
    rec.instr.qty_inc = dbr_fract_to_real(lot_numer, lot_denom);
    rec.instr.price_dp = dbr_real_to_dp(rec.instr.price_inc);
    rec.instr.pip_dp = pip_dp;
    rec.instr.qty_dp = dbr_real_to_dp(rec.instr.qty_inc);
    rec.instr.min_lots = min_lots;
    rec.instr.max_lots = max_lots;
    rec.instr.state = nullptr;
}

ssize_t
read_instr(DbrPool pool, DbrSlNode*& first) noexcept
{
    ssize_t size = 0;

    DbrQueue rq;
    dbr_queue_init(&rq);

    DbrRec* rec = dbr_pool_alloc_rec(pool);
    set_instr(*rec, 1, "EURUSD.SPOTFWD", "EURUSD.SPOTFWD", "CURRENCY", "SPOTFWD",
              "EUR", "USD", 1, 10000, 1000000, 1, 4, 1, 10);
    dbr_queue_push(&rq, &rec->model_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    set_instr(*rec, 2, "GBPUSD.SPOTFWD", "GBPUSD.SPOTFWD", "CURRENCY", "SPOTFWD",
              "GBP", "USD", 1, 10000, 1000000, 1, 4, 1, 10);
    dbr_queue_push(&rq, &rec->model_node_);
    ++size;

    first = rq.first;
    return size;
}

void
set_market(DbrRec& rec, DbrIden id, const char* mnem, DbrIden instr, const char* tenor,
           DbrDate settl_date) noexcept
{
    rec.type = DBR_MARKET;
    rec.id = id;
    strncpy(rec.mnem, mnem, DBR_MNEM_MAX);
    rec.market.instr.id = instr;
    strncpy(rec.market.tenor, tenor, DBR_TENOR_MAX);
    rec.market.settl_date = settl_date;
    rec.market.state = nullptr;
}

ssize_t
read_market(DbrPool pool, DbrSlNode*& first) noexcept
{
    ssize_t size = 0;

    DbrQueue rq;
    dbr_queue_init(&rq);

    DbrRec* rec = dbr_pool_alloc_rec(pool);
    set_market(*rec, 1, "EURUSD", 1, "SP", 20130417);
    dbr_queue_push(&rq, &rec->model_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    set_market(*rec, 2, "GBPUSD", 2, "SP", 20130417);
    dbr_queue_push(&rq, &rec->model_node_);
    ++size;

    first = rq.first;
    return size;
}

void
set_trader(DbrRec& rec, DbrIden id, const char* mnem, const char* display,
           const char* email) noexcept
{
    rec.type = DBR_TRADER;
    rec.id = id;
    strncpy(rec.mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec.trader.display, display, DBR_DISPLAY_MAX);
    strncpy(rec.trader.email, email, DBR_EMAIL_MAX);
    rec.trader.state = nullptr;
}

ssize_t
read_trader(DbrPool pool, DbrSlNode*& first) noexcept
{
    ssize_t size = 0;

    DbrQueue rq;
    dbr_queue_init(&rq);

    DbrRec* rec = dbr_pool_alloc_rec(pool);
    set_trader(*rec, 1, "WRAMIREZ", "Wayne Ramirez", "wayne.ramirez@doobry.org");
    dbr_queue_push(&rq, &rec->model_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    set_trader(*rec, 2, "SFLORES", "Steven Flores", "steven.flores@doobry.org");
    dbr_queue_push(&rq, &rec->model_node_);
    ++size;

    first = rq.first;
    return size;
}

void
set_accnt(DbrRec& rec, DbrIden id, const char* mnem, const char* display,
          const char* email) noexcept
{
    rec.type = DBR_ACCNT;
    rec.id = id;
    strncpy(rec.mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec.accnt.display, display, DBR_DISPLAY_MAX);
    strncpy(rec.accnt.email, email, DBR_EMAIL_MAX);
    rec.accnt.state = nullptr;
}

ssize_t
read_accnt(DbrPool pool, DbrSlNode*& first) noexcept
{
    ssize_t size = 0;

    DbrQueue rq;
    dbr_queue_init(&rq);

    DbrRec* rec = dbr_pool_alloc_rec(pool);
    set_accnt(*rec, 1, "DBRA", "Account A", "dbra@doobry.org");
    dbr_queue_push(&rq, &rec->model_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    set_accnt(*rec, 2, "DBRB", "Account B", "dbrb@doobry.org");
    dbr_queue_push(&rq, &rec->model_node_);
    ++size;

    first = rq.first;
    return size;
}

ssize_t
read_order(DbrPool pool, DbrSlNode*& first) noexcept
{
    first = nullptr;
    return 0;
}

void
set_memb(DbrMemb& memb, DbrIden accnt, DbrIden trader) noexcept
{
    memb.accnt.id = accnt;
    memb.trader.id = trader;
}

ssize_t
read_memb(DbrPool pool, DbrSlNode*& first) noexcept
{
    ssize_t size = 0;

    DbrQueue rq;
    dbr_queue_init(&rq);

    DbrMemb* memb = dbr_pool_alloc_memb(pool, 1);
    set_memb(*memb, 1, 1);
    dbr_queue_push(&rq, &memb->model_node_);
    ++size;

    memb = dbr_pool_alloc_memb(pool, 1);
    set_memb(*memb, 2, 1);
    dbr_queue_push(&rq, &memb->model_node_);
    ++size;

    memb = dbr_pool_alloc_memb(pool, 2);
    set_memb(*memb, 2, 2);
    dbr_queue_push(&rq, &memb->model_node_);
    ++size;

    first = rq.first;
    return size;
}

ssize_t
read_trade(DbrPool pool, DbrSlNode*& first) noexcept
{
    first = nullptr;
    return 0;
}

ssize_t
read_posn(DbrPool pool, DbrSlNode*& first) noexcept
{
    first = nullptr;
    return 0;
}
} // anonymous

ssize_t
Model::read_entity(int type, DbrSlNode*& first) noexcept
{
    ssize_t ret;
    switch (type) {
    case DBR_INSTR:
        ret = read_instr(pool_, first);
        break;
    case DBR_MARKET:
        ret = read_market(pool_, first);
        break;
    case DBR_TRADER:
        ret = read_trader(pool_, first);
        break;
    case DBR_ACCNT:
        ret = read_accnt(pool_, first);
        break;
    case DBR_ORDER:
        ret = read_order(pool_, first);
        break;
    case DBR_MEMB:
        ret = read_memb(pool_, first);
        break;
    case DBR_TRADE:
        ret = read_trade(pool_, first);
        break;
    case DBR_POSN:
        ret = read_posn(pool_, first);
        break;
    default:
        dbr_err_set(DBR_EINVAL, "invalid type '%d'", type);
        first = nullptr;
        ret = -1;
    }
    return ret;
}

DbrSlNode*
Model::end_entity() noexcept
{
    return nullptr;
}
