/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "factory.hpp"

#include <scpp/elm/pool.hpp>

#include <sc/elm/conv.h>

using namespace sc;
using namespace std;

shared_ptr<ScRec>
create_accnt(Pool& pool, ScIden id, const char* mnem, const char* display, const char* email)
{
    auto deleter = [&pool](ScRec* rec) {
        pool.free_rec(rec);
    };
    shared_ptr<ScRec> rec(pool.alloc_rec(), deleter);
    sc_rec_init(rec.get());

    rec->type = SC_ENTITY_ACCNT;
    rec->id = id;
    strncpy(rec->mnem, mnem, SC_MNEM_MAX);
    strncpy(rec->display, display, SC_DISPLAY_MAX);

    // Body.
    strncpy(rec->accnt.email, email, SC_EMAIL_MAX);
    return rec;
}

shared_ptr<ScRec>
create_contr(Pool& pool, ScIden id, const char* mnem, const char* display, const char* asset_type,
             const char* asset, const char* ccy, int tick_numer, int tick_denom, int lot_numer,
             int lot_denom, int pip_dp, ScLots min_lots, ScLots max_lots)
{
    auto deleter = [&pool](ScRec* rec) {
        pool.free_rec(rec);
    };
    std::shared_ptr<ScRec> rec(pool.alloc_rec(), deleter);
    sc_rec_init(rec.get());

    rec->type = SC_ENTITY_CONTR;
    rec->id = id;
    strncpy(rec->mnem, mnem, SC_MNEM_MAX);
    strncpy(rec->display, display, SC_DISPLAY_MAX);

    // Body.
    strncpy(rec->contr.asset_type, asset_type, SC_MNEM_MAX);
    strncpy(rec->contr.asset, asset, SC_MNEM_MAX);
    strncpy(rec->contr.ccy, ccy, SC_MNEM_MAX);
    rec->contr.tick_numer = tick_numer;
    rec->contr.tick_denom = tick_denom;
    rec->contr.price_inc = sc_fract_to_real(tick_numer, tick_denom);
    rec->contr.lot_numer = lot_numer;
    rec->contr.lot_denom = lot_denom;
    rec->contr.qty_inc = sc_fract_to_real(lot_numer, lot_denom);
    rec->contr.price_dp = sc_real_to_dp(rec->contr.price_inc);
    rec->contr.pip_dp = pip_dp;
    rec->contr.qty_dp = sc_real_to_dp(rec->contr.qty_inc);
    rec->contr.min_lots = min_lots;
    rec->contr.max_lots = max_lots;
    return rec;
}

shared_ptr<ScPerm>
create_perm(Pool& pool, ScIden tid, ScIden gid)
{
    auto deleter = [&pool](ScPerm* perm) {
        pool.free_perm(perm);
    };
    std::shared_ptr<ScPerm> perm(pool.alloc_perm(), deleter);
    sc_perm_init(perm.get());

    perm->giveup.id_only = gid;
    perm->trader.id_only = tid;

    return perm;
}

shared_ptr<ScOrder>
create_order(Pool& pool, ScIden id, ScRec& trader, ScRec& giveup, ScRec& contr,
             ScJd settl_day, const char* ref, int action, ScTicks ticks, ScLots lots,
             ScLots min_lots, ScMillis now)
{
    auto deleter = [&pool](ScOrder* order) {
        pool.free_order(order);
    };
    std::shared_ptr<ScOrder> order(pool.alloc_order(), deleter);
    sc_order_init(order.get());

    order->level = NULL;
    order->id = id;
    order->i.trader.rec = &trader;
    order->i.giveup.rec = &giveup;
    order->i.contr.rec = &contr;
    order->i.settl_day = settl_day;
    if (ref)
        strncpy(order->i.ref, ref, SC_REF_MAX);
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

shared_ptr<ScOrder>
create_order(Pool& pool, ScIden id, ScIden tid, ScIden gid, ScIden cid,
             ScJd settl_day, const char* ref, int action, ScTicks ticks, ScLots lots,
             ScLots min_lots, ScMillis now)
{
    auto deleter = [&pool](ScOrder* order) {
        pool.free_order(order);
    };
    std::shared_ptr<ScOrder> order(pool.alloc_order(), deleter);
    sc_order_init(order.get());

    order->level = NULL;
    order->id = id;
    order->i.trader.id_only = tid;
    order->i.giveup.id_only = gid;
    order->i.contr.id_only = cid;
    order->i.settl_day = settl_day;
    if (ref)
        strncpy(order->i.ref, ref, SC_REF_MAX);
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

std::shared_ptr<ScExec>
create_trade(sc::Pool& pool, ScIden id, ScIden order, ScRec& trader, ScRec& giveup,
             ScRec& contr, ScJd settl_day, const char* ref, int action, ScTicks ticks,
             ScLots lots, ScLots resd, ScLots exec, ScTicks last_ticks, ScLots last_lots,
             ScIden match, int role, ScRec& cpty, ScMillis now)
{
    auto deleter = [&pool](ScExec* exec) {
        pool.free_exec(exec);
    };
    std::shared_ptr<ScExec> ptr(pool.alloc_exec(), deleter);
    sc_exec_init(ptr.get());

    ptr->id = id;
    ptr->order = order;
    ptr->i.trader.rec = &trader;
    ptr->i.giveup.rec = &giveup;
    ptr->i.contr.rec = &contr;
    ptr->i.settl_day = settl_day;
    if (ref)
        strncpy(ptr->i.ref, ref, SC_REF_MAX);
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

shared_ptr<ScExec>
create_trade(Pool& pool, ScIden id, ScIden order, ScIden tid, ScIden gid,
             ScIden cid, ScJd settl_day, const char* ref, int action, ScTicks ticks,
             ScLots lots, ScLots resd, ScLots exec, ScTicks last_ticks, ScLots last_lots,
             ScIden match, int role, ScIden cpty, ScMillis now)
{
    auto deleter = [&pool](ScExec* exec) {
        pool.free_exec(exec);
    };
    std::shared_ptr<ScExec> ptr(pool.alloc_exec(), deleter);
    sc_exec_init(ptr.get());

    ptr->id = id;
    ptr->order = order;
    ptr->i.trader.id_only = tid;
    ptr->i.giveup.id_only = gid;
    ptr->i.contr.id_only = cid;
    ptr->i.settl_day = settl_day;
    if (ref)
        strncpy(ptr->i.ref, ref, SC_REF_MAX);
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
