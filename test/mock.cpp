/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "mock.hpp"
#include "test.hpp"

#include <sc/elm/pool.h>

#include <sc/ash/queue.h>

using namespace sc;
using namespace std;

namespace {

void
set_accnt(ScRec& rec, ScIden id, const char* mnem, const char* display,
          const char* email)
{
    rec.type = SC_ENTITY_ACCNT;
    rec.id = id;
    strncpy(rec.mnem, mnem, SC_MNEM_MAX);
    strncpy(rec.display, display, SC_DISPLAY_MAX);
    strncpy(rec.accnt.email, email, SC_EMAIL_MAX);
    rec.accnt.state = nullptr;
}

ssize_t
read_accnt(ScPool pool, ScSlNode*& first)
{
    ssize_t size = 0;

    ScQueue rq;
    sc_queue_init(&rq);

    ScRec* rec = sc_pool_alloc_rec(pool);
    check(rec);
    sc_rec_init(rec);
    set_accnt(*rec, 1, "WRAMIREZ", "Wayne Ramirez", "wayne.ramirez@swirlycloud.com");
    sc_queue_insert_back(&rq, &rec->shared_node_);
    ++size;

    rec = sc_pool_alloc_rec(pool);
    check(rec);
    sc_rec_init(rec);
    set_accnt(*rec, 2, "SFLORES", "Steven Flores", "steven.flores@swirlycloud.com");
    sc_queue_insert_back(&rq, &rec->shared_node_);
    ++size;

    rec = sc_pool_alloc_rec(pool);
    check(rec);
    sc_rec_init(rec);
    set_accnt(*rec, 3, "SCA", "Account A", "sca@swirlycloud.com");
    sc_queue_insert_back(&rq, &rec->shared_node_);
    ++size;

    rec = sc_pool_alloc_rec(pool);
    check(rec);
    sc_rec_init(rec);
    set_accnt(*rec, 4, "SCB", "Account B", "scb@swirlycloud.com");
    sc_queue_insert_back(&rq, &rec->shared_node_);
    ++size;

    first = rq.first;
    return size;
}

void
set_contr(ScRec& rec, ScIden id, const char* mnem, const char* display, const char* asset_type,
          const char* asset, const char* ccy, int tick_numer, int tick_denom, int lot_numer,
          int lot_denom, int pip_dp, ScLots min_lots, ScLots max_lots)
{
    rec.type = SC_ENTITY_CONTR;
    rec.id = id;
    strncpy(rec.mnem, mnem, SC_MNEM_MAX);
    strncpy(rec.display, display, SC_DISPLAY_MAX);
    strncpy(rec.contr.asset_type, asset_type, SC_MNEM_MAX);
    strncpy(rec.contr.asset, asset, SC_MNEM_MAX);
    strncpy(rec.contr.ccy, ccy, SC_MNEM_MAX);
    rec.contr.price_inc = sc_fract_to_real(tick_numer, tick_denom);
    rec.contr.qty_inc = sc_fract_to_real(lot_numer, lot_denom);
    rec.contr.price_dp = sc_real_to_dp(rec.contr.price_inc);
    rec.contr.pip_dp = pip_dp;
    rec.contr.qty_dp = sc_real_to_dp(rec.contr.qty_inc);
    rec.contr.min_lots = min_lots;
    rec.contr.max_lots = max_lots;
}

ssize_t
read_contr(ScPool pool, ScSlNode*& first)
{
    ssize_t size = 0;

    ScQueue rq;
    sc_queue_init(&rq);

    ScRec* rec = sc_pool_alloc_rec(pool);
    check(rec);
    sc_rec_init(rec);
    set_contr(*rec, 1, "EURUSD", "EURUSD", "CURRENCY", "EUR", "USD",
              1, 10000, 1000000, 1, 4, 1, 10);
    sc_queue_insert_back(&rq, &rec->shared_node_);
    ++size;

    rec = sc_pool_alloc_rec(pool);
    check(rec);
    sc_rec_init(rec);
    set_contr(*rec, 2, "GBPUSD", "GBPUSD", "CURRENCY", "GBP", "USD",
              1, 10000, 1000000, 1, 4, 1, 10);
    sc_queue_insert_back(&rq, &rec->shared_node_);
    ++size;

    first = rq.first;
    return size;
}

void
set_perm(ScPerm& perm, ScIden gid, ScIden tid)
{
    perm.giveup.id_only = gid;
    perm.trader.id_only = tid;
}

ssize_t
read_perm(ScPool pool, ScSlNode*& first)
{
    ssize_t size = 0;

    ScQueue rq;
    sc_queue_init(&rq);

    ScPerm* perm = sc_pool_alloc_perm(pool);
    check(perm);
    sc_perm_init(perm);
    set_perm(*perm, 1, 1);
    sc_queue_insert_back(&rq, &perm->shared_node_);
    ++size;

    perm = sc_pool_alloc_perm(pool);
    check(perm);
    sc_perm_init(perm);
    set_perm(*perm, 2, 1);
    sc_queue_insert_back(&rq, &perm->shared_node_);
    ++size;

    perm = sc_pool_alloc_perm(pool);
    check(perm);
    sc_perm_init(perm);
    set_perm(*perm, 2, 2);
    sc_queue_insert_back(&rq, &perm->shared_node_);
    ++size;

    first = rq.first;
    return size;
}

ssize_t
read_order(ScPool pool, ScSlNode*& first)
{
    first = nullptr;
    return 0;
}

ssize_t
read_trade(ScPool pool, ScSlNode*& first)
{
    first = nullptr;
    return 0;
}

ssize_t
read_posn(ScPool pool, ScSlNode*& first)
{
    first = nullptr;
    return 0;
}
} // anonymous

// Journ.

void
Journ::destroy() noexcept
{
}

ScBool
Journ::insert_exec_list(ScSlNode* first, ScBool enriched) noexcept
{
    return SC_TRUE;
}

ScBool
Journ::insert_exec(ScExec& exec, ScBool enriched) noexcept
{
    return SC_TRUE;
}

ScBool
Journ::update_exec(ScIden id, ScMillis modified) noexcept
{
    return SC_TRUE;
}

// Model.

void
Model::destroy() noexcept
{
}

ssize_t
Model::read_entity(int type, ScPool pool, ScSlNode*& first) noexcept
{
    ssize_t ret = -1;
    try {
        switch (type) {
        case SC_ENTITY_ACCNT:
            ret = read_accnt(pool, first);
            break;
        case SC_ENTITY_CONTR:
            ret = read_contr(pool, first);
            break;
        case SC_ENTITY_PERM:
            ret = read_perm(pool, first);
            break;
        case SC_ENTITY_ORDER:
            ret = read_order(pool, first);
            break;
        case SC_ENTITY_EXEC:
            ret = read_trade(pool, first);
            break;
        case SC_ENTITY_POSN:
            ret = read_posn(pool, first);
            break;
        default:
            sc_err_setf(SC_EINVAL, "invalid type '%d'", type);
            first = nullptr;
        }
    } catch (const TestException& e) {
        cout << " fail" << endl;
        cerr << e.file() << ':' << e.line() << ": " << e.what() << endl;
        exit(1);
    } catch (const sc::ScException& e) {
        cout << " fail" << endl;
        cerr << e.file() << ':' << e.line() << ": " << e.what() << " (" << e.num() << ')' << endl;
        exit(1);
    } catch (const exception& e) {
        cout << " fail" << endl;
        cerr << "exception: " << e.what() << endl;
        exit(1);
    }
    return ret;
}
