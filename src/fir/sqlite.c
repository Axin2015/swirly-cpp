/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "sqlite.h"

#include "sqlite3.h"

#include <sc/elm/conv.h>
#include <sc/elm/pool.h>
#include <sc/ash/err.h>

#include <sc/ash/log.h>
#include <sc/ash/queue.h>

#include <string.h>

#define INSERT_EXEC_SQL                                                 \
    "INSERT INTO exec (id, order_, trader, giveup, contr, settl_day,"   \
    " ref, state, action, ticks, lots, resd, exec, last_ticks,"         \
    " last_lots, min_lots, match, acked, role, cpty, created,"          \
    " modified)"                                                        \
    " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 0,"    \
    " ?, ?, ?, ?)"

#define UPDATE_EXEC_SQL                                                 \
    "UPDATE exec SET acked = 1, modified = ?"                           \
    " WHERE id = ?"

#define SELECT_ACCNT_SQL                                                \
    "SELECT id, mnem, display, email"                                   \
    " FROM accnt_v ORDER BY id"

#define SELECT_CONTR_SQL                                                \
    "SELECT id, mnem, display, asset_type, asset, ccy, tick_numer,"     \
    " tick_denom, lot_numer, lot_denom, pip_dp, min_lots, max_lots"     \
    " FROM contr_v ORDER BY id"

#define SELECT_PERM_SQL                                                 \
    "SELECT trader, giveup"                                             \
    " FROM perm ORDER BY trader"

#define SELECT_ORDER_SQL                                                \
    "SELECT id, trader, giveup, contr, settl_day, ref, state,"          \
    " action, ticks, lots, resd, exec, last_ticks, last_lots,"          \
    " min_lots, created, modified"                                      \
    " FROM order_ WHERE resd > 0 ORDER BY id"

#define SELECT_TRADE_SQL                                                \
    "SELECT id, order_, trader, giveup, contr, settl_day, ref,"         \
    " action, ticks, lots, resd, exec, last_ticks, last_lots,"          \
    " min_lots, match, role, cpty, created"                             \
    " FROM trade_v WHERE acked = 0 ORDER BY id"

#define SELECT_POSN_SQL                                                 \
    "SELECT accnt, contr, settl_day, action, licks, lots"               \
    " FROM posn_v ORDER BY accnt, contr, settl_day, action"

// Only called if failure occurs during cache load, so no need to free state members as they will
// not have been allocated.

static sqlite3_stmt*
prepare(sqlite3* db, const char* sql)
{
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare(db, sql, -1, &stmt, NULL);
    if (sc_unlikely(rc != SQLITE_OK)) {
        sc_err_set(SC_EIO, sqlite3_errmsg(db));
        stmt = NULL;
    }
    return stmt;
}

