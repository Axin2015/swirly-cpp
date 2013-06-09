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
#ifndef ELM_EXEC_H
#define ELM_EXEC_H

#include <dbr/defs.h>
#include <dbr/model.h>

struct ElmExec {
    struct ElmCtx* ctx;
    struct ElmIndex* index;
    DbrModel model;
};

DBR_EXTERN void
elm_exec_init(struct ElmExec* exec, struct ElmCtx* ctx, struct ElmIndex* index, DbrModel model);

DBR_EXTERN struct DbrOrder*
elm_exec_submit(struct ElmExec* exec, struct DbrRec* trec, struct DbrRec* arec, const char* ref,
                struct DbrRec* mrec, int action, DbrTicks ticks, DbrLots lots, DbrLots min,
                DbrFlags flags, DbrMillis now, struct DbrTrans* trans);

DBR_EXTERN struct DbrOrder*
elm_exec_revise_id(struct ElmExec* exec, struct ElmTrader* trader, DbrIden id, DbrLots lots,
                   DbrMillis now);

DBR_EXTERN struct DbrOrder*
elm_exec_revise_ref(struct ElmExec* exec, struct ElmTrader* trader, const char* ref, DbrLots lots,
                    DbrMillis now);

DBR_EXTERN struct DbrOrder*
elm_exec_cancel_id(struct ElmExec* exec, struct ElmTrader* trader, DbrIden id, DbrMillis now);

DBR_EXTERN struct DbrOrder*
elm_exec_cancel_ref(struct ElmExec* exec, struct ElmTrader* trader, const char* ref, DbrMillis now);

DBR_EXTERN DbrBool
elm_exec_archive_order(struct ElmExec* exec, struct ElmTrader* trader, DbrIden id, DbrMillis now);

DBR_EXTERN DbrBool
elm_exec_archive_trade(struct ElmExec* exec, struct ElmAccnt* accnt, DbrIden id, DbrMillis now);

DBR_EXTERN void
elm_exec_free_matches(struct ElmExec* exec, struct DbrSlNode* first);

#endif // ELM_EXEC_H
