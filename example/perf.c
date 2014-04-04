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

#include <dbr/err.h>
#include <dbr/log.h>
#include <dbr/pool.h>
#include <dbr/serv.h>
#include <dbr/sess.h>
#include <dbr/sqljourn.h>
#include <dbr/sqlmodel.h>
#include <dbr/trader.h>
#include <dbr/util.h>
#include <dbr/zmqjourn.h>

#include <stdlib.h>

#include <uuid/uuid.h>
#include <zmq.h>

static void* ctx = NULL;
static DbrJourn journ = NULL;
static DbrPool pool = NULL;
static DbrServ serv = NULL;

// Time Stamp Counter.

typedef unsigned long long tsc_t;

static inline tsc_t
tsc(void)
{
    unsigned high; // edx (=d)
    unsigned low;  // eax (=a)
    __asm__ __volatile__ ("rdtsc" : "=d"(high), "=a"(low));
    return (((tsc_t)high) << 32) | ((tsc_t)low);
}

static struct DbrRec*
find_rec_mnem(int type, const char* mnem)
{
    struct DbrSlNode* node = dbr_serv_find_rec_mnem(serv, type, mnem);
    if (dbr_unlikely(!node)) {
        dbr_err_setf(DBR_EINVAL, "no such record '%.16s'", mnem);
        return NULL;
    }
    return dbr_serv_rec_entry(node);
}

static DbrJourn
factory(void* arg)
{
    return dbr_sqljourn_create((const char*)arg);
}

static DbrBool
load(DbrServ serv, const char* path)
{
    DbrBool ret;

    DbrModel model = dbr_sqlmodel_create("doobry.db");
    if (dbr_likely(model)) {
        ret = dbr_serv_load(serv, model);
        dbr_model_destroy(model);
    } else
        ret = DBR_FALSE;

    return ret;
}

static DbrBool
run(void)
{
    const DbrDate settl_date = 20140307;

    struct DbrRec* trec = find_rec_mnem(DBR_ENTITY_TRADER, "WRAMIREZ");
    if (dbr_unlikely(!trec))
        goto fail1;

    struct DbrRec* arec = find_rec_mnem(DBR_ENTITY_ACCNT, "DBRA");
    if (dbr_unlikely(!arec))
        goto fail1;

    struct DbrRec* crec = find_rec_mnem(DBR_ENTITY_CONTR, "EURUSD");
    if (dbr_unlikely(!crec))
        goto fail1;

    DbrTrader trader = dbr_serv_trader(serv, trec);
    if (dbr_unlikely(!trader))
        goto fail1;

    DbrAccnt accnt = dbr_serv_accnt(serv, arec);
    if (dbr_unlikely(!accnt))
        goto fail1;

    struct DbrBook* book = dbr_serv_book(serv, crec, settl_date);
    if (dbr_unlikely(!book))
        goto fail1;

    DbrUuid uuid;
    uuid_generate(uuid);
    struct DbrSess* sess = dbr_serv_sess(serv, uuid);
    if (dbr_unlikely(!sess))
        goto fail1;

    if (dbr_unlikely(!dbr_sess_logon(sess, trader)))
        goto fail1;

    enum { ITERS = 500 };
    tsc_t total = 0;
    for (int i = 0; i < ITERS; ++i) {

        const tsc_t start = tsc();
        if (dbr_unlikely(!dbr_serv_place(serv, trader, accnt, book, NULL,
                                         DBR_ACTION_BUY, 12345, 1, 0))) {
            dbr_err_perror("dbr_serv_place() failed");
            goto fail2;
        }

        if (dbr_unlikely(!dbr_serv_place(serv, trader, accnt, book, NULL,
                                         DBR_ACTION_SELL, 12345, 1, 0))) {
            dbr_err_perror("dbr_serv_place() failed");
            goto fail2;
        }
        const tsc_t finish = tsc();
        total += finish - start;

        dbr_serv_clear(serv);

        struct DbrRbNode* node;
        while ((node = dbr_trader_first_trade(trader)) != DBR_TRADER_END_TRADE) {
            struct DbrExec* trade = dbr_trader_trade_entry(node);
            if (!dbr_serv_ack_trade(serv, trader, trade->id)) {
                dbr_err_perror("dbr_serv_ack_trade() failed");
                goto fail2;
            }
        }
    }
    const double ms = total / 1e6;
    const int orders = ITERS * 2;
    dbr_log_info("orders=%d, total=%.3f ms, per_order=%.3f ms", orders, ms, ms / orders);

    dbr_sess_logoff(sess, trader, DBR_FALSE);
    return DBR_TRUE;
 fail2:
    dbr_sess_logoff(sess, trader, DBR_FALSE);
 fail1:
    return DBR_FALSE;
}

int
main(int argc, char* argv[])
{
    int status = EXIT_FAILURE;

    ctx = zmq_ctx_new();
    if (!ctx) {
        dbr_err_printf(DBR_EIO, "zmq_ctx_new() failed: %s", zmq_strerror(zmq_errno()));
        goto exit1;
    }

    journ = dbr_zmqjourn_create(ctx, 1 * 1024 * 1024, factory, "doobry.db");
    if (!journ) {
        dbr_err_perror("dbr_sqljourn_create() failed");
        goto exit2;
    }

    pool = dbr_pool_create(8 * 1024 * 1024);
    if (!pool) {
        dbr_err_perror("dbr_pool_create() failed");
        goto exit3;
    }

    serv = dbr_serv_create("doobry.bin", journ, pool);
    if (!serv) {
        dbr_err_perror("dbr_serv_create() failed");
        goto exit4;
    }

    if (!load(serv, "doobry.db")) {
        dbr_err_perror("load() failed");
        goto exit5;
    }

    if (!run()) {
        dbr_err_perror("run() failed");
        goto exit5;
    }

    status = EXIT_SUCCESS;
 exit5:
    dbr_serv_destroy(serv);
 exit4:
    dbr_pool_destroy(pool);
 exit3:
    dbr_journ_destroy(journ);
 exit2:
    zmq_ctx_destroy(ctx);
 exit1:
    return status;
}