static ScBool
exec_sql(sqlite3* db, const char* sql)
{
    sqlite3_stmt* stmt = prepare(db, sql);
    if (!stmt)
        goto fail1;

    // SQLITE_ROW  100
    // SQLITE_DONE 101

    if (sc_unlikely(sqlite3_step(stmt) < SQLITE_ROW)) {
        sc_err_set(SC_EIO, sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        goto fail1;
    }

    sqlite3_finalize(stmt);
    return SC_TRUE;
 fail1:
    return SC_FALSE;
}

static ScBool
exec_stmt(sqlite3* db, sqlite3_stmt* stmt)
{
    int rc = sqlite3_step(stmt);
    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
    if (sc_unlikely(rc != SQLITE_DONE)) {
        sc_err_set(SC_EIO, sqlite3_errmsg(db));
        return SC_FALSE;
    }
    return SC_TRUE;
}

#if SC_DEBUG_LEVEL >= 2
static void
trace_sql(void* unused, const char* sql)
{
    sc_log_debug2("%s", sql);
}
#endif // SC_DEBUG_LEVEL >= 2

static inline int
bind_text(sqlite3_stmt* stmt, int col, const char* text, size_t maxlen)
{
    return sqlite3_bind_text(stmt, col, text, strnlen(text, maxlen), SQLITE_STATIC);
}

static ScBool
bind_insert_exec(struct FirSqlite* sqlite, const struct ScExec* exec, ScBool enriched)
{
    enum {
        ID = 1,
        ORDER,
        TRADER,
        GIVEUP,
        CONTR,
        SETTL_DAY,
        REF,
        STATE,
        ACTION,
        TICKS,
        LOTS,
        RESD,
        EXEC,
        LAST_TICKS,
        LAST_LOTS,
        MIN_LOTS,
        MATCH,
        ROLE,
        CPTY,
        CREATED,
        MODIFIED
    };
    sqlite3_stmt* stmt = sqlite->insert_exec;
    int rc = sqlite3_bind_int64(stmt, ID, exec->id);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, ORDER, exec->order);
    if (rc != SQLITE_OK)
        goto fail1;

    if (enriched) {

        rc = sqlite3_bind_int64(stmt, TRADER, exec->i.trader.rec->id);
        if (rc != SQLITE_OK)
            goto fail1;

        rc = sqlite3_bind_int64(stmt, GIVEUP, exec->i.giveup.rec->id);
        if (rc != SQLITE_OK)
            goto fail1;

        rc = sqlite3_bind_int64(stmt, CONTR, exec->i.contr.rec->id);
        if (rc != SQLITE_OK)
            goto fail1;

    } else {

        rc = sqlite3_bind_int64(stmt, TRADER, exec->i.trader.id_only);
        if (rc != SQLITE_OK)
            goto fail1;

        rc = sqlite3_bind_int64(stmt, GIVEUP, exec->i.giveup.id_only);
        if (rc != SQLITE_OK)
            goto fail1;

        rc = sqlite3_bind_int64(stmt, CONTR, exec->i.contr.id_only);
        if (rc != SQLITE_OK)
            goto fail1;
    }

    rc = sqlite3_bind_int(stmt, SETTL_DAY, exec->i.settl_day);
    if (rc != SQLITE_OK)
        goto fail1;

    if (exec->i.ref[0] != '\0')
        rc = bind_text(stmt, REF, exec->i.ref, SC_REF_MAX);
    else
        rc = sqlite3_bind_null(stmt, REF);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int(stmt, STATE, exec->i.state);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int(stmt, ACTION, exec->i.action);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, TICKS, exec->i.ticks);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, LOTS, exec->i.lots);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, RESD, exec->i.resd);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, EXEC, exec->i.exec);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, LAST_TICKS, exec->i.last_ticks);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, LAST_LOTS, exec->i.last_lots);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, MIN_LOTS, exec->i.min_lots);
    if (rc != SQLITE_OK)
        goto fail1;

    const ScIden match = exec->match;
    rc = match != 0 ? sqlite3_bind_int64(stmt, MATCH, match) : sqlite3_bind_null(stmt, MATCH);
    if (rc != SQLITE_OK)
        goto fail1;

    const int role = exec->role;
    rc = role != 0 ? sqlite3_bind_int(stmt, ROLE, role) : sqlite3_bind_null(stmt, ROLE);
    if (rc != SQLITE_OK)
        goto fail1;

    if (enriched) {
        if (exec->cpty.rec)
            rc = sqlite3_bind_int64(stmt, CPTY, exec->cpty.rec->id);
        else
            rc = sqlite3_bind_null(stmt, CPTY);
    } else {
        const ScIden cpty = exec->cpty.id_only;
        rc = cpty != 0 ? sqlite3_bind_int64(stmt, CPTY, cpty) : sqlite3_bind_null(stmt, CPTY);
    }
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, CREATED, exec->created);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, MODIFIED, exec->created);
    if (rc != SQLITE_OK)
        goto fail1;

    return SC_TRUE;
 fail1:
    sc_err_set(SC_EIO, sqlite3_errmsg(sqlite->db));
    sqlite3_clear_bindings(stmt);
    return SC_FALSE;
}

static ScBool
bind_update_exec(struct FirSqlite* sqlite, ScIden id, ScMillis modified)
{
    enum {
        MODIFIED = 1,
        ID
    };
    sqlite3_stmt* stmt = sqlite->update_exec;
    int rc = sqlite3_bind_int64(stmt, MODIFIED, modified);
    if (rc != SQLITE_OK)
        goto fail1;

    rc = sqlite3_bind_int64(stmt, ID, id);
    if (rc != SQLITE_OK)
        goto fail1;

    return SC_TRUE;
 fail1:
    sc_err_set(SC_EIO, sqlite3_errmsg(sqlite->db));
    sqlite3_clear_bindings(stmt);
    return SC_FALSE;
}

