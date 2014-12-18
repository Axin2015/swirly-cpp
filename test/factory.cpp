/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
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

shared_ptr<DbrPerm>
create_perm(Pool& pool, DbrIden tid, DbrIden gid)
{
    auto deleter = [&pool](DbrPerm* perm) {
        pool.free_perm(perm);
    };
    std::shared_ptr<DbrPerm> perm(pool.alloc_perm(), deleter);
    dbr_perm_init(perm.get());

    perm->giveup.id_only = gid;
    perm->trader.id_only = tid;

    return perm;
}

shared_ptr<DbrOrder>
create_order(Pool& pool, DbrIden id, DbrRec& trader, DbrRec& giveup, DbrRec& contr,
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
    order->i.trader.rec = &trader;
    order->i.giveup.rec = &giveup;
    order->i.contr.rec = &contr;
    order->i.settl_day = settl_day;
    if (ref)
        strncpy(order->i.ref, ref, DBR_REF_MAX);
    else
        order->i.ref[0] = '\0';

    order->i.state = 0;
    order->i.action = action;
    order->i.ticks = ticks;
    order->i.lots = lots;
    order->i.resd = lots;
    order->i.exec = 0;
    order->i.last_ticks = 0;
    order->i.last_lots = 0;
    order->i.min_lots = min_lots;
    order->created = now;
    order->modified = now;

    return order;
}

shared_ptr<DbrOrder>
create_order(Pool& pool, DbrIden id, DbrIden tid, DbrIden gid, DbrIden cid,
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
    order->i.trader.id_only = tid;
    order->i.giveup.id_only = gid;
    order->i.contr.id_only = cid;
    order->i.settl_day = settl_day;
    if (ref)
        strncpy(order->i.ref, ref, DBR_REF_MAX);
    else
        order->i.ref[0] = '\0';

    order->i.state = 0;
    order->i.action = action;
    order->i.ticks = ticks;
    order->i.lots = lots;
    order->i.resd = lots;
    order->i.exec = 0;
    order->i.last_ticks = 0;
    order->i.last_lots = 0;
    order->i.min_lots = min_lots;
    order->created = now;
    order->modified = now;

    return order;
}

std::shared_ptr<DbrExec>
create_trade(dbr::Pool& pool, DbrIden id, DbrIden order, DbrRec& trader, DbrRec& giveup,
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
    ptr->i.trader.rec = &trader;
    ptr->i.giveup.rec = &giveup;
    ptr->i.contr.rec = &contr;
    ptr->i.settl_day = settl_day;
    if (ref)
        strncpy(ptr->i.ref, ref, DBR_REF_MAX);
    else
        ptr->i.ref[0] = '\0';
    ptr->i.state = 0;
    ptr->i.action = action;
    ptr->i.ticks = ticks;
    ptr->i.lots = lots;
    ptr->i.resd = resd;
    ptr->i.exec = exec;
    ptr->i.last_ticks = last_ticks;
    ptr->i.last_lots = last_lots;
    ptr->i.min_lots = 0;
    ptr->match = match;
    ptr->role = role;
    ptr->cpty.rec = &cpty;
    ptr->created = now;

    return ptr;
}

shared_ptr<DbrExec>
create_trade(Pool& pool, DbrIden id, DbrIden order, DbrIden tid, DbrIden gid,
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
    ptr->i.trader.id_only = tid;
    ptr->i.giveup.id_only = gid;
    ptr->i.contr.id_only = cid;
    ptr->i.settl_day = settl_day;
    if (ref)
        strncpy(ptr->i.ref, ref, DBR_REF_MAX);
    else
        ptr->i.ref[0] = '\0';
    ptr->i.state = 0;
    ptr->i.action = action;
    ptr->i.ticks = ticks;
    ptr->i.lots = lots;
    ptr->i.resd = resd;
    ptr->i.exec = exec;
    ptr->i.last_ticks = last_ticks;
    ptr->i.last_lots = last_lots;
    ptr->i.min_lots = 0;
    ptr->match = match;
    ptr->role = role;
    ptr->cpty.id_only = cpty;
    ptr->created = now;

    return ptr;
}
