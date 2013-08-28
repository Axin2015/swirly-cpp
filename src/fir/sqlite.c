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
#define _XOPEN_SOURCE 700 // strnlen()
#include "sqlite.h"

#include "sqlite3.h"

#include <dbr/conv.h>
#include <dbr/err.h>
#include <dbr/journ.h>
#include <dbr/log.h>
#include <dbr/model.h>
#include <dbr/queue.h>
#include <dbr/sqlite.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define INSERT_ORDER_SQL                                                \
    "INSERT INTO order_ (id, rev, status, trader, accnt, contr,"        \
    " settl_date, ref, action, ticks, resd, exec, lots, min, flags,"    \
    " archive, created, modified)"                                      \
    " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 0, ?, ?)"

#define UPDATE_ORDER_SQL                                                \
    "UPDATE order_ SET rev = ?, status = ?, resd = ?, exec = ?,"        \
    " lots = ?,  modified = ?"                                          \
    " WHERE id = ?"

#define ARCHIVE_ORDER_SQL                                             \
    "UPDATE order_ SET archive = 1, modified = ?"                     \
    " WHERE id = ?"

#define INSERT_TRADE_SQL                                                \
    "INSERT INTO trade (id, match, order_, order_rev, cpty, role,"      \
    " action, ticks, resd, exec, lots, archive, created, modified)"                                                        \
    " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 0, ?, ?)"

#define ARCHIVE_TRADE_SQL                                               \
    "UPDATE trade SET archive = 1, modified = ?"                        \
    " WHERE id = ?"

#define SELECT_CONTR_SQL                                                \
    "SELECT id, mnem, display, asset_type, asset, ccy, tick_numer,"     \
    " tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots"     \
    " FROM contr_v"

#define SELECT_TRADER_SQL                                    \
    "SELECT id, mnem, display, email"                        \
    " FROM trader_v"

#define SELECT_ACCNT_SQL                                      \
    "SELECT id, mnem, display, email"                         \
    " FROM accnt_v"

#define SELECT_ORDER_SQL                                                \
    "SELECT id, rev, status, trader, accnt, contr, settl_date, ref,"    \
    " action, ticks, resd, exec, lots, min, flags, created, modified"   \
    " FROM order_ WHERE archive = 0 ORDER BY id"

#define SELECT_MEMB_SQL                                    \
    "SELECT accnt, trader"                                 \
    " FROM memb ORDER BY accnt"

#define SELECT_TRADE_SQL                                                \
    "SELECT id, match, order_, order_rev, trader, accnt, contr, settl_date," \
    " ref, cpty, role, action, ticks, resd, exec, lots, created, modified" \
    " FROM trade_v WHERE archive = 0 ORDER BY id"

#define SELECT_POSN_SQL                                         \
    "SELECT accnt, contr, settl_date, action, licks, lots"      \
    " FROM posn_v ORDER BY accnt, contr, settl_date, action"

// Only called if failure occurs during cache load, so no need to free state members as they will
// not have been allocated.

static sqlite3_stmt*
prepare(sqlite3* db, const char* sql)
{
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare(db, sql, -1, &stmt, NULL);
    if (dbr_unlikely(rc != SQLITE_OK)) {
        dbr_err_set(DBR_EIO, sqlite3_errmsg(db));
        stmt = NULL;
    }
    return stmt;
}

static DbrBool
exec_sql(sqlite3* db, const char* sql)
{
    sqlite3_stmt* stmt = prepare(db, sql);
    if (!stmt)
        goto fail1;

    // SQLITE_ROW  100
    // SQLITE_DONE 101

    if (dbr_unlikely(sqlite3_step(stmt) < SQLITE_ROW)) {
        dbr_err_set(DBR_EIO, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        goto fail1;
    }

    sqlite3_finalize(stmt);
    return true;
 fail1:
    return false;
}

static DbrBool
exec_stmt(sqlite3* db, sqlite3_stmt* stmt)
{
    int rc = sqlite3_step(stmt);
    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
    if (dbr_unlikely(rc != SQLITE_DONE)) {
        dbr_err_set(DBR_EIO, sqlite3_errmsg(db));
        return false;
    }
    return true;
}

#if DBR_DEBUG_LEVEL >= 2
static void
trace_sql(void* unused, const char* sql)
{
    dbr_log_debug1("%s", sql);
}
#endif // DBR_DEBUG_LEVEL >= 2

static inline int
bind_text(sqlite3_stmt* stmt, int col, const char* text, size_t maxlen)
{
    return sqlite3_bind_text(stmt, col, text, strnlen(text, maxlen), SQLITE_STATIC);
}

static DbrBool
bind_insert_order(struct FirSqlite* sqlite, const struct DbrOrder* order)
{
    enum {
        ID = 1,
        REV,
        STATUS,
        TRADER,
        ACCNT,
        CONTR,
        SETTL_DATE,
        REF,
        ACTION,
        TICKS,
        RESD,
        EXEC,
        LOTS,
        MIN,
        FLAGS,
        CREATED,
        MODIFIED
    };
    sqlite3_stmt* stmt = sqlite->insert_order;
    int rc = sqlite3_bind_int64(stmt, ID, order->id);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int(stmt, REV, order->rev);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int(stmt, STATUS, order->status);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, TRADER, order->trader.rec->id);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, ACCNT, order->accnt.rec->id);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, CONTR, order->contr.rec->id);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int(stmt, SETTL_DATE, order->settl_date);
    if (rc != SQLITE_OK)
        goto fail1;

    if (order->ref[0] != '\0')
        rc = bind_text(stmt, REF, order->ref, DBR_REF_MAX);
    else
        rc = sqlite3_bind_null(stmt, REF);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int(stmt, ACTION, order->action);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, TICKS, order->ticks);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, RESD, order->resd);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, EXEC, order->exec);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, LOTS, order->lots);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, MIN, order->min);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, FLAGS, order->flags);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, CREATED, order->created);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, MODIFIED, order->modified);
    if (rc != SQLITE_OK)
        goto fail1;

    return true;
 fail1:
    dbr_err_set(DBR_EIO, sqlite3_errmsg(sqlite->db));
    sqlite3_clear_bindings(stmt);
    return false;
}