static ssize_t
select_accnt(struct FirSqlite* sqlite, ScPool pool, struct ScSlNode** first)
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

    struct ScQueue rq;
    sc_queue_init(&rq);

    size_t size = 0;
    for (;;) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {

            struct ScRec* rec = sc_pool_alloc_rec(pool);
            if (!rec)
                goto fail2;
            sc_rec_init(rec);

            // Header.

            rec->type = SC_ENTITY_ACCNT;
            rec->id = sqlite3_column_int64(stmt, ID);
            strncpy(rec->mnem,
                    (const char*)sqlite3_column_text(stmt, MNEM), SC_MNEM_MAX);
            strncpy(rec->display,
                    (const char*)sqlite3_column_text(stmt, DISPLAY), SC_DISPLAY_MAX);

            // Body.

            strncpy(rec->accnt.email,
                    (const char*)sqlite3_column_text(stmt, EMAIL), SC_EMAIL_MAX);
            rec->accnt.state = NULL;

            sc_log_debug3("accnt: id=%ld,mnem=%.16s,display=%.64s,email=%.64s",
                           rec->id, rec->mnem, rec->display, rec->accnt.email);

            sc_queue_insert_back(&rq, &rec->shared_node_);
            ++size;

        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            sc_err_set(SC_EIO, sqlite3_errmsg(sqlite->db));
            goto fail2;
        }
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    *first = sc_queue_first(&rq);
    return size;
 fail2:
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    sc_pool_free_entity_list(pool, SC_ENTITY_ACCNT, sc_queue_first(&rq));
    *first = NULL;
 fail1:
    return -1;
}

static ssize_t
select_contr(struct FirSqlite* sqlite, ScPool pool, struct ScSlNode** first)
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

    struct ScQueue rq;
    sc_queue_init(&rq);

    ssize_t size = 0;
    for (;;) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {

            struct ScRec* rec = sc_pool_alloc_rec(pool);
            if (!rec)
                goto fail2;
            sc_rec_init(rec);

            // Header.

            rec->type = SC_ENTITY_CONTR;
            rec->id = sqlite3_column_int64(stmt, ID);
            strncpy(rec->mnem,
                    (const char*)sqlite3_column_text(stmt, MNEM), SC_MNEM_MAX);
            strncpy(rec->display,
                    (const char*)sqlite3_column_text(stmt, DISPLAY), SC_DISPLAY_MAX);

            // Body.

            strncpy(rec->contr.asset_type,
                    (const char*)sqlite3_column_text(stmt, ASSET_TYPE), SC_MNEM_MAX);
            strncpy(rec->contr.asset,
                    (const char*)sqlite3_column_text(stmt, ASSET), SC_MNEM_MAX);
            strncpy(rec->contr.ccy,
                    (const char*)sqlite3_column_text(stmt, CCY), SC_MNEM_MAX);

            const int tick_numer = sqlite3_column_int(stmt, TICK_NUMER);
            const int tick_denom = sqlite3_column_int(stmt, TICK_DENOM);
            const double price_inc = sc_fract_to_real(tick_numer, tick_denom);
            rec->contr.tick_numer = tick_numer;
            rec->contr.tick_denom = tick_denom;
            rec->contr.price_inc = price_inc;

            const int lot_numer = sqlite3_column_int(stmt, LOT_NUMER);
            const int lot_denom = sqlite3_column_int(stmt, LOT_DENOM);
            const double qty_inc = sc_fract_to_real(lot_numer, lot_denom);
            rec->contr.lot_numer = lot_numer;
            rec->contr.lot_denom = lot_denom;
            rec->contr.qty_inc = qty_inc;

            rec->contr.price_dp = sc_real_to_dp(price_inc);
            rec->contr.pip_dp = sqlite3_column_int(stmt, PIP_DP);
            rec->contr.qty_dp = sc_real_to_dp(qty_inc);

            rec->contr.min_lots = sqlite3_column_int64(stmt, MIN_LOTS);
            rec->contr.max_lots = sqlite3_column_int64(stmt, MAX_LOTS);

            sc_log_debug3("contr: id=%ld,mnem=%.16s,display=%.64s,asset_type=%.16s,"
                           "asset=%.16s,ccy=%.16s,price_inc=%f,qty_inc=%.2f,price_dp=%d,"
                           "pip_dp=%d,qty_dp=%d,min_lots=%ld,max_lots=%ld",
                           rec->id, rec->mnem, rec->display, rec->contr.asset_type,
                           rec->contr.asset, rec->contr.ccy, rec->contr.price_inc,
                           rec->contr.qty_inc, rec->contr.price_dp, rec->contr.pip_dp,
                           rec->contr.qty_dp, rec->contr.min_lots, rec->contr.max_lots);

            sc_queue_insert_back(&rq, &rec->shared_node_);
            ++size;

        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            sc_err_set(SC_EIO, sqlite3_errmsg(sqlite->db));
            goto fail2;
        }
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    *first = sc_queue_first(&rq);
    return size;
 fail2:
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    sc_pool_free_entity_list(pool, SC_ENTITY_CONTR, sc_queue_first(&rq));
    *first = NULL;
 fail1:
    return -1;
}

