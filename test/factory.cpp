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
#include "factory.hpp"

#include <dbrpp/elm/pool.hpp>

#include <dbr/elm/conv.h>

using namespace dbr;
using namespace std;

shared_ptr<DbrRec>
create_accnt(Pool& pool, DbrIden id, const char* mnem, const char* display, const char* email)
{
    auto deleter = [&pool](DbrRec* rec) {
        pool.free_rec(rec);
    };
    shared_ptr<DbrRec> rec(pool.alloc_rec(), deleter);
    dbr_rec_init(rec.get());

    rec->type = DBR_ENTITY_ACCNT;
    rec->id = id;
    strncpy(rec->mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec->display, display, DBR_DISPLAY_MAX);

    // Body.
    strncpy(rec->accnt.email, email, DBR_EMAIL_MAX);
    return rec;
}

shared_ptr<DbrRec>
create_contr(Pool& pool, DbrIden id, const char* mnem, const char* display, const char* asset_type,
             const char* asset, const char* ccy, int tick_numer, int tick_denom, int lot_numer,
             int lot_denom, int pip_dp, DbrLots min_lots, DbrLots max_lots)
{
    auto deleter = [&pool](DbrRec* rec) {
        pool.free_rec(rec);
    };
    std::shared_ptr<DbrRec> rec(pool.alloc_rec(), deleter);
    dbr_rec_init(rec.get());

    rec->type = DBR_ENTITY_CONTR;
    rec->id = id;
    strncpy(rec->mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec->display, display, DBR_DISPLAY_MAX);

    // Body.
    strncpy(rec->contr.asset_type, asset_type, DBR_MNEM_MAX);
    strncpy(rec->contr.asset, asset, DBR_MNEM_MAX);
    strncpy(rec->contr.ccy, ccy, DBR_MNEM_MAX);
    rec->contr.tick_numer = tick_numer;
    rec->contr.tick_denom = tick_denom;
    rec->contr.price_inc = dbr_fract_to_real(tick_numer, tick_denom);
    rec->contr.lot_numer = lot_numer;
    rec->contr.lot_denom = lot_denom;
    rec->contr.qty_inc = dbr_fract_to_real(lot_numer, lot_denom);
    rec->contr.price_dp = dbr_real_to_dp(rec->contr.price_inc);
    rec->contr.pip_dp = pip_dp;
    rec->contr.qty_dp = dbr_real_to_dp(rec->contr.qty_inc);
    rec->contr.min_lots = min_lots;
    rec->contr.max_lots = max_lots;
    return rec;
}

shared_ptr<DbrMemb>
create_memb(Pool& pool, DbrIden uid, DbrIden gid)
{
    auto deleter = [&pool](DbrMemb* memb) {
        pool.free_memb(memb);
    };
    std::shared_ptr<DbrMemb> memb(pool.alloc_memb(), deleter);
    dbr_memb_init(memb.get());

    memb->group.id_only = gid;
    memb->user.id_only = uid;

    return memb;
}

shared_ptr<DbrOrder>
create_order(Pool& pool, DbrIden id, DbrRec& user, DbrRec& group, DbrRec& contr,
             DbrJd settl_day, const char* ref, int action, DbrTicks ticks, DbrLots lots,
             DbrLots min_lots, DbrMillis now)
{
    auto deleter = [&pool](DbrOrder* order) {
        pool.free_order(order);
    };
    std::shared_ptr<DbrOrder> order(pool.alloc_order(), deleter);
    dbr_order_init(order.get());

    order->level = NULL;
    order->id = id;
    order->c.user.rec = &user;
    order->c.group.rec = &group;
    order->c.contr.rec = &contr;
    order->c.settl_day = settl_day;
    if (ref)
        strncpy(order->c.ref, ref, DBR_REF_MAX);
    else
        order->c.ref[0] = '\0';

    order->c.state = 0;
    order->c.action = action;
    order->c.ticks = ticks;
    order->c.lots = lots;
    order->c.resd = lots;
    order->c.exec = 0;
    order->c.last_ticks = 0;
    order->c.last_lots = 0;
    order->c.min_lots = min_lots;
    order->created = now;
    order->modified = now;

    return order;
}