static DbrBool
bind_update_order(struct FirSqlite* sqlite, DbrIden id, int rev, int status,
                  DbrLots resd, DbrLots exec, DbrLots lots, DbrMillis now)
{
    enum {
        REV = 1,
        STATUS,
        RESD,
        EXEC,
        LOTS,
        MODIFIED,
        ID
    };
    sqlite3_stmt* stmt = sqlite->update_order;
    int rc = sqlite3_bind_int(stmt, REV, rev);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int(stmt, STATUS, status);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, RESD, resd);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, EXEC, exec);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, LOTS, lots);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, MODIFIED, now);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, ID, id);
    if (rc != SQLITE_OK)
        goto fail1;

    return true;
 fail1:
    dbr_err_set(DBR_EIO, sqlite3_errmsg(sqlite->db));
    sqlite3_clear_bindings(stmt);
    return false;
}

static DbrBool
bind_archive_order(struct FirSqlite* sqlite, DbrIden id, DbrMillis now)
{
    enum {
        MODIFIED = 1,
        ID
    };
    sqlite3_stmt* stmt = sqlite->archive_order;
    int rc = sqlite3_bind_int64(stmt, MODIFIED, now);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, ID, id);
    if (rc != SQLITE_OK)
        goto fail1;

    return true;
 fail1:
    dbr_err_set(DBR_EIO, sqlite3_errmsg(sqlite->db));
    sqlite3_clear_bindings(stmt);
    return false;
}

static DbrBool
bind_insert_trade(struct FirSqlite* sqlite, const struct DbrTrade* trade)
{
    enum {
        ID = 1,
        MATCH,
        ORDER,
        ORDER_REV,
        CPTY,
        ROLE,
        ACTION,
        TICKS,
        RESD,
        EXEC,
        LOTS,
        CREATED,
        MODIFIED
    };
    sqlite3_stmt* stmt = sqlite->insert_trade;
    int rc = sqlite3_bind_int64(stmt, ID, trade->id);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, MATCH, trade->match);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, ORDER, trade->order);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int(stmt, ORDER_REV, trade->order_rev);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, CPTY, trade->cpty.rec->id);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int(stmt, ROLE, trade->role);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int(stmt, ACTION, trade->action);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, TICKS, trade->ticks);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, RESD, trade->resd);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, EXEC, trade->exec);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, LOTS, trade->lots);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, CREATED, trade->created);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, MODIFIED, trade->modified);
    if (rc != SQLITE_OK)
        goto fail1;

    return true;
 fail1:
    dbr_err_set(DBR_EIO, sqlite3_errmsg(sqlite->db));
    sqlite3_clear_bindings(stmt);
    return false;
}

static DbrBool
bind_archive_trade(struct FirSqlite* sqlite, DbrIden id, DbrMillis now)
{
    enum {
        MODIFIED = 1,
        ID
    };
    sqlite3_stmt* stmt = sqlite->archive_trade;
    int rc = sqlite3_bind_int64(stmt, MODIFIED, now);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, ID, id);
    if (rc != SQLITE_OK)
        goto fail1;

    return true;
 fail1:
    dbr_err_set(DBR_EIO, sqlite3_errmsg(sqlite->db));
    sqlite3_clear_bindings(stmt);
    return false;
}