static ssize_t
select_perm(struct FirSqlite* sqlite, ScPool pool, struct ScSlNode** first)
{
    enum {
        TRADER,
        GIVEUP
    };

    sqlite3_stmt* stmt = prepare(sqlite->db, SELECT_PERM_SQL);
    if (!stmt)
        goto fail1;

    struct ScQueue mq;
    sc_queue_init(&mq);

    ssize_t size = 0;
    for (;;) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {

            struct ScPerm* perm = sc_pool_alloc_perm(pool);
            if (!perm)
                goto fail2;
            sc_perm_init(perm);

            perm->trader.id_only = sqlite3_column_int64(stmt, TRADER);
            perm->giveup.id_only = sqlite3_column_int64(stmt, GIVEUP);

            sc_log_debug3("perm: trader=%ld,giveup=%ld",
                           perm->trader.id_only, perm->giveup.id_only);

            sc_queue_insert_back(&mq, &perm->shared_node_);
            ++size;

        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            sc_err_set(SC_EIO, sqlite3_errmsg(sqlite->db));
            goto fail2;
        }
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    *first = sc_queue_first(&mq);
    return size;
 fail2:
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    sc_pool_free_entity_list(pool, SC_ENTITY_PERM, sc_queue_first(&mq));
    *first = NULL;
 fail1:
    return -1;
}

static ssize_t
select_order(struct FirSqlite* sqlite, ScPool pool, struct ScSlNode** first)
{
    enum {
        ID,
        TRADER,
        GIVEUP,
        CONTR,
        SETTL_DAY,
        REF,
        STATE,
        ACTION,
        TICKS,
        LOTS,
        RESD,
        EXEC,
        LAST_TICKS,
        LAST_LOTS,
        MIN_LOTS,
        CREATED,
        MODIFIED
    };

    sqlite3_stmt* stmt = prepare(sqlite->db, SELECT_ORDER_SQL);
    if (!stmt)
        goto fail1;

    struct ScQueue oq;
    sc_queue_init(&oq);

    ssize_t size = 0;
    for (;;) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {

            struct ScOrder* order = sc_pool_alloc_order(pool);
            if (!order)
                goto fail2;
            sc_order_init(order);

            order->level = NULL;
            order->id = sqlite3_column_int64(stmt, ID);
            order->i.trader.id_only = sqlite3_column_int64(stmt, TRADER);
            order->i.giveup.id_only = sqlite3_column_int64(stmt, GIVEUP);
            order->i.contr.id_only = sqlite3_column_int64(stmt, CONTR);
            order->i.settl_day = sqlite3_column_int(stmt, SETTL_DAY);
            if (sqlite3_column_type(stmt, REF) != SQLITE_NULL)
                strncpy(order->i.ref,
                        (const char*)sqlite3_column_text(stmt, REF), SC_REF_MAX);
            else
                order->i.ref[0] = '\0';
            order->i.state = sqlite3_column_int(stmt, STATE);
            order->i.action = sqlite3_column_int(stmt, ACTION);
            order->i.ticks = sqlite3_column_int64(stmt, TICKS);
            order->i.lots = sqlite3_column_int64(stmt, LOTS);
            order->i.resd = sqlite3_column_int64(stmt, RESD);
            order->i.exec = sqlite3_column_int64(stmt, EXEC);
            order->i.last_ticks = sqlite3_column_int64(stmt, LAST_TICKS);
            order->i.last_lots = sqlite3_column_int64(stmt, LAST_LOTS);
            order->i.min_lots = sqlite3_column_int64(stmt, MIN_LOTS);
            order->created = sqlite3_column_int64(stmt, CREATED);
            order->modified = sqlite3_column_int64(stmt, MODIFIED);

            sc_log_debug3("order: id=%ld,trader=%ld,giveup=%ld,contr=%ld,settl_day=%d,"
                           "ref=%.64s,state=%d,action=%d,ticks=%ld,lots=%ld,resd=%ld,"
                           "exec=%ld,last_ticks=%ld,last_lots=%ld,min_lots=%ld,"
                           "created=%ld,modified=%ld",
                           order->id, order->i.trader.id_only, order->i.giveup.id_only,
                           order->i.contr.id_only, order->i.settl_day, order->i.ref,
                           order->i.state, order->i.action, order->i.ticks, order->i.lots,
                           order->i.resd, order->i.exec, order->i.last_ticks, order->i.last_lots,
                           order->i.min_lots, order->created, order->modified);

            sc_queue_insert_back(&oq, &order->shared_node_);
            ++size;

        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            sc_err_set(SC_EIO, sqlite3_errmsg(sqlite->db));
            goto fail2;
        }
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    *first = sc_queue_first(&oq);
    return size;
 fail2:
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    sc_pool_free_entity_list(pool, SC_ENTITY_ORDER, sc_queue_first(&oq));
    *first = NULL;
 fail1:
    return -1;
}

