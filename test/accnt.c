/**
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

void
accnt_id(void)
{
    DbrCtx ctx = dbr_ctx_create(1);
    DbrModel model = model_create(ctx);
    DbrEnv env = dbr_env_create(ctx, model);

    struct DbrSlNode* node = dbr_rec_find_mnem(env, DBR_ACCNT, "DBRA");
    check(node != NULL);
    struct DbrRec* arec = dbr_rec_entry(node);
    check(arec != NULL);
    DbrAccnt accnt = dbr_accnt_lazy(env, arec);
    check(accnt != NULL);
    check(dbr_accnt_id(accnt) ==  arec->id);

    dbr_env_destroy(env);
    model_destroy(model);
    dbr_ctx_destroy(ctx);
}
