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
#include "mock.hpp"

#include <dbr/queue.h>

using namespace dbr;
using namespace std;

namespace {

void
set_trader(DbrRec& rec, DbrIden id, const char* mnem, const char* display,
           const char* email) noexcept
{
    rec.type = DBR_TRADER;
    rec.id = id;
    strncpy(rec.mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec.display, display, DBR_DISPLAY_MAX);
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
    dbr_rec_init(rec);
    set_trader(*rec, 1, "WRAMIREZ", "Wayne Ramirez", "wayne.ramirez@doobry.org");
    dbr_queue_insert_back(&rq, &rec->entity_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    dbr_rec_init(rec);
    set_trader(*rec, 2, "SFLORES", "Steven Flores", "steven.flores@doobry.org");
    dbr_queue_insert_back(&rq, &rec->entity_node_);
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
    strncpy(rec.display, display, DBR_DISPLAY_MAX);
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
    dbr_rec_init(rec);
    set_accnt(*rec, 1, "DBRA", "Account A", "dbra@doobry.org");
    dbr_queue_insert_back(&rq, &rec->entity_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    dbr_rec_init(rec);
    set_accnt(*rec, 2, "DBRB", "Account B", "dbrb@doobry.org");
    dbr_queue_insert_back(&rq, &rec->entity_node_);
    ++size;

    first = rq.first;
    return size;
}

void
set_contr(DbrRec& rec, DbrIden id, const char* mnem, const char* display, const char* asset_type,
          const char* asset, const char* ccy, int tick_numer, int tick_denom, int lot_numer,
          int lot_denom, int pip_dp, DbrLots min_lots, DbrLots max_lots) noexcept
{
    rec.type = DBR_CONTR;
    rec.id = id;
    strncpy(rec.mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec.display, display, DBR_DISPLAY_MAX);
    strncpy(rec.contr.asset_type, asset_type, DBR_MNEM_MAX);
    strncpy(rec.contr.asset, asset, DBR_MNEM_MAX);
    strncpy(rec.contr.ccy, ccy, DBR_MNEM_MAX);
    rec.contr.price_inc = dbr_fract_to_real(tick_numer, tick_denom);
    rec.contr.qty_inc = dbr_fract_to_real(lot_numer, lot_denom);
    rec.contr.price_dp = dbr_real_to_dp(rec.contr.price_inc);
    rec.contr.pip_dp = pip_dp;
    rec.contr.qty_dp = dbr_real_to_dp(rec.contr.qty_inc);
    rec.contr.min_lots = min_lots;
    rec.contr.max_lots = max_lots;
}

ssize_t
read_contr(DbrPool pool, DbrSlNode*& first) noexcept
{
    ssize_t size = 0;

    DbrQueue rq;
    dbr_queue_init(&rq);

    DbrRec* rec = dbr_pool_alloc_rec(pool);
    dbr_rec_init(rec);
    set_contr(*rec, 1, "EURUSD", "EURUSD", "CURRENCY", "EUR", "USD",
              1, 10000, 1000000, 1, 4, 1, 10);
    dbr_queue_insert_back(&rq, &rec->entity_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    dbr_rec_init(rec);
    set_contr(*rec, 2, "GBPUSD", "GBPUSD", "CURRENCY", "GBP", "USD",
              1, 10000, 1000000, 1, 4, 1, 10);
    dbr_queue_insert_back(&rq, &rec->entity_node_);
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

ssize_t
read_trade(DbrPool pool, DbrSlNode*& first) noexcept
{
    first = nullptr;
    return 0;
}

void
set_memb(DbrMemb& memb, DbrIden trader, DbrIden accnt) noexcept
{
    memb.trader.id_only = trader;
    memb.accnt.id_only = accnt;
}

ssize_t
read_memb(DbrPool pool, DbrSlNode*& first) noexcept
{
    ssize_t size = 0;

    DbrQueue rq;
    dbr_queue_init(&rq);

    DbrMemb* memb = dbr_pool_alloc_memb(pool);
    dbr_memb_init(memb);
    set_memb(*memb, 1, 1);
    dbr_queue_insert_back(&rq, &memb->entity_node_);
    ++size;

    memb = dbr_pool_alloc_memb(pool);
    dbr_memb_init(memb);
    set_memb(*memb, 1, 2);
    dbr_queue_insert_back(&rq, &memb->entity_node_);
    ++size;

    memb = dbr_pool_alloc_memb(pool);
    dbr_memb_init(memb);
    set_memb(*memb, 2, 2);
    dbr_queue_insert_back(&rq, &memb->entity_node_);
    ++size;

    first = rq.first;
    return size;
}

ssize_t
read_posn(DbrPool pool, DbrSlNode*& first) noexcept
{
    first = nullptr;
    return 0;
}
} // anonymous

// Journ.

DbrIden
Journ::alloc_id() noexcept
{
    return id_++;
}

DbrBool
Journ::begin_trans() noexcept
{
    return DBR_TRUE;
}

DbrBool
Journ::commit_trans() noexcept
{
    return DBR_TRUE;
}

DbrBool
Journ::rollback_trans() noexcept
{
    return DBR_TRUE;
}

DbrBool
Journ::insert_order(DbrIden id, int rev, int status, DbrIden tid, DbrIden aid, DbrIden cid,
                    DbrDate settl_date, const char* ref, int action, DbrTicks ticks,
                    DbrLots resd, DbrLots exec, DbrLots lots, DbrLots min, DbrFlags flags,
                    DbrMillis now) noexcept
{
    return DBR_TRUE;
}

DbrBool
Journ::update_order(DbrIden id, int rev, int status, DbrLots resd, DbrLots exec,
                    DbrLots lots, DbrMillis now) noexcept
{
    return DBR_TRUE;
}

DbrBool
Journ::archive_order(DbrIden id, DbrMillis now) noexcept
{
    return DBR_TRUE;
}

DbrBool
Journ::insert_trade(DbrIden id, DbrIden order, int rev, DbrIden tid, DbrIden aid, DbrIden cid,
                    DbrDate settl_date, const char* ref, int action, DbrTicks ticks, DbrLots resd,
                    DbrLots exec, DbrLots lots, DbrIden match, DbrIden cpty, int role,
                    DbrMillis now) noexcept
{
    return DBR_TRUE;
}

DbrBool
Journ::archive_trade(DbrIden id, DbrMillis now) noexcept
{
    return DBR_TRUE;
}

// Model.

ssize_t
Model::read_entity(int type, DbrPool pool, DbrSlNode*& first) noexcept
{
    ssize_t ret;
    switch (type) {
    case DBR_TRADER:
        ret = read_trader(pool, first);
        break;
    case DBR_ACCNT:
        ret = read_accnt(pool, first);
        break;
    case DBR_CONTR:
        ret = read_contr(pool, first);
        break;
    case DBR_ORDER:
        ret = read_order(pool, first);
        break;
    case DBR_TRADE:
        ret = read_trade(pool, first);
        break;
    case DBR_MEMB:
        ret = read_memb(pool, first);
        break;
    case DBR_POSN:
        ret = read_posn(pool, first);
        break;
    default:
        dbr_err_setf(DBR_EINVAL, "invalid type '%d'", type);
        first = nullptr;
        ret = -1;
    }
    return ret;
}