static ssize_t
select_contr(struct FirSqlite* sqlite, struct DbrSlNode** first)
{
    enum {
        ID,
        MNEM,
        DISPLAY,
        ASSET_TYPE,
        ASSET,
        CCY,
        TICK_NUMER,
        TICK_DENOM,
        LOT_NUMER,
        LOT_DENOM,
        PIP_DP,
        MIN_LOTS,
        MAX_LOTS
    };

    sqlite3_stmt* stmt = prepare(sqlite->db, SELECT_CONTR_SQL);
    if (!stmt)
        goto fail1;

    struct DbrQueue rq;
    dbr_queue_init(&rq);

    ssize_t size = 0;
    for (;;) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {

            struct DbrRec* rec = dbr_pool_alloc_rec(sqlite->pool);
            if (!rec)
                goto fail2;

            // Header.

            rec->type = DBR_CONTR;
            rec->id = sqlite3_column_int64(stmt, ID);
            strncpy(rec->mnem,
                    (const char*)sqlite3_column_text(stmt, MNEM), DBR_MNEM_MAX);

            // Body.

            strncpy(rec->contr.display,
                    (const char*)sqlite3_column_text(stmt, DISPLAY), DBR_DISPLAY_MAX);
            strncpy(rec->contr.asset_type,
                    (const char*)sqlite3_column_text(stmt, ASSET_TYPE), DBR_MNEM_MAX);
            strncpy(rec->contr.asset,
                    (const char*)sqlite3_column_text(stmt, ASSET), DBR_MNEM_MAX);
            strncpy(rec->contr.ccy,
                    (const char*)sqlite3_column_text(stmt, CCY), DBR_MNEM_MAX);

            const int tick_numer = sqlite3_column_int(stmt, TICK_NUMER);
            const int tick_denom = sqlite3_column_int(stmt, TICK_DENOM);
            const double price_inc = dbr_fract_to_real(tick_numer, tick_denom);
            rec->contr.tick_numer = tick_numer;
            rec->contr.tick_denom = tick_denom;
            rec->contr.price_inc = price_inc;

            const int lot_numer = sqlite3_column_int(stmt, LOT_NUMER);
            const int lot_denom = sqlite3_column_int(stmt, LOT_DENOM);
            const double qty_inc = dbr_fract_to_real(lot_numer, lot_denom);
            rec->contr.lot_numer = lot_numer;
            rec->contr.lot_denom = lot_denom;
            rec->contr.qty_inc = qty_inc;

            rec->contr.price_dp = dbr_real_to_dp(price_inc);
            rec->contr.pip_dp = sqlite3_column_int(stmt, PIP_DP);
            rec->contr.qty_dp = dbr_real_to_dp(qty_inc);

            rec->contr.min_lots = sqlite3_column_int64(stmt, MIN_LOTS);
            rec->contr.max_lots = sqlite3_column_int64(stmt, MAX_LOTS);

            dbr_log_debug3("contr: id=%ld,mnem=%.16s,display=%.64s,asset_type=%.16s,"
                           "asset=%.16s,ccy=%.16s,price_inc=%f,qty_inc=%.2f,price_dp=%d,"
                           "pip_dp=%d,qty_dp=%d,min_lots=%ld,max_lots=%ld",
                           rec->id, rec->mnem, rec->contr.display, rec->contr.asset_type,
                           rec->contr.asset, rec->contr.ccy, rec->contr.price_inc,
                           rec->contr.qty_inc, rec->contr.price_dp, rec->contr.pip_dp,
                           rec->contr.qty_dp, rec->contr.min_lots, rec->contr.max_lots);

            dbr_queue_push(&rq, &rec->model_node_);
            ++size;

        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            dbr_err_set(DBR_EIO, sqlite3_errmsg(sqlite->db));
            goto fail2;
        }
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    *first = dbr_queue_first(&rq);
    return size;
 fail2:
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    dbr_pool_free_list(sqlite->pool, DBR_CONTR, dbr_queue_first(&rq));
    *first = NULL;
 fail1:
    return -1;
}

static ssize_t
select_trader(struct FirSqlite* sqlite, struct DbrSlNode** first)
{
    enum {
        ID,
        MNEM,
        DISPLAY,
        EMAIL
    };

    sqlite3_stmt* stmt = prepare(sqlite->db, SELECT_TRADER_SQL);
    if (!stmt)
        goto fail1;

    struct DbrQueue rq;
    dbr_queue_init(&rq);

    size_t size = 0;
    for (;;) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {

            struct DbrRec* rec = dbr_pool_alloc_rec(sqlite->pool);
            if (!rec)
                goto fail2;

            // Header.

            rec->type = DBR_TRADER;
            rec->id = sqlite3_column_int64(stmt, ID);
            strncpy(rec->mnem,
                    (const char*)sqlite3_column_text(stmt, MNEM), DBR_MNEM_MAX);

            // Body.

            strncpy(rec->trader.display,
                    (const char*)sqlite3_column_text(stmt, DISPLAY), DBR_DISPLAY_MAX);
            strncpy(rec->trader.email,
                    (const char*)sqlite3_column_text(stmt, EMAIL), DBR_EMAIL_MAX);
            rec->trader.state = NULL;

            dbr_log_debug3("trader: id=%ld,mnem=%.16s,display=%.64s,email=%.64s",
                           rec->id, rec->mnem, rec->trader.display, rec->trader.email);

            dbr_queue_push(&rq, &rec->model_node_);
            ++size;

        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            dbr_err_set(DBR_EIO, sqlite3_errmsg(sqlite->db));
            goto fail2;
        }
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    *first = dbr_queue_first(&rq);
    return size;
 fail2:
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    dbr_pool_free_list(sqlite->pool, DBR_TRADER, dbr_queue_first(&rq));
    *first = NULL;
 fail1:
    return -1;
}

