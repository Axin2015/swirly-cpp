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
#include <dbr/pool.h>
#include <dbr/serv.h>
#include <dbr/sqljourn.h>
#include <dbr/sqlmodel.h>
#include <dbr/util.h>
#include <dbr/zmqjourn.h>

#include <zmq.h>

#include <stdlib.h>

static void* ctx = NULL;
static DbrJourn journ = NULL;
static DbrPool pool = NULL;
static DbrServ serv = NULL;

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
    return DBR_TRUE;
}

int
main(int argc, char* argv[])
{
    int status = EXIT_FAILURE;

    ctx = zmq_ctx_new();
    if (!ctx) {
        dbr_err_setf(DBR_EIO, "zmq_ctx_new() failed: %s", zmq_strerror(zmq_errno()));
        dbr_err_print();
        goto exit1;
    }

    journ = dbr_zmqjourn_create(ctx, 1 * 1024 * 1024, factory, "doobry.db");
    if (!journ) {
        dbr_err_prints("dbr_sqljourn_create() failed");
        goto exit2;
    }

    pool = dbr_pool_create(dbr_millis(), 8 * 1024 * 1024);
    if (!pool) {
        dbr_err_prints("dbr_pool_create() failed");
        goto exit3;
    }

    serv = dbr_serv_create(journ, pool);
    if (!serv) {
        dbr_err_prints("dbr_serv_create() failed");
        goto exit4;
    }

    if (!load(serv, "doobry.db")) {
        dbr_err_prints("load() failed");
        goto exit5;
    }

    if (!run())
        goto exit5;

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
