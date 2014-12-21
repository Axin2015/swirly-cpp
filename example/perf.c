/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include <sc/fir/sqljourn.h>
#include <sc/fir/sqlmodel.h>

#include <sc/fig/accnt.h>
#include <sc/fig/serv.h>
#include <sc/fig/sess.h>

#include <sc/elm/pool.h>
#include <sc/elm/zmqjourn.h>

#include <sc/ash/err.h>
#include <sc/ash/log.h>
#include <sc/ash/util.h>

#include <stdlib.h>

#include <uuid/uuid.h>
#include <zmq.h>

static void* zctx = NULL;
static ScJourn journ = NULL;
static ScPool pool = NULL;
static ScServ serv = NULL;

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

static struct ScRec*
find_rec_mnem(int type, const char* mnem)
{
    struct ScSlNode* node = sc_serv_find_rec_mnem(serv, type, mnem);
    if (sc_unlikely(!node)) {
        sc_err_setf(SC_EINVAL, "no such record '%.16s'", mnem);
        return NULL;
    }
    return sc_serv_rec_entry(node);
}

static ScJourn
factory(void* arg)
{
    return sc_sqljourn_create((const char*)arg);
}

static ScBool
load(ScServ serv, const char* path)
{
    ScBool ret;

    ScModel model = sc_sqlmodel_create("twirly.db");
    if (sc_likely(model)) {
        ret = sc_serv_load(serv, model);
        sc_model_destroy(model);
    } else
        ret = SC_FALSE;

    return ret;
}

static ScBool
run(void)
{
    const ScJd settl_day = sc_ymd_to_jd(2014, 3, 14);

    struct ScRec* trec = find_rec_mnem(SC_ENTITY_ACCNT, "WRAMIREZ");
    if (sc_unlikely(!trec))
        goto fail1;

    struct ScRec* grec = find_rec_mnem(SC_ENTITY_ACCNT, "SCA");
    if (sc_unlikely(!grec))
        goto fail1;

    struct ScRec* crec = find_rec_mnem(SC_ENTITY_CONTR, "EURUSD");
    if (sc_unlikely(!crec))
        goto fail1;

    ScAccnt trader = sc_serv_accnt(serv, trec);
    if (sc_unlikely(!trader))
        goto fail1;

    ScAccnt giveup = sc_serv_accnt(serv, grec);
    if (sc_unlikely(!giveup))
        goto fail1;

    struct ScBook* book = sc_serv_book(serv, crec, settl_day);
    if (sc_unlikely(!book))
        goto fail1;

    ScUuid uuid;
    uuid_generate(uuid);
    struct ScSess* sess = sc_serv_sess(serv, uuid);
    if (sc_unlikely(!sess))
        goto fail1;

    if (sc_unlikely(!sc_sess_logon(sess, trader)))
        goto fail1;

    enum { ITERS = 500 };
    tsc_t total = 0;
    for (int i = 0; i < ITERS; ++i) {

        const tsc_t start = tsc();
        if (sc_unlikely(!sc_serv_place(serv, trader, giveup, book, NULL,
                                         SC_ACTION_BUY, 12345, 1, 0))) {
            sc_err_perror("sc_serv_place() failed");
            goto fail2;
        }

        if (sc_unlikely(!sc_serv_place(serv, trader, giveup, book, NULL,
                                         SC_ACTION_SELL, 12345, 1, 0))) {
            sc_err_perror("sc_serv_place() failed");
            goto fail2;
        }
        const tsc_t finish = tsc();
        total += finish - start;

        sc_serv_clear(serv);

        struct ScRbNode* node;
        while ((node = sc_accnt_first_trade(trader)) != SC_ACCNT_END_TRADE) {
            struct ScExec* trade = sc_accnt_trade_entry(node);
            if (!sc_serv_ack_trade(serv, trader, trade->id)) {
                sc_err_perror("sc_serv_ack_trade() failed");
                goto fail2;
            }
        }
    }
    const double ms = total / 1e6;
    const int orders = ITERS * 2;
    sc_log_info("orders=%d, total=%.3f ms, per_order=%.3f ms", orders, ms, ms / orders);

    sc_sess_logoff(sess, trader);
    return SC_TRUE;
 fail2:
    sc_sess_logoff(sess, trader);
 fail1:
    return SC_FALSE;
}

int
main(int argc, char* argv[])
{
    int status = EXIT_FAILURE;

    zctx = zmq_ctx_new();
    if (!zctx) {
        sc_err_printf(SC_EIO, "zmq_ctx_new() failed: %s", zmq_strerror(zmq_errno()));
        goto exit1;
    }

    journ = sc_zmqjourn_create(zctx, 1 * 1024 * 1024, factory, "twirly.db");
    if (!journ) {
        sc_err_perror("sc_sqljourn_create() failed");
        goto exit2;
    }

    pool = sc_pool_create(8 * 1024 * 1024);
    if (!pool) {
        sc_err_perror("sc_pool_create() failed");
        goto exit3;
    }

    serv = sc_serv_create("twirly.bin", journ, pool);
    if (!serv) {
        sc_err_perror("sc_serv_create() failed");
        goto exit4;
    }

    if (!load(serv, "twirly.db")) {
        sc_err_perror("load() failed");
        goto exit5;
    }

    if (!run()) {
        sc_err_perror("run() failed");
        goto exit5;
    }

    status = EXIT_SUCCESS;
 exit5:
    sc_serv_destroy(serv);
 exit4:
    sc_pool_destroy(pool);
 exit3:
    sc_journ_destroy(journ);
 exit2:
    zmq_ctx_destroy(zctx);
 exit1:
    return status;
}