static ssize_t
select_accnt(struct FirSqlite* sqlite, struct DbrSlNode** first)
{
    enum {
        ID,
        MNEM,
        DISPLAY,
        EMAIL
    };

    sqlite3_stmt* stmt = prepare(sqlite->db, SELECT_ACCNT_SQL);
    if (!stmt)
        goto fail1;

    struct DbrQueue rq;
    dbr_queue_init(&rq);

    size_t size = 0;
    for (;;) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {

            struct DbrRec* rec = dbr_pool_alloc_rec(sqlite->pool);
            if (!rec)
                goto fail2;

            // Header.

            rec->type = DBR_ACCNT;
            rec->id = sqlite3_column_int64(stmt, ID);
            strncpy(rec->mnem,
                    (const char*)sqlite3_column_text(stmt, MNEM), DBR_MNEM_MAX);

            // Body.

            strncpy(rec->accnt.display,
                    (const char*)sqlite3_column_text(stmt, DISPLAY), DBR_DISPLAY_MAX);
            strncpy(rec->accnt.email,
                    (const char*)sqlite3_column_text(stmt, EMAIL), DBR_EMAIL_MAX);
            rec->accnt.state = NULL;

            dbr_log_debug3("accnt: id=%ld,mnem=%.16s,display=%.64s,email=%.64s",
                           rec->id, rec->mnem, rec->accnt.display, rec->accnt.email);

            dbr_queue_push(&rq, &rec->model_node_);
            ++size;

        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            dbr_err_set(DBR_EIO, sqlite3_errmsg(sqlite->db));
            goto fail2;
        }
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    *first = dbr_queue_first(&rq);
    return size;
 fail2:
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    dbr_pool_free_list(sqlite->pool, DBR_ACCNT, dbr_queue_first(&rq));
    *first = NULL;
 fail1:
    return -1;
}

static ssize_t
select_order(struct FirSqlite* sqlite, struct DbrSlNode** first)
{
    enum {
        ID,
        REV,
        STATUS,
        TRADER,
        ACCNT,
        CONTR,
        SETTL_DATE,
        REF,
        ACTION,
        TICKS,
        RESD,
        EXEC,
        LOTS,
        MIN,
        FLAGS,
        CREATED,
        MODIFIED
    };

    sqlite3_stmt* stmt = prepare(sqlite->db, SELECT_ORDER_SQL);
    if (!stmt)
        goto fail1;

    struct DbrQueue oq;
    dbr_queue_init(&oq);

    ssize_t size = 0;
    for (;;) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {

            const DbrIden id = sqlite3_column_int64(stmt, ID);
            struct DbrOrder* order = dbr_pool_alloc_order(sqlite->pool, id);
            if (!order)
                goto fail2;

            order->id = id;
            order->level = NULL;
            order->rev = sqlite3_column_int(stmt, REV);
            order->status = sqlite3_column_int(stmt, STATUS);
            order->trader.id = sqlite3_column_int64(stmt, TRADER);
            order->accnt.id = sqlite3_column_int64(stmt, ACCNT);
            order->contr.id = sqlite3_column_int64(stmt, CONTR);
            order->settl_date = sqlite3_column_int(stmt, SETTL_DATE);
            if (sqlite3_column_type(stmt, REF) != SQLITE_NULL)
                strncpy(order->ref,
                        (const char*)sqlite3_column_text(stmt, REF), DBR_REF_MAX);
            else
                order->ref[0] = '\0';
            order->action = sqlite3_column_int(stmt, ACTION);
            order->ticks = sqlite3_column_int64(stmt, TICKS);
            order->resd = sqlite3_column_int64(stmt, RESD);
            order->exec = sqlite3_column_int64(stmt, EXEC);
            order->lots = sqlite3_column_int64(stmt, LOTS);
            order->min = sqlite3_column_int64(stmt, MIN);
            order->flags = sqlite3_column_int64(stmt, FLAGS);
            order->created = sqlite3_column_int64(stmt, CREATED);
            order->modified = sqlite3_column_int64(stmt, MODIFIED);

            dbr_log_debug3("order: id=%ld,rev=%d,status=%d,trader=%ld,accnt=%ld,contr=%ld,"
                           "settl_date=%d,ref=%.64s,action=%d,ticks=%ld,resd=%ld,exec=%ld,"
                           "lots=%ld,min=%ld,flags=%ld,created=%ld,modified=%ld",
                           order->id, order->rev, order->status, order->trader.id, order->accnt.id,
                           order->contr.id, order->settl_date, order->ref, order->action,
                           order->ticks, order->resd, order->exec, order->lots, order->min,
                           order->flags, order->created, order->modified);

            dbr_queue_push(&oq, &order->model_node_);
            ++size;

        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            dbr_err_set(DBR_EIO, sqlite3_errmsg(sqlite->db));
            goto fail2;
        }
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    *first = dbr_queue_first(&oq);
    return size;
 fail2:
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    dbr_pool_free_list(sqlite->pool, DBR_ORDER, dbr_queue_first(&oq));
    *first = NULL;
 fail1:
    return -1;
}

