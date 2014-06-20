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
#include <dbr/fir/sqljourn.h>
#include <dbr/fir/sqlmodel.h>

#include <dbr/fig/accnt.h>
#include <dbr/fig/serv.h>
#include <dbr/fig/sess.h>

#include <dbr/elm/pool.h>
#include <dbr/elm/zmqjourn.h>

#include <dbr/ash/err.h>
#include <dbr/ash/log.h>
#include <dbr/ash/util.h>

#include <stdlib.h>

#include <uuid/uuid.h>
#include <zmq.h>

static void* zctx = NULL;
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
    const DbrJd settl_day = dbr_ymd_to_jd(2014, 3, 14);

    struct DbrRec* urec = find_rec_mnem(DBR_ENTITY_ACCNT, "WRAMIREZ");
    if (dbr_unlikely(!urec))
        goto fail1;

    struct DbrRec* grec = find_rec_mnem(DBR_ENTITY_ACCNT, "DBRA");
    if (dbr_unlikely(!grec))
        goto fail1;

    struct DbrRec* crec = find_rec_mnem(DBR_ENTITY_CONTR, "EURUSD");
    if (dbr_unlikely(!crec))
        goto fail1;

    DbrAccnt user = dbr_serv_accnt(serv, urec);
    if (dbr_unlikely(!user))
        goto fail1;

    DbrAccnt group = dbr_serv_accnt(serv, grec);
    if (dbr_unlikely(!group))
        goto fail1;

    struct DbrBook* book = dbr_serv_book(serv, crec, settl_day);
    if (dbr_unlikely(!book))
        goto fail1;

    DbrUuid uuid;
    uuid_generate(uuid);
    struct DbrSess* sess = dbr_serv_sess(serv, uuid);
    if (dbr_unlikely(!sess))
        goto fail1;

    if (dbr_unlikely(!dbr_sess_logon(sess, user)))
        goto fail1;

    enum { ITERS = 500 };
    tsc_t total = 0;
    for (int i = 0; i < ITERS; ++i) {

        const tsc_t start = tsc();
        if (dbr_unlikely(!dbr_serv_place(serv, user, group, book, NULL,
                                         DBR_ACTION_BUY, 12345, 1, 0))) {
            dbr_err_perror("dbr_serv_place() failed");
            goto fail2;
        }

        if (dbr_unlikely(!dbr_serv_place(serv, user, group, book, NULL,
                                         DBR_ACTION_SELL, 12345, 1, 0))) {
            dbr_err_perror("dbr_serv_place() failed");
            goto fail2;
        }
        const tsc_t finish = tsc();
        total += finish - start;

        dbr_serv_clear(serv);

        struct DbrRbNode* node;
        while ((node = dbr_accnt_first_trade(user)) != DBR_ACCNT_END_TRADE) {
            struct DbrExec* trade = dbr_accnt_trade_entry(node);
            if (!dbr_serv_ack_trade(serv, user, trade->id)) {
                dbr_err_perror("dbr_serv_ack_trade() failed");
                goto fail2;
            }
        }
    }
    const double ms = total / 1e6;
    const int orders = ITERS * 2;
    dbr_log_info("orders=%d, total=%.3f ms, per_order=%.3f ms", orders, ms, ms / orders);

    dbr_sess_logoff(sess, user);
    return DBR_TRUE;
 fail2:
    dbr_sess_logoff(sess, user);
 fail1:
    return DBR_FALSE;
}

int
main(int argc, char* argv[])
{
    int status = EXIT_FAILURE;

    zctx = zmq_ctx_new();
    if (!zctx) {
        dbr_err_printf(DBR_EIO, "zmq_ctx_new() failed: %s", zmq_strerror(zmq_errno()));
        goto exit1;
    }

    journ = dbr_zmqjourn_create(zctx, 1 * 1024 * 1024, factory, "doobry.db");
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
    zmq_ctx_destroy(zctx);
 exit1:
    return status;
}
