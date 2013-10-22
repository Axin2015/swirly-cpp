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
#include <dbr/clnt.h>

#include <dbr/err.h>
#include <dbr/model.h>
#include <dbr/msg.h>

#include <stdbool.h>
#include <stdlib.h> // malloc()
#include <string.h> // strncpy()

struct ElmClnt {
    struct DbrIModel model_;
};

static inline struct ElmClnt*
model_implof(DbrModel model)
{
    return dbr_implof(struct ElmClnt, model_, model);
}

static ssize_t
read_entity(DbrModel model, int type, DbrPool pool, struct DbrSlNode** first)
{
    const char* mnem = "x";
    struct DbrMsg msg;

    struct ElmClnt* clnt = model_implof(model);
    switch (type) {
    case DBR_TRADER:
    case DBR_ACCNT:
    case DBR_CONTR:
        msg.type = DBR_READ_REC_REQ;
        msg.read_rec_req.type = type;
       break;
    case DBR_ORDER:
        msg.type = DBR_READ_TRADER_ORDER_REQ;
        strncpy(msg.read_trader_order_req.trader, mnem, DBR_MNEM_MAX);
        break;
    case DBR_TRADE:
        msg.type = DBR_READ_TRADER_TRADE_REQ;
        strncpy(msg.read_trader_trade_req.trader, mnem, DBR_MNEM_MAX);
        break;
    case DBR_POSN:
        msg.type = DBR_READ_ACCNT_POSN_REQ;
        strncpy(msg.read_accnt_posn_req.accnt, mnem, DBR_MNEM_MAX);
        break;
    }
    return clnt - clnt;
}

static const struct DbrModelVtbl MODEL_VTBL = {
    .read_entity = read_entity
};

static DbrBool
emplace_recs(DbrClnt clnt, int type)
{
    return true;
}

static DbrBool
emplace_orders(DbrClnt clnt)
{
    return true;
}

static DbrBool
emplace_trades(DbrClnt clnt)
{
    return true;
}

static DbrBool
emplace_posns(DbrClnt clnt)
{
    return true;
}

DBR_API DbrClnt
dbr_clnt_create(void)
{
    DbrClnt clnt = malloc(sizeof(struct ElmClnt));
    if (dbr_unlikely(!clnt)) {
        dbr_err_set(DBR_ENOMEM, "out of memory");
        goto fail1;
    }
    clnt->model_.vtbl = &MODEL_VTBL;

    // Data structures are fully initialised at this point.

    if (!emplace_recs(clnt, DBR_TRADER)
        || !emplace_recs(clnt, DBR_ACCNT)
        || !emplace_recs(clnt, DBR_CONTR)
        || !emplace_orders(clnt)
        || !emplace_trades(clnt)
        || !emplace_posns(clnt)) {
        // Use destroy since fully initialised.
        dbr_clnt_destroy(clnt);
        goto fail1;
    }
    return clnt;
 fail1:
    return NULL;
}

DBR_API void
dbr_clnt_destroy(DbrClnt clnt)
{
    if (clnt) {
        free(clnt);
    }
}