static ssize_t
select_trade(struct FirSqlite* sqlite, ScPool pool, struct ScSlNode** first)
{
    enum {
        ID,
        ORDER,
        TRADER,
        GIVEUP,
        CONTR,
        SETTL_DAY,
        REF,
        ACTION,
        TICKS,
        LOTS,
        RESD,
        EXEC,
        LAST_TICKS,
        LAST_LOTS,
        MIN_LOTS,
        MATCH,
        ROLE,
        CPTY,
        CREATED
    };

    sqlite3_stmt* stmt = prepare(sqlite->db, SELECT_TRADE_SQL);
    if (!stmt)
        goto fail1;

    struct ScQueue tq;
    sc_queue_init(&tq);

    ssize_t size = 0;
    for (;;) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {

            struct ScExec* exec = sc_pool_alloc_exec(pool);
            if (!exec)
                goto fail2;
            sc_exec_init(exec);

            exec->id = sqlite3_column_int64(stmt, ID);
            exec->order = sqlite3_column_int64(stmt, ORDER);
            exec->i.trader.id_only = sqlite3_column_int64(stmt, TRADER);
            exec->i.giveup.id_only = sqlite3_column_int64(stmt, GIVEUP);
            exec->i.contr.id_only = sqlite3_column_int64(stmt, CONTR);
            exec->i.settl_day = sqlite3_column_int(stmt, SETTL_DAY);
            if (sqlite3_column_type(stmt, REF) != SQLITE_NULL)
                strncpy(exec->i.ref,
                        (const char*)sqlite3_column_text(stmt, REF), SC_REF_MAX);
            else
                exec->i.ref[0] = '\0';
            exec->i.state = SC_STATE_TRADE;
            exec->i.action = sqlite3_column_int(stmt, ACTION);
            exec->i.ticks = sqlite3_column_int64(stmt, TICKS);
            exec->i.lots = sqlite3_column_int64(stmt, LOTS);
            exec->i.resd = sqlite3_column_int64(stmt, RESD);
            exec->i.exec = sqlite3_column_int64(stmt, EXEC);
            exec->i.last_ticks = sqlite3_column_int64(stmt, LAST_TICKS);
            exec->i.last_lots = sqlite3_column_int64(stmt, LAST_LOTS);
            exec->i.min_lots = sqlite3_column_int64(stmt, MIN_LOTS);
            exec->match = sqlite3_column_type(stmt, MATCH) != SQLITE_NULL
                ? sqlite3_column_int64(stmt, MATCH) : 0;
            exec->role = sqlite3_column_type(stmt, ROLE) != SQLITE_NULL
                ? sqlite3_column_int(stmt, ROLE) : 0;
            exec->cpty.id_only = sqlite3_column_type(stmt, CPTY) != SQLITE_NULL
                ? sqlite3_column_int64(stmt, CPTY) : 0;
            exec->created = sqlite3_column_int64(stmt, CREATED);

            sc_log_debug3("exec: id=%ld,order=%ld,trader=%ld,giveup=%ld,contr=%ld,settl_day=%d,"
                           "ref=%.64s,action=%d,ticks=%ld,lots=%ld,resd=%ld,exec=%ld,"
                           "last_ticks=%ld,last_lots=%ld,min_lots=%ld,match=%ld,role=%d,cpty=%ld,"
                           "created=%ld",
                           exec->id, exec->order, exec->i.trader.id_only, exec->i.giveup.id_only,
                           exec->i.contr.id_only, exec->i.settl_day, exec->i.ref, exec->i.action,
                           exec->i.ticks, exec->i.lots, exec->i.resd, exec->i.exec,
                           exec->i.last_ticks, exec->i.last_lots, exec->i.min_lots,
                           exec->match, exec->role, exec->cpty.id_only, exec->created);

            sc_queue_insert_back(&tq, &exec->shared_node_);
            ++size;

        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            sc_err_set(SC_EIO, sqlite3_errmsg(sqlite->db));
            goto fail2;
        }
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    *first = sc_queue_first(&tq);
    return size;
 fail2:
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    sc_pool_free_entity_list(pool, SC_ENTITY_EXEC, sc_queue_first(&tq));
    *first = NULL;
 fail1:
    return -1;
}

