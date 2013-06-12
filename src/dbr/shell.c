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
#define _XOPEN_SOURCE 700 // getline()

#include <dbr/conv.h>
#include <dbr/core.h>
#include <dbr/err.h>
#include <dbr/rest.h>
#include <dbr/string.h>
#include <dbr/util.h>

#include <stdlib.h>       // free()

static void
print_instr(const struct DbrRec* rec)
{
    assert(rec);
    size_t n = dbr_instrlen(rec);
    char buf[n + 1];
    dbr_instrcpy(buf, rec);
    printf("%s\n", buf);
}

static void
print_market(struct DbrRec* rec)
{
    assert(rec);
    size_t n = dbr_marketlen(rec);
    char buf[n + 1];
    dbr_marketcpy(buf, rec);
    printf("%s\n", buf);
}

static void
print_trader(const struct DbrRec* rec)
{
    assert(rec);
    size_t n = dbr_traderlen(rec);
    char buf[n + 1];
    dbr_tradercpy(buf, rec);
    printf("%s\n", buf);
}

static void
print_accnt(const struct DbrRec* rec)
{
    assert(rec);
    size_t n = dbr_accntlen(rec);
    char buf[n + 1];
    dbr_accntcpy(buf, rec);
    printf("%s\n", buf);
}

static void
print_order(const struct DbrOrder* order)
{
    assert(order);
    size_t n = dbr_orderlen(order);
    char buf[n + 1];
    dbr_ordercpy(buf, order);
    printf("%s\n", buf);
}

static void
print_trade(const struct DbrTrade* trade)
{
    assert(trade);
    size_t n = dbr_tradelen(trade);
    char buf[n + 1];
    dbr_tradecpy(buf, trade);
    printf("%s\n", buf);
}

static void
print_posn(const struct DbrPosn* posn)
{
    assert(posn);
    size_t n = dbr_posnlen(posn);
    char buf[n + 1];
    dbr_posncpy(buf, posn);
    printf("%s\n", buf);
}

static DbrTrader
find_trader_mnem(DbrEnv env, const char* mnem)
{
    struct DbrSlNode* it = dbr_rec_find_mnem(env, DBR_TRADER, mnem);
    struct DbrSlNode* end = dbr_rec_end(env);
    if (it == end)
        return NULL;

    return dbr_trader_lazy(env, dbr_rec_entry(it));
}

static DbrAccnt
find_accnt_mnem(DbrEnv env, const char* mnem)
{
    struct DbrSlNode* it = dbr_rec_find_mnem(env, DBR_ACCNT, mnem);
    struct DbrSlNode* end = dbr_rec_end(env);
    if (it == end)
        return NULL;

    return dbr_accnt_lazy(env, dbr_rec_entry(it));
}

static void
get_instr_list(DbrEnv env)
{
    size_t size;
    struct DbrSlNode* it = dbr_rec_entries(env, DBR_INSTR, &size);
    struct DbrSlNode* end = dbr_rec_end(env);
    for (; it != end; it = it->next) {
        struct DbrRec* rec = dbr_rec_entry(it);
        print_instr(rec);
    }
}

static void
get_instr_mnem(DbrEnv env, const char* mnem)
{
    struct DbrSlNode* it = dbr_rec_find_mnem(env, DBR_INSTR, mnem);
    struct DbrSlNode* end = dbr_rec_end(env);
    if (it != end) {
        struct DbrRec* rec = dbr_rec_entry(it);
        print_instr(rec);
    }
}

static void
get_market_list(DbrEnv env)
{
    size_t size;
    struct DbrSlNode* it = dbr_rec_entries(env, DBR_MARKET, &size);
    struct DbrSlNode* end = dbr_rec_end(env);
    for (; it != end; it = it->next) {
        struct DbrRec* rec = dbr_rec_entry(it);
        print_market(rec);
    }
}

static void
get_market_mnem(DbrEnv env, const char* mnem)
{
    struct DbrSlNode* it = dbr_rec_find_mnem(env, DBR_MARKET, mnem);
    struct DbrSlNode* end = dbr_rec_end(env);
    if (it != end) {
        struct DbrRec* rec = dbr_rec_entry(it);
        print_market(rec);
    }
}

static void
get_trader_list(DbrEnv env)
{
    size_t size;
    struct DbrSlNode* it = dbr_rec_entries(env, DBR_TRADER, &size);
    struct DbrSlNode* end = dbr_rec_end(env);
    for (; it != end; it = it->next) {
        struct DbrRec* rec = dbr_rec_entry(it);
        print_trader(rec);
    }
}

