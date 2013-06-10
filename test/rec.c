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
#include "test.h"
#include "model.h"

#include <dbr/conv.h>
#include <dbr/core.h>

static struct DbrRec*
get_rec_id(DbrEnv env, int type, DbrIden id)
{
    struct DbrSlNode* node = dbr_rec_find_id(env, type, id);
    check(node != NULL);
    return dbr_rec_entry(node);
}

static struct DbrRec*
get_rec_mnem(DbrEnv env, int type, const char* mnem)
{
    struct DbrSlNode* node = dbr_rec_find_mnem(env, type, mnem);
    check(node != NULL);
    return dbr_rec_entry(node);
}

void
find_instr(void)
{
    DbrCtx ctx = dbr_ctx_create(1);
    DbrModel model = model_create(ctx);
    DbrEnv env = dbr_env_create(ctx, model);

    struct DbrSlNode* node = dbr_rec_find_mnem(env, DBR_INSTR, "BAD");
    check(node == NULL);

    struct DbrRec* irec = get_rec_mnem(env, DBR_INSTR, "EURUSD.SPOTFWD");
    check(irec != NULL);

    check(irec == get_rec_id(env, DBR_INSTR, irec->id));
    check(sequal(irec->mnem, "EURUSD.SPOTFWD", DBR_MNEM_MAX));
    // Body.
    check(sequal(irec->instr.display, "EURUSD.SPOTFWD", DBR_DISPLAY_MAX));
    check(sequal(irec->instr.asset_type, "CURRENCY", DBR_MNEM_MAX));
    check(sequal(irec->instr.instr_type, "SPOTFWD", DBR_MNEM_MAX));
    check(sequal(irec->instr.asset, "EUR", DBR_MNEM_MAX));
    check(sequal(irec->instr.ccy, "USD", DBR_MNEM_MAX));
    check(fequal(irec->instr.price_inc, 0.0001));
    check(fequal(irec->instr.qty_inc, 1e6));
    check(irec->instr.price_dp == 4);
    check(irec->instr.pip_dp == 4);
    check(irec->instr.qty_dp == 0);
    check(irec->instr.min_lots == 1);
    check(irec->instr.max_lots == 10);

    dbr_env_destroy(env);
    model_destroy(model);
    dbr_ctx_destroy(ctx);
}

void
find_market(void)
{
    DbrCtx ctx = dbr_ctx_create(1);
    DbrModel model = model_create(ctx);
    DbrEnv env = dbr_env_create(ctx, model);

    struct DbrSlNode* node = dbr_rec_find_mnem(env, DBR_MARKET, "BAD");
    check(node == NULL);

    struct DbrRec* mrec = get_rec_mnem(env, DBR_MARKET, "EURUSD");
    check(mrec != NULL);
    check(mrec == get_rec_id(env, DBR_MARKET, mrec->id));
    check(sequal(mrec->mnem, "EURUSD", DBR_MNEM_MAX));
    // Body.
    check(sequal(mrec->market.tenor, "SP", DBR_TENOR_MAX));

    dbr_env_destroy(env);
    model_destroy(model);
    dbr_ctx_destroy(ctx);
}

void
find_accnt(void)
{
    DbrCtx ctx = dbr_ctx_create(1);
    DbrModel model = model_create(ctx);
    DbrEnv env = dbr_env_create(ctx, model);

    struct DbrSlNode* node = dbr_rec_find_mnem(env, DBR_ACCNT, "BAD");
    check(node == NULL);

    struct DbrRec* arec = get_rec_mnem(env, DBR_ACCNT, "DBRA");
    check(arec != NULL);
    check(arec == get_rec_id(env, DBR_ACCNT, arec->id));
    check(sequal(arec->mnem, "DBRA", DBR_MNEM_MAX));
    // Body.
    check(sequal(arec->accnt.display, "Account A", DBR_DISPLAY_MAX));
    check(sequal(arec->accnt.email, "dbra@doobry.org", DBR_EMAIL_MAX));

    dbr_env_destroy(env);
    model_destroy(model);
    dbr_ctx_destroy(ctx);
}

void
find_trader(void)
{
    DbrCtx ctx = dbr_ctx_create(1);
    DbrModel model = model_create(ctx);
    DbrEnv env = dbr_env_create(ctx, model);

    struct DbrSlNode* node = dbr_rec_find_mnem(env, DBR_TRADER, "BAD");
    check(node == NULL);

    struct DbrRec* trec = get_rec_mnem(env, DBR_TRADER, "WRAMIREZ");
    check(trec != NULL);
    check(trec == get_rec_id(env, DBR_TRADER, trec->id));
    check(sequal(trec->mnem, "WRAMIREZ", DBR_MNEM_MAX));
    // Body.
    check(sequal(trec->trader.display, "Wayne Ramirez", DBR_DISPLAY_MAX));
    check(sequal(trec->trader.email, "wayne.ramirez@doobry.org", DBR_EMAIL_MAX));

    dbr_env_destroy(env);
    model_destroy(model);
    dbr_ctx_destroy(ctx);
}