static ssize_t
select_posn(struct FirSqlite* sqlite, ScPool pool, struct ScSlNode** first)
{
    enum {
        ACCNT,
        CONTR,
        SETTL_DAY,
        ACTION,
        LICKS,
        LOTS
    };

    sqlite3_stmt* stmt = prepare(sqlite->db, SELECT_POSN_SQL);
    if (!stmt)
        goto fail1;

    struct ScQueue pq;
    sc_queue_init(&pq);

    struct ScPosn* posn = NULL;

    ssize_t size = 0;
    for (;;) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {

            const ScIden accnt = sqlite3_column_int64(stmt, ACCNT);
            const ScIden contr = sqlite3_column_int64(stmt, CONTR);
            const ScJd settl_day = sqlite3_column_int(stmt, SETTL_DAY);

            // Posn is null for first row.
            if (posn && posn->accnt.id_only == accnt && posn->contr.id_only == contr
                && posn->settl_day == settl_day) {

                // Set other side.

                const int action = sqlite3_column_int(stmt, ACTION);
                if (action == SC_ACTION_BUY) {
                    posn->buy_licks = sqlite3_column_int64(stmt, LICKS);
                    posn->buy_lots = sqlite3_column_int64(stmt, LOTS);
                } else {
                    assert(action == SC_ACTION_SELL);
                    posn->sell_licks = sqlite3_column_int64(stmt, LICKS);
                    posn->sell_lots = sqlite3_column_int64(stmt, LOTS);
                }
                continue;
            }

            posn = sc_pool_alloc_posn(pool);
            if (sc_unlikely(!posn))
                goto fail2;
            sc_posn_init(posn);

            posn->accnt.id_only = accnt;
            posn->contr.id_only = contr;
            posn->settl_day = settl_day;

            const int action = sqlite3_column_int(stmt, ACTION);
            if (action == SC_ACTION_BUY) {
                posn->buy_licks = sqlite3_column_int64(stmt, LICKS);
                posn->buy_lots = sqlite3_column_int64(stmt, LOTS);
                posn->sell_licks = 0;
                posn->sell_lots = 0;
            } else {
                assert(action == SC_ACTION_SELL);
                posn->buy_licks = 0;
                posn->buy_lots = 0;
                posn->sell_licks = sqlite3_column_int64(stmt, LICKS);
                posn->sell_lots = sqlite3_column_int64(stmt, LOTS);
            }

            sc_log_debug3("posn: accnt=%ld,contr=%ld,settl_day=%d,buy_licks=%ld,buy_lots=%ld,"
                           "sell_licks=%ld,sell_lots=%ld",
                           posn->accnt.id_only, posn->contr.id_only, posn->settl_day,
                           posn->buy_licks, posn->buy_lots, posn->sell_licks, posn->sell_lots);

            sc_queue_insert_back(&pq, &posn->shared_node_);
            ++size;

        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            sc_err_set(SC_EIO, sqlite3_errmsg(sqlite->db));
            goto fail2;
        }
    }

    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    *first = sc_queue_first(&pq);
    return size;
 fail2:
    sqlite3_clear_bindings(stmt);
    sqlite3_finalize(stmt);
    sc_pool_free_entity_list(pool, SC_ENTITY_POSN, sc_queue_first(&pq));
    *first = NULL;
 fail1:
    return -1;
}

