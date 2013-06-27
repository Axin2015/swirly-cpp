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
#include "test.hpp"
#include "model.hpp"

#include <dbr/ctx.hpp>
#include <dbr/pool.hpp>

#include <dbr/conv.h>

using namespace dbr;

static DbrRec*
get_rec_id(DbrCtx ctx, int type, DbrIden id)
{
    DbrSlNode* node = dbr_ctx_find_rec_id(ctx, type, id);
    check(node != NULL);
    return dbr_rec_entry(node);
}

static DbrRec*
get_rec_mnem(DbrCtx ctx, int type, const char* mnem)
{
    DbrSlNode* node = dbr_ctx_find_rec_mnem(ctx, type, mnem);
    check(node != NULL);
    return dbr_rec_entry(node);
}

TEST_CASE(find_instr)
{
    Pool pool;
    Model model(pool, 1);
    Ctx ctx(pool, &model);

    DbrSlNode* node = dbr_ctx_find_rec_mnem(ctx, DBR_INSTR, "BAD");
    check(node == NULL);

    DbrRec* irec = get_rec_mnem(ctx, DBR_INSTR, "EURUSD.SPOTFWD");
    check(irec != NULL);

    check(irec == get_rec_id(ctx, DBR_INSTR, irec->id));
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
}

TEST_CASE(find_market)
{
    Pool pool;
    Model model(pool, 1);
    Ctx ctx(pool, &model);

    DbrSlNode* node = dbr_ctx_find_rec_mnem(ctx, DBR_MARKET, "BAD");
    check(node == NULL);

    DbrRec* mrec = get_rec_mnem(ctx, DBR_MARKET, "EURUSD");
    check(mrec != NULL);
    check(mrec == get_rec_id(ctx, DBR_MARKET, mrec->id));
    check(sequal(mrec->mnem, "EURUSD", DBR_MNEM_MAX));
    // Body.
    check(sequal(mrec->market.tenor, "SP", DBR_TENOR_MAX));
}

TEST_CASE(find_accnt)
{
    Pool pool;
    Model model(pool, 1);
    Ctx ctx(pool, &model);

    DbrSlNode* node = dbr_ctx_find_rec_mnem(ctx, DBR_ACCNT, "BAD");
    check(node == NULL);

    DbrRec* arec = get_rec_mnem(ctx, DBR_ACCNT, "DBRA");
    check(arec != NULL);
    check(arec == get_rec_id(ctx, DBR_ACCNT, arec->id));
    check(sequal(arec->mnem, "DBRA", DBR_MNEM_MAX));
    // Body.
    check(sequal(arec->accnt.display, "Account A", DBR_DISPLAY_MAX));
    check(sequal(arec->accnt.email, "dbra@doobry.org", DBR_EMAIL_MAX));
}

TEST_CASE(find_trader)
{
    Pool pool;
    Model model(pool, 1);
    Ctx ctx(pool, &model);

    DbrSlNode* node = dbr_ctx_find_rec_mnem(ctx, DBR_TRADER, "BAD");
    check(node == NULL);

    DbrRec* trec = get_rec_mnem(ctx, DBR_TRADER, "WRAMIREZ");
    check(trec != NULL);
    check(trec == get_rec_id(ctx, DBR_TRADER, trec->id));
    check(sequal(trec->mnem, "WRAMIREZ", DBR_MNEM_MAX));
    // Body.
    check(sequal(trec->trader.display, "Wayne Ramirez", DBR_DISPLAY_MAX));
    check(sequal(trec->trader.email, "wayne.ramirez@doobry.org", DBR_EMAIL_MAX));
}
