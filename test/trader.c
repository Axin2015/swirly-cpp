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
#include <dbr/ctx.h>
#include <dbr/trader.h>

TEST_CASE(trader_id)
{
    DbrPool pool = dbr_pool_create();
    DbrModel model = model_create(pool, 1);
    DbrCtx ctx = dbr_ctx_create(pool, model);

    struct DbrSlNode* node = dbr_ctx_find_rec_mnem(ctx, DBR_TRADER, "WRAMIREZ");
    check(node != NULL);
    struct DbrRec* trec = dbr_rec_entry(node);
    check(trec != NULL);
    DbrTrader trader = dbr_ctx_trader(ctx, trec);
    check(trader != NULL);
    check(dbr_trader_id(trader) ==  trec->id);

    dbr_ctx_destroy(ctx);
    model_destroy(model);
    dbr_pool_destroy(pool);
}