static void
get_trader_mnem(DbrEnv env, const char* mnem)
{
    struct DbrSlNode* it = dbr_rec_find_mnem(env, DBR_TRADER, mnem);
    struct DbrSlNode* end = dbr_rec_end(env);
    if (it != end) {
        struct DbrRec* rec = dbr_rec_entry(it);
        print_trader(rec);
    }
}

static void
get_accnt_list(DbrEnv env)
{
    size_t size;
    struct DbrSlNode* it = dbr_rec_entries(env, DBR_ACCNT, &size);
    struct DbrSlNode* end = dbr_rec_end(env);
    for (; it != end; it = it->next) {
        struct DbrRec* rec = dbr_rec_entry(it);
        print_accnt(rec);
    }
}

static void
get_accnt_mnem(DbrEnv env, const char* mnem)
{
    struct DbrSlNode* it = dbr_rec_find_mnem(env, DBR_ACCNT, mnem);
    struct DbrSlNode* end = dbr_rec_end(env);
    if (it != end) {
        struct DbrRec* rec = dbr_rec_entry(it);
        print_accnt(rec);
    }
}

static void
get_order_list(DbrEnv env, const char* mnem)
{
    DbrTrader trader = find_trader_mnem(env, mnem);
    if (!trader) {
        fprintf(stderr, "invalid trader: %.16s\n", mnem);
        return;
    }

    struct DbrRbNode* it = dbr_trader_first_order(trader);
    struct DbrRbNode* end = dbr_trader_end_order(trader);
    for (; it != end; it = dbr_rbnode_next(it)) {
        struct DbrOrder* order = dbr_trader_order_entry(it);
        print_order(order);
    }
}

static void
get_order_id(DbrEnv env, const char* mnem, DbrIden id)
{
    DbrTrader trader = find_trader_mnem(env, mnem);
    if (!trader) {
        fprintf(stderr, "invalid trader: %.16s\n", mnem);
        return;
    }

    struct DbrRbNode* it = dbr_trader_find_order_id(trader, id);
    struct DbrRbNode* end = dbr_trader_end_order(trader);
    if (it != end) {
        struct DbrOrder* order = dbr_trader_order_entry(it);
        print_order(order);
    }
}

static void
get_trade_list(DbrEnv env, const char* mnem)
{
    DbrAccnt accnt = find_accnt_mnem(env, mnem);
    if (!accnt) {
        fprintf(stderr, "invalid accnt: %.16s\n", mnem);
        return;
    }

    struct DbrRbNode* it = dbr_accnt_first_trade(accnt);
    struct DbrRbNode* end = dbr_accnt_end_trade(accnt);
    for (; it != end; it = dbr_rbnode_next(it)) {
        struct DbrTrade* trade = dbr_accnt_trade_entry(it);
        print_trade(trade);
    }
}

static void
get_trade_id(DbrEnv env, const char* mnem, DbrIden id)
{
    DbrAccnt accnt = find_accnt_mnem(env, mnem);
    if (!accnt) {
        fprintf(stderr, "invalid accnt: %.16s\n", mnem);
        return;
    }

    struct DbrRbNode* it = dbr_accnt_find_trade_id(accnt, id);
    struct DbrRbNode* end = dbr_accnt_end_trade(accnt);
    if (it != end) {
        struct DbrTrade* trade = dbr_accnt_trade_entry(it);
        print_trade(trade);
    }
}

static void
get_posn_list(DbrEnv env, const char* mnem)
{
    DbrAccnt accnt = find_accnt_mnem(env, mnem);
    if (!accnt) {
        fprintf(stderr, "invalid accnt: %.16s\n", mnem);
        return;
    }

    struct DbrRbNode* it = dbr_accnt_first_posn(accnt);
    struct DbrRbNode* end = dbr_accnt_end_posn(accnt);
    for (; it != end; it = dbr_rbnode_next(it)) {
        struct DbrPosn* posn = dbr_accnt_posn_entry(it);
        print_posn(posn);
    }
}