static ssize_t
select_memb(struct FirSqlite* sqlite, struct DbrSlNode** first)
{
    enum {
        ACCNT,
        TRADER
    };

    sqlite3_stmt* stmt = prepare(sqlite->db, SELECT_MEMB_SQL);
    if (!stmt)
        goto fail1;

    struct DbrQueue mq;
    dbr_queue_init(&mq);

    ssize_t size = 0;
    for (;;) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {

            const DbrIden trader = sqlite3_column_int64(stmt, TRADER);
            struct DbrMemb* memb = dbr_pool_alloc_memb(sqlite->pool, trader);
            if (!memb)
                goto fail2;

            memb->accnt.id = sqlite3_column_int64(stmt, ACCNT);
            memb->trader.id = trader;

            dbr_log_debug3("memb: accnt=%ld,trader=%ld", memb->accnt.id, memb->trader.id);

            dbr_queue_push(&mq, &memb->model_node_);
            ++size;

        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            dbr_err_set(DBR_EIO, sqlite3_errmsg(sqlite->db));
            goto fail2;
        }
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    *first = dbr_queue_first(&mq);
    return size;
 fail2:
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    dbr_pool_free_list(sqlite->pool, DBR_MEMB, dbr_queue_first(&mq));
    *first = NULL;
 fail1:
    return -1;
}

static ssize_t
select_trade(struct FirSqlite* sqlite, struct DbrSlNode** first)
{
    enum {
        ID,
        MATCH,
        ORDER,
        ORDER_REV,
        TRADER,
        ACCNT,
        CONTR,
        SETTL_DATE,
        REF,
        CPTY,
        ROLE,
        ACTION,
        TICKS,
        RESD,
        EXEC,
        LOTS,
        CREATED,
        MODIFIED
    };

    sqlite3_stmt* stmt = prepare(sqlite->db, SELECT_TRADE_SQL);
    if (!stmt)
        goto fail1;

    struct DbrQueue tq;
    dbr_queue_init(&tq);

    ssize_t size = 0;
    for (;;) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {

            const DbrIden id = sqlite3_column_int64(stmt, ID);
            struct DbrTrade* trade = dbr_pool_alloc_trade(sqlite->pool, id);
            if (!trade)
                goto fail2;

            trade->id = id;
            trade->match = sqlite3_column_int64(stmt, MATCH);
            trade->order = sqlite3_column_int64(stmt, ORDER);
            trade->order_rev = sqlite3_column_int(stmt, ORDER_REV);
            trade->trader.id = sqlite3_column_int64(stmt, TRADER);
            trade->accnt.id = sqlite3_column_int64(stmt, ACCNT);
            trade->contr.id = sqlite3_column_int64(stmt, CONTR);
            trade->settl_date = sqlite3_column_int(stmt, SETTL_DATE);
            if (sqlite3_column_type(stmt, REF) != SQLITE_NULL)
                strncpy(trade->ref,
                        (const char*)sqlite3_column_text(stmt, REF), DBR_REF_MAX);
            else
                trade->ref[0] = '\0';
            trade->cpty.id = sqlite3_column_int64(stmt, CPTY);
            trade->role = sqlite3_column_int(stmt, ROLE);
            trade->action = sqlite3_column_int(stmt, ACTION);
            trade->ticks = sqlite3_column_int64(stmt, TICKS);
            trade->resd = sqlite3_column_int64(stmt, RESD);
            trade->exec = sqlite3_column_int64(stmt, EXEC);
            trade->lots = sqlite3_column_int64(stmt, LOTS);
            trade->created = sqlite3_column_int64(stmt, CREATED);
            trade->modified = sqlite3_column_int64(stmt, MODIFIED);

            dbr_log_debug3("trade: id=%ld,match=%ld,order=%ld,order_rev=%d,trader=%ld,accnt=%ld,"
                           "contr=%ld,settl_date=%d,ref=%.64s,cpty=%ld,role=%d,action=%d,"
                           "ticks=%ld,resd=%ld,exec=%ld,lots=%ld,settl_date=%d,created=%ld,"
                           "modified=%ld",
                           trade->id, trade->match, trade->order, trade->order_rev,
                           trade->trader.id, trade->accnt.id, trade->contr.id, trade->settl_date,
                           trade->ref, trade->cpty.id, trade->role, trade->action, trade->ticks,
                           trade->resd, trade->exec, trade->lots, trade->settl_date,
                           trade->created, trade->modified);

            dbr_queue_push(&tq, &trade->model_node_);
            ++size;

        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            dbr_err_set(DBR_EIO, sqlite3_errmsg(sqlite->db));
            goto fail2;
        }
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    *first = dbr_queue_first(&tq);
    return size;
 fail2:
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    dbr_pool_free_list(sqlite->pool, DBR_TRADE, dbr_queue_first(&tq));
    *first = NULL;
 fail1:
    return -1;
}