SC_EXTERN ScBool
fir_sqlite_init(struct FirSqlite* sqlite, const char* path)
{
    sqlite3* db;
    int rc = sqlite3_open_v2(path, &db, SQLITE_OPEN_READWRITE, NULL);
    if (sc_unlikely(rc != SQLITE_OK)) {
        sc_err_set(SC_EIO, sqlite3_errmsg(db));
        // Must close even if open failed.
        goto fail1;
    }

    // Install trace function for debugging.
#if SC_DEBUG_LEVEL >= 2
    sqlite3_trace(db, trace_sql, NULL);
#endif // SC_DEBUG_LEVEL >= 2

#if SC_DEBUG_LEVEL >= 1
    rc = sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, 1, NULL);
    if (sc_unlikely(rc != SQLITE_OK)) {
        sc_err_set(SC_EIO, sqlite3_errmsg(db));
        goto fail1;
    }
#endif // SC_DEBUG_LEVEL >= 1

    if (sc_unlikely(!exec_sql(db, "PRAGMA synchronous = OFF")
                     || !exec_sql(db, "PRAGMA journal_mode = MEMORY"))) {
        sc_err_set(SC_EIO, sqlite3_errmsg(db));
        goto fail1;
    }

    sqlite3_stmt* insert_exec = prepare(db, INSERT_EXEC_SQL);
    if (!insert_exec)
        goto fail1;

    sqlite3_stmt* update_exec = prepare(db, UPDATE_EXEC_SQL);
    if (!update_exec)
        goto fail2;

    sqlite->db = db;
    sqlite->insert_exec = insert_exec;
    sqlite->update_exec = update_exec;
    return SC_TRUE;
 fail2:
    sqlite3_finalize(insert_exec);
 fail1:
    sqlite3_close(db);
    return SC_FALSE;
}

SC_EXTERN void
fir_sqlite_term(struct FirSqlite* sqlite)
{
    assert(sqlite);
    sqlite3_finalize(sqlite->update_exec);
    sqlite3_finalize(sqlite->insert_exec);
    sqlite3_close(sqlite->db);
}

SC_EXTERN ScBool
fir_sqlite_begin_trans(struct FirSqlite* sqlite)
{
    return exec_sql(sqlite->db, "BEGIN TRANSACTION");
}

SC_EXTERN ScBool
fir_sqlite_commit_trans(struct FirSqlite* sqlite)
{
    return exec_sql(sqlite->db, "COMMIT TRANSACTION");
}

SC_EXTERN ScBool
fir_sqlite_rollback_trans(struct FirSqlite* sqlite)
{
    return exec_sql(sqlite->db, "ROLLBACK TRANSACTION");
}

SC_EXTERN ScBool
fir_sqlite_insert_exec(struct FirSqlite* sqlite, const struct ScExec* exec, ScBool enriched)
{
    return bind_insert_exec(sqlite, exec, enriched)
        && exec_stmt(sqlite->db, sqlite->insert_exec);
}

SC_EXTERN ScBool
fir_sqlite_update_exec(struct FirSqlite* sqlite, ScIden id, ScMillis modified)
{
    return bind_update_exec(sqlite, id, modified)
        && exec_stmt(sqlite->db, sqlite->update_exec);
}

SC_EXTERN ssize_t
fir_sqlite_select_entity(struct FirSqlite* sqlite, int type, ScPool pool,
                         struct ScSlNode** first)
{
    ssize_t ret;
    switch (type) {
    case SC_ENTITY_ACCNT:
        ret = select_accnt(sqlite, pool, first);
        break;
    case SC_ENTITY_CONTR:
        ret = select_contr(sqlite, pool, first);
        break;
    case SC_ENTITY_PERM:
        ret = select_perm(sqlite, pool, first);
        break;
    case SC_ENTITY_ORDER:
        ret = select_order(sqlite, pool, first);
        break;
    case SC_ENTITY_EXEC:
        ret = select_trade(sqlite, pool, first);
        break;
    case SC_ENTITY_POSN:
        ret = select_posn(sqlite, pool, first);
        break;
    default:
        sc_err_setf(SC_EINVAL, "invalid type '%d'", type);
        *first = NULL;
        ret = -1;
    }
    return ret;
}
