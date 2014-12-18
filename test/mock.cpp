/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "mock.hpp"
#include "test.hpp"

#include <dbr/elm/pool.h>

#include <dbr/ash/queue.h>

using namespace dbr;
using namespace std;

namespace {

void
set_accnt(DbrRec& rec, DbrIden id, const char* mnem, const char* display,
          const char* email)
{
    rec.type = DBR_ENTITY_ACCNT;
    rec.id = id;
    strncpy(rec.mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec.display, display, DBR_DISPLAY_MAX);
    strncpy(rec.accnt.email, email, DBR_EMAIL_MAX);
    rec.accnt.state = nullptr;
}

ssize_t
read_accnt(DbrPool pool, DbrSlNode*& first)
{
    ssize_t size = 0;

    DbrQueue rq;
    dbr_queue_init(&rq);

    DbrRec* rec = dbr_pool_alloc_rec(pool);
    check(rec);
    dbr_rec_init(rec);
    set_accnt(*rec, 1, "WRAMIREZ", "Wayne Ramirez", "wayne.ramirez@doobry.org");
    dbr_queue_insert_back(&rq, &rec->shared_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    check(rec);
    dbr_rec_init(rec);
    set_accnt(*rec, 2, "SFLORES", "Steven Flores", "steven.flores@doobry.org");
    dbr_queue_insert_back(&rq, &rec->shared_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    check(rec);
    dbr_rec_init(rec);
    set_accnt(*rec, 3, "DBRA", "Account A", "dbra@doobry.org");
    dbr_queue_insert_back(&rq, &rec->shared_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    check(rec);
    dbr_rec_init(rec);
    set_accnt(*rec, 4, "DBRB", "Account B", "dbrb@doobry.org");
    dbr_queue_insert_back(&rq, &rec->shared_node_);
    ++size;

    first = rq.first;
    return size;
}

void
set_contr(DbrRec& rec, DbrIden id, const char* mnem, const char* display, const char* asset_type,
          const char* asset, const char* ccy, int tick_numer, int tick_denom, int lot_numer,
          int lot_denom, int pip_dp, DbrLots min_lots, DbrLots max_lots)
{
    rec.type = DBR_ENTITY_CONTR;
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
read_contr(DbrPool pool, DbrSlNode*& first)
{
    ssize_t size = 0;

    DbrQueue rq;
    dbr_queue_init(&rq);

    DbrRec* rec = dbr_pool_alloc_rec(pool);
    check(rec);
    dbr_rec_init(rec);
    set_contr(*rec, 1, "EURUSD", "EURUSD", "CURRENCY", "EUR", "USD",
              1, 10000, 1000000, 1, 4, 1, 10);
    dbr_queue_insert_back(&rq, &rec->shared_node_);
    ++size;

    rec = dbr_pool_alloc_rec(pool);
    check(rec);
    dbr_rec_init(rec);
    set_contr(*rec, 2, "GBPUSD", "GBPUSD", "CURRENCY", "GBP", "USD",
              1, 10000, 1000000, 1, 4, 1, 10);
    dbr_queue_insert_back(&rq, &rec->shared_node_);
    ++size;

    first = rq.first;
    return size;
}

void
set_perm(DbrPerm& perm, DbrIden gid, DbrIden tid)
{
    perm.giveup.id_only = gid;
    perm.trader.id_only = tid;
}

ssize_t
read_perm(DbrPool pool, DbrSlNode*& first)
{
    ssize_t size = 0;

    DbrQueue rq;
    dbr_queue_init(&rq);

    DbrPerm* perm = dbr_pool_alloc_perm(pool);
    check(perm);
    dbr_perm_init(perm);
    set_perm(*perm, 1, 1);
    dbr_queue_insert_back(&rq, &perm->shared_node_);
    ++size;

    perm = dbr_pool_alloc_perm(pool);
    check(perm);
    dbr_perm_init(perm);
    set_perm(*perm, 2, 1);
    dbr_queue_insert_back(&rq, &perm->shared_node_);
    ++size;

    perm = dbr_pool_alloc_perm(pool);
    check(perm);
    dbr_perm_init(perm);
    set_perm(*perm, 2, 2);
    dbr_queue_insert_back(&rq, &perm->shared_node_);
    ++size;

    first = rq.first;
    return size;
}

ssize_t
read_order(DbrPool pool, DbrSlNode*& first)
{
    first = nullptr;
    return 0;
}

ssize_t
read_trade(DbrPool pool, DbrSlNode*& first)
{
    first = nullptr;
    return 0;
}

ssize_t
read_posn(DbrPool pool, DbrSlNode*& first)
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

DbrBool
Journ::insert_exec_list(DbrSlNode* first, DbrBool enriched) noexcept
{
    return DBR_TRUE;
}

DbrBool
Journ::insert_exec(DbrExec& exec, DbrBool enriched) noexcept
{
    return DBR_TRUE;
}

DbrBool
Journ::update_exec(DbrIden id, DbrMillis modified) noexcept
{
    return DBR_TRUE;
}

// Model.

void
Model::destroy() noexcept
{
}

ssize_t
Model::read_entity(int type, DbrPool pool, DbrSlNode*& first) noexcept
{
    ssize_t ret = -1;
    try {
        switch (type) {
        case DBR_ENTITY_ACCNT:
            ret = read_accnt(pool, first);
            break;
        case DBR_ENTITY_CONTR:
            ret = read_contr(pool, first);
            break;
        case DBR_ENTITY_PERM:
            ret = read_perm(pool, first);
            break;
        case DBR_ENTITY_ORDER:
            ret = read_order(pool, first);
            break;
        case DBR_ENTITY_EXEC:
            ret = read_trade(pool, first);
            break;
        case DBR_ENTITY_POSN:
            ret = read_posn(pool, first);
            break;
        default:
            dbr_err_setf(DBR_EINVAL, "invalid type '%d'", type);
            first = nullptr;
        }
    } catch (const TestException& e) {
        cout << " fail" << endl;
        cerr << e.file() << ':' << e.line() << ": " << e.what() << endl;
        exit(1);
    } catch (const dbr::DbrException& e) {
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