static ssize_t
select_posn(struct FirSqlite* sqlite, struct DbrSlNode** first)
{
    enum {
        ID,
        ACCNT,
        CONTR,
        SETTL_DATE,
        ACTION,
        LICKS,
        LOTS
    };

    sqlite3_stmt* stmt = prepare(sqlite->db, SELECT_POSN_SQL);
    if (!stmt)
        goto fail1;

    struct DbrQueue pq;
    dbr_queue_init(&pq);

    struct DbrPosn* posn = NULL;

    ssize_t size = 0;
    for (;;) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {

            const DbrIden accnt = sqlite3_column_int64(stmt, ACCNT);
            const DbrIden contr = sqlite3_column_int64(stmt, CONTR);
            const int settl_date = sqlite3_column_int(stmt, SETTL_DATE);

            // Posn is null for first row.
            if (posn && posn->accnt.id == accnt && posn->contr.id == contr
                && posn->settl_date == settl_date) {

                // Set other side.

                const int action = sqlite3_column_int(stmt, ACTION);
                if (action == DBR_BUY) {
                    posn->buy_licks = sqlite3_column_int64(stmt, LICKS);
                    posn->buy_lots = sqlite3_column_int64(stmt, LOTS);
                } else {
                    assert(action == DBR_SELL);
                    posn->sell_licks = sqlite3_column_int64(stmt, LICKS);
                    posn->sell_lots = sqlite3_column_int64(stmt, LOTS);
                }
                continue;
            }

            // Synthetic id from contract and settlment date.
            const DbrIden key = contr * 100000000L + settl_date;
            posn = dbr_pool_alloc_posn(sqlite->pool, key);
            if (dbr_unlikely(!posn))
                goto fail2;

            posn->accnt.id = accnt;
            posn->contr.id = contr;
            posn->settl_date = settl_date;

            const int action = sqlite3_column_int(stmt, ACTION);
            if (action == DBR_BUY) {
                posn->buy_licks = sqlite3_column_int64(stmt, LICKS);
                posn->buy_lots = sqlite3_column_int64(stmt, LOTS);
                posn->sell_licks = 0;
                posn->sell_lots = 0;
            } else {
                assert(action == DBR_SELL);
                posn->buy_licks = 0;
                posn->buy_lots = 0;
                posn->sell_licks = sqlite3_column_int64(stmt, LICKS);
                posn->sell_lots = sqlite3_column_int64(stmt, LOTS);
            }

            dbr_queue_push(&pq, &posn->model_node_);
            ++size;

        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            dbr_err_set(DBR_EIO, sqlite3_errmsg(sqlite->db));
            goto fail2;
        }
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    *first = dbr_queue_first(&pq);
    return size;
 fail2:
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    dbr_pool_free_list(sqlite->pool, DBR_POSN, dbr_queue_first(&pq));
    *first = NULL;
 fail1:
    return -1;
}

DBR_EXTERN DbrBool
fir_sqlite_init(struct FirSqlite* sqlite, DbrPool pool, const char* path)
{
    sqlite3* db;
    int rc = sqlite3_open_v2(path, &db, SQLITE_OPEN_READWRITE, NULL);
    if (dbr_unlikely(rc != SQLITE_OK)) {
        dbr_err_set(DBR_EIO, sqlite3_errmsg(db));
        // Must close even if open failed.
        goto fail1;
    }

    // Install trace function for debugging.
#if DBR_DEBUG_LEVEL >= 2
    sqlite3_trace(db, trace_sql, NULL);
#endif // DBR_DEBUG_LEVEL >= 2

#if DBR_DEBUG_LEVEL >= 1
    rc = sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, 1, NULL);
    if (dbr_unlikely(rc != SQLITE_OK)) {
        dbr_err_set(DBR_EIO, sqlite3_errmsg(db));
        goto fail1;
    }