static void
post_order(DbrEnv env, const char* trader, const char* accnt, const char* market,
           int action, DbrTicks ticks, DbrLots lots)
{
    struct DbrSlNode* it;
    struct DbrSlNode* end = dbr_rec_end(env);

    if ((it = dbr_rec_find_mnem(env, DBR_TRADER, trader)) == end) {
        fprintf(stderr, "invalid trader: %.16s\n", trader);
        return;
    }
    struct DbrRec* trec = dbr_rec_entry(it);

    if ((it = dbr_rec_find_mnem(env, DBR_ACCNT, accnt)) == end) {
        fprintf(stderr, "invalid accnt: %.16s\n", accnt);
        return;
    }
    struct DbrRec* arec = dbr_rec_entry(it);

    if ((it = dbr_rec_find_mnem(env, DBR_MARKET, market)) == end) {
        fprintf(stderr, "invalid market: %.16s\n", market);
        return;
    }
    struct DbrRec* mrec = dbr_rec_entry(it);

    struct DbrTrans trans;
    if (!dbr_exec_submit(env, trec, arec, NULL, mrec, action, ticks, lots, 0, 0, &trans)) {
        dbr_err_print(stderr, "dbr_exec_submit() failed");
        return;
    }

    print_order(trans.new_order);
    dbr_exec_free_matches(env, trans.first_match);
}

static DbrPool pool = NULL;
static DbrModel model = NULL;
static DbrEnv env = NULL;
static char* line = NULL;

static void
destroy(void)
{
    free(line);
    dbr_env_destroy(env);
    dbr_sqlite_destroy(model);
    dbr_pool_destroy(pool);
}

int
main(int argc, char* argv[])
{
    atexit(destroy);
    if (!(pool = dbr_pool_create())) {
        fprintf(stderr, "dbr_pool_create() failed\n");
        return 1;
    }

    if (!(model = dbr_sqlite_create(pool, dbr_millis(), "test.db"))) {
        dbr_err_print(stderr, "dbr_sqlite_create() failed");
        return 1;
    }

    if (!(env = dbr_env_create(pool, model))) {
        dbr_err_print(stderr, "dbr_env_create() failed");
        return 1;
    }

    size_t len = 0;
    struct DbrRest rest;
    dbr_rest_init(&rest);
    for (;;) {
        const ssize_t size = getline(&line, &len, stdin);
        if (size == -1) {
            // Returns -1 on failure to read a line (including end-of-file condition).
            if (feof(stdin))
                break;
            fprintf(stderr, "getline error\n");
            return 1;
        }
        switch (dbr_rest_json(&rest, line, size - 1)) {
        case -1:
            fprintf(stderr, "parse error\n");
            dbr_rest_init(&rest);
            // Fallthrough.
        case 0:
            continue;
        case 1:
            break;
        }
        switch (rest.func) {
        case DBR_GET | DBR_INSTR:
            get_instr_list(env);
            break;
        case DBR_GET | DBR_INSTR | DBR_MNEM_PARAM:
            get_instr_mnem(env, rest.mnem);
            break;
        case DBR_GET | DBR_MARKET:
            get_market_list(env);
            break;
        case DBR_GET | DBR_MARKET | DBR_MNEM_PARAM:
            get_market_mnem(env, rest.mnem);
            break;
        case DBR_GET | DBR_TRADER:
            get_trader_list(env);
            break;
        case DBR_GET | DBR_TRADER | DBR_MNEM_PARAM:
            get_trader_mnem(env, rest.mnem);
            break;
        case DBR_GET | DBR_ACCNT:
            get_accnt_list(env);
            break;
        case DBR_GET | DBR_ACCNT | DBR_MNEM_PARAM:
            get_accnt_mnem(env, rest.mnem);
            break;
        case DBR_GET | DBR_ORDER | DBR_TRADER_PARAM:
            get_order_list(env, rest.trader);
            break;
        case DBR_GET | DBR_ORDER | DBR_TRADER_PARAM | DBR_ID_PARAM:
            get_order_id(env, rest.trader, rest.id);
            break;
        case DBR_GET | DBR_TRADE | DBR_ACCNT_PARAM:
            get_trade_list(env, rest.accnt);
            break;
        case DBR_GET | DBR_TRADE | DBR_ACCNT_PARAM | DBR_ID_PARAM:
            get_trade_id(env, rest.accnt, rest.id);
            break;
        case DBR_GET | DBR_POSN | DBR_ACCNT_PARAM:
            get_posn_list(env, rest.accnt);
            break;
        case DBR_POST | DBR_ORDER | DBR_TRADER_PARAM | DBR_ACCNT_PARAM | DBR_MARKET_PARAM
            | DBR_ACTION_PARAM | DBR_TICKS_PARAM | DBR_LOTS_PARAM:
            post_order(env, rest.trader, rest.accnt, rest.market, rest.action,
                       rest.ticks, rest.lots);
            break;
        default:
            fprintf(stderr, "unsupported func\n");
            break;
        }
        dbr_rest_init(&rest);
    }
    return 0;
}