shared_ptr<DbrOrder>
create_order(Pool& pool, DbrIden id, DbrIden uid, DbrIden gid, DbrIden cid,
             DbrJd settl_day, const char* ref, int action, DbrTicks ticks, DbrLots lots,
             DbrLots min_lots, DbrMillis now)
{
    auto deleter = [&pool](DbrOrder* order) {
        pool.free_order(order);
    };
    std::shared_ptr<DbrOrder> order(pool.alloc_order(), deleter);
    dbr_order_init(order.get());

    order->level = NULL;
    order->id = id;
    order->c.user.id_only = uid;
    order->c.group.id_only = gid;
    order->c.contr.id_only = cid;
    order->c.settl_day = settl_day;
    if (ref)
        strncpy(order->c.ref, ref, DBR_REF_MAX);
    else
        order->c.ref[0] = '\0';

    order->c.state = 0;
    order->c.action = action;
    order->c.ticks = ticks;
    order->c.lots = lots;
    order->c.resd = lots;
    order->c.exec = 0;
    order->c.last_ticks = 0;
    order->c.last_lots = 0;
    order->c.min_lots = min_lots;
    order->created = now;
    order->modified = now;

    return order;
}

std::shared_ptr<DbrExec>
create_trade(dbr::Pool& pool, DbrIden id, DbrIden order, DbrRec& user, DbrRec& group,
             DbrRec& contr, DbrJd settl_day, const char* ref, int action, DbrTicks ticks,
             DbrLots lots, DbrLots resd, DbrLots exec, DbrTicks last_ticks, DbrLots last_lots,
             DbrIden match, int role, DbrRec& cpty, DbrMillis now)
{
    auto deleter = [&pool](DbrExec* exec) {
        pool.free_exec(exec);
    };
    std::shared_ptr<DbrExec> ptr(pool.alloc_exec(), deleter);
    dbr_exec_init(ptr.get());

    ptr->id = id;
    ptr->order = order;
    ptr->c.user.rec = &user;
    ptr->c.group.rec = &group;
    ptr->c.contr.rec = &contr;
    ptr->c.settl_day = settl_day;
    if (ref)
        strncpy(ptr->c.ref, ref, DBR_REF_MAX);
    else
        ptr->c.ref[0] = '\0';
    ptr->c.state = 0;
    ptr->c.action = action;
    ptr->c.ticks = ticks;
    ptr->c.lots = lots;
    ptr->c.resd = resd;
    ptr->c.exec = exec;
    ptr->c.last_ticks = last_ticks;
    ptr->c.last_lots = last_lots;
    ptr->c.min_lots = 0;
    ptr->match = match;
    ptr->role = role;
    ptr->cpty.rec = &cpty;
    ptr->created = now;

    return ptr;
}

shared_ptr<DbrExec>
create_trade(Pool& pool, DbrIden id, DbrIden order, DbrIden uid, DbrIden gid,
             DbrIden cid, DbrJd settl_day, const char* ref, int action, DbrTicks ticks,
             DbrLots lots, DbrLots resd, DbrLots exec, DbrTicks last_ticks, DbrLots last_lots,
             DbrIden match, int role, DbrIden cpty, DbrMillis now)
{
    auto deleter = [&pool](DbrExec* exec) {
        pool.free_exec(exec);
    };
    std::shared_ptr<DbrExec> ptr(pool.alloc_exec(), deleter);
    dbr_exec_init(ptr.get());

    ptr->id = id;
    ptr->order = order;
    ptr->c.user.id_only = uid;
    ptr->c.group.id_only = gid;
    ptr->c.contr.id_only = cid;
    ptr->c.settl_day = settl_day;
    if (ref)
        strncpy(ptr->c.ref, ref, DBR_REF_MAX);
    else
        ptr->c.ref[0] = '\0';
    ptr->c.state = 0;
    ptr->c.action = action;
    ptr->c.ticks = ticks;
    ptr->c.lots = lots;
    ptr->c.resd = resd;
    ptr->c.exec = exec;
    ptr->c.last_ticks = last_ticks;
    ptr->c.last_lots = last_lots;
    ptr->c.min_lots = 0;
    ptr->match = match;
    ptr->role = role;
    ptr->cpty.id_only = cpty;
    ptr->created = now;

    return ptr;
}