#endif // DBR_DEBUG_LEVEL >= 1

    if (dbr_unlikely(!exec_sql(db, "PRAGMA synchronous = OFF")
                     || !exec_sql(db, "PRAGMA journal_mode = MEMORY"))) {
        dbr_err_set(DBR_EIO, sqlite3_errmsg(db));
        goto fail1;
    }

    sqlite3_stmt* insert_order = prepare(db, INSERT_ORDER_SQL);
    if (!insert_order)
        goto fail1;

    sqlite3_stmt* update_order = prepare(db, UPDATE_ORDER_SQL);
    if (!update_order)
        goto fail2;

    sqlite3_stmt* archive_order = prepare(db, ARCHIVE_ORDER_SQL);
    if (!archive_order)
        goto fail3;

    sqlite3_stmt* insert_trade = prepare(db, INSERT_TRADE_SQL);
    if (!insert_trade)
        goto fail4;

    sqlite3_stmt* archive_trade = prepare(db, ARCHIVE_TRADE_SQL);
    if (!archive_trade)
        goto fail5;

    sqlite->pool = pool;
    sqlite->db = db;
    sqlite->insert_order = insert_order;
    sqlite->update_order = update_order;
    sqlite->archive_order = archive_order;
    sqlite->insert_trade = insert_trade;
    sqlite->archive_trade = archive_trade;
    return true;

 fail5:
    sqlite3_finalize(insert_trade);
 fail4:
    sqlite3_finalize(archive_order);
 fail3:
    sqlite3_finalize(update_order);
 fail2:
    sqlite3_finalize(insert_order);
 fail1:
    sqlite3_close(db);
    return false;
}

DBR_EXTERN void
fir_sqlite_term(struct FirSqlite* sqlite)
{
    assert(sqlite);
    sqlite3_finalize(sqlite->archive_trade);
    sqlite3_finalize(sqlite->insert_trade);
    sqlite3_finalize(sqlite->archive_order);
    sqlite3_finalize(sqlite->update_order);
    sqlite3_finalize(sqlite->insert_order);
    sqlite3_close(sqlite->db);
}

DBR_EXTERN DbrBool
fir_sqlite_begin_trans(struct FirSqlite* sqlite)
{
    return exec_sql(sqlite->db, "BEGIN TRANSACTION");
}

DBR_EXTERN DbrBool
fir_sqlite_commit_trans(struct FirSqlite* sqlite)
{
    return exec_sql(sqlite->db, "COMMIT TRANSACTION");
}

DBR_EXTERN DbrBool
fir_sqlite_rollback_trans(struct FirSqlite* sqlite)
{
    return exec_sql(sqlite->db, "ROLLBACK TRANSACTION");
}

DBR_EXTERN DbrBool
fir_sqlite_insert_order(struct FirSqlite* sqlite, const struct DbrOrder* order)
{
    return bind_insert_order(sqlite, order)
        && exec_stmt(sqlite->db, sqlite->insert_order);
}

DBR_EXTERN DbrBool
fir_sqlite_update_order(struct FirSqlite* sqlite, DbrIden id, int rev, int status,
                        DbrLots resd, DbrLots exec, DbrLots lots, DbrMillis now)
{
    return bind_update_order(sqlite, id, rev, status, resd, exec, lots, now)
        && exec_stmt(sqlite->db, sqlite->update_order);
}

DBR_EXTERN DbrBool
fir_sqlite_archive_order(struct FirSqlite* sqlite, DbrIden id, DbrMillis now)
{
    return bind_archive_order(sqlite, id, now)
        && exec_stmt(sqlite->db, sqlite->archive_order);
}

DBR_EXTERN DbrBool
fir_sqlite_insert_trade(struct FirSqlite* sqlite, const struct DbrTrade* trade)
{
    return bind_insert_trade(sqlite, trade)
        && exec_stmt(sqlite->db, sqlite->insert_trade);
}

DBR_EXTERN DbrBool
fir_sqlite_archive_trade(struct FirSqlite* sqlite, DbrIden id, DbrMillis now)
{
    return bind_archive_trade(sqlite, id, now)
        && exec_stmt(sqlite->db, sqlite->archive_trade);
}

DBR_EXTERN ssize_t
fir_sqlite_select_entity(struct FirSqlite* sqlite, int type, struct DbrSlNode** first)
{
    ssize_t ret;
    switch (type) {
    case DBR_CONTR:
        ret = select_contr(sqlite, first);
        break;
    case DBR_TRADER:
        ret = select_trader(sqlite, first);
        break;
    case DBR_ACCNT:
        ret = select_accnt(sqlite, first);
        break;
    case DBR_ORDER:
        ret = select_order(sqlite, first);
        break;
    case DBR_MEMB:
        ret = select_memb(sqlite, first);
        break;
    case DBR_TRADE:
        ret = select_trade(sqlite, first);
        break;
    case DBR_POSN:
        ret = select_posn(sqlite, first);
        break;
    default:
        dbr_err_set(DBR_EINVAL, "invalid type '%d'", type);
        *first = NULL;
        ret = -1;
    }
    return ret;
}

DBR_EXTERN struct DbrSlNode*
fir_sqlite_end_entity(struct FirSqlite* sqlite)
{
    return NULL;
}

struct DbrSqlite_ {
    DbrIden id;
    struct FirSqlite impl;
    struct DbrIJourn journ_;
    struct DbrIModel model_;
};

static inline struct DbrSqlite_*
journ_implof(DbrJourn journ)
{
    return dbr_implof(struct DbrSqlite_, journ_, journ);
}

static inline struct DbrSqlite_*
model_implof(DbrModel model)
{
    return dbr_implof(struct DbrSqlite_, model_, model);
}

static DbrIden
alloc_id(DbrJourn journ)
{
    struct DbrSqlite_* sqlite = journ_implof(journ);
    return sqlite->id++;
}

static DbrBool
begin_trans(DbrJourn journ)
{
    struct DbrSqlite_* sqlite = journ_implof(journ);
    struct FirSqlite* impl = &sqlite->impl;
    return fir_sqlite_begin_trans(impl);
}

static DbrBool
commit_trans(DbrJourn journ)
{
    struct DbrSqlite_* sqlite = journ_implof(journ);
    struct FirSqlite* impl = &sqlite->impl;
    return fir_sqlite_commit_trans(impl);
}

static DbrBool
rollback_trans(DbrJourn journ)
{
    struct DbrSqlite_* sqlite = journ_implof(journ);
    struct FirSqlite* impl = &sqlite->impl;
    return fir_sqlite_rollback_trans(impl);
}

static DbrBool
insert_order(DbrJourn journ, struct DbrOrder* order)
{
    struct DbrSqlite_* sqlite = journ_implof(journ);
    struct FirSqlite* impl = &sqlite->impl;
    return fir_sqlite_insert_order(impl, order);
}

static DbrBool
update_order(DbrJourn journ, DbrIden id, int rev, int status, DbrLots resd, DbrLots exec,
             DbrLots lots, DbrMillis now)
{
    struct DbrSqlite_* sqlite = journ_implof(journ);
    struct FirSqlite* impl = &sqlite->impl;
    return fir_sqlite_update_order(impl, id, rev, status, resd, exec, lots, now);
}

static DbrBool
archive_order(DbrJourn journ, DbrIden id, DbrMillis now)
{
    struct DbrSqlite_* sqlite = journ_implof(journ);
    struct FirSqlite* impl = &sqlite->impl;
    return fir_sqlite_archive_order(impl, id, now);
}

static DbrBool
insert_trade(DbrJourn journ, struct DbrTrade* trade)
{
    struct DbrSqlite_* sqlite = journ_implof(journ);
    struct FirSqlite* impl = &sqlite->impl;
    return fir_sqlite_insert_trade(impl, trade);
}

static DbrBool
archive_trade(DbrJourn journ, DbrIden id, DbrMillis now)
{
    struct DbrSqlite_* sqlite = journ_implof(journ);
    struct FirSqlite* impl = &sqlite->impl;
    return fir_sqlite_archive_trade(impl, id, now);
}

static const struct DbrJournVtbl JOURN_VTBL = {
    .alloc_id = alloc_id,
    .begin_trans = begin_trans,
    .commit_trans = commit_trans,
    .rollback_trans = rollback_trans,
    .insert_order = insert_order,
    .update_order = update_order,
    .archive_order = archive_order,
    .insert_trade = insert_trade,
    .archive_trade = archive_trade
};

static ssize_t
read_entity(DbrModel model, int type, struct DbrSlNode** first)
{
    struct DbrSqlite_* sqlite = model_implof(model);
    struct FirSqlite* impl = &sqlite->impl;
    return fir_sqlite_select_entity(impl, type, first);
}

static struct DbrSlNode*
end_entity(DbrModel model)
{
    struct DbrSqlite_* sqlite = model_implof(model);
    struct FirSqlite* impl = &sqlite->impl;
    return fir_sqlite_end_entity(impl);
}

static const struct DbrModelVtbl MODEL_VTBL = {
    .read_entity = read_entity,
    .end_entity = end_entity
};

DBR_API DbrSqlite
dbr_sqlite_create(DbrPool pool, DbrIden seed, const char* path)
{
    struct DbrSqlite_* sqlite = malloc(sizeof(struct DbrSqlite_));
    if (dbr_unlikely(!sqlite))
        goto fail1;

    // Seed identity.
    sqlite->id = seed;

    if (!fir_sqlite_init(&sqlite->impl, pool, path))
        goto fail2;

    sqlite->journ_.vtbl = &JOURN_VTBL;
    sqlite->model_.vtbl = &MODEL_VTBL;
    return sqlite;
 fail2:
    free(sqlite);
 fail1:
    return NULL;
}

DBR_API void
dbr_sqlite_destroy(DbrSqlite sqlite)
{
    if (sqlite) {
        fir_sqlite_term(&sqlite->impl);
        free(sqlite);
    }
}

DBR_API DbrJourn
dbr_sqlite_journ(DbrSqlite sqlite)
{
    return &sqlite->journ_;
}

DBR_API DbrModel
dbr_sqlite_model(DbrSqlite sqlite)
{
    return &sqlite->model_;
}
