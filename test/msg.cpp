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

#include <dbr/msg.h>
#include <dbr/types.h>

static void
set_instr(struct DbrRec* rec, DbrIden id, const char* mnem, const char* display,
          const char* asset_type, const char* instr_type, const char* asset, const char* ccy,
          int tick_numer, int tick_denom, int lot_numer, int lot_denom, int pip_dp,
          DbrLots min_lots, DbrLots max_lots)
{
    rec->type = DBR_INSTR;
    rec->id = id;
    strncpy(rec->mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec->instr.display, display, DBR_DISPLAY_MAX);
    strncpy(rec->instr.asset_type, asset_type, DBR_MNEM_MAX);
    strncpy(rec->instr.instr_type, instr_type, DBR_MNEM_MAX);
    strncpy(rec->instr.asset, asset, DBR_MNEM_MAX);
    strncpy(rec->instr.ccy, ccy, DBR_MNEM_MAX);
    rec->instr.tick_numer = tick_numer;
    rec->instr.tick_denom = tick_denom;
    rec->instr.lot_numer = lot_numer;
    rec->instr.lot_denom = lot_denom;
    rec->instr.pip_dp = pip_dp;
    rec->instr.min_lots = min_lots;
    rec->instr.max_lots = max_lots;
}

TEST_CASE(msg_instr)
{
    struct DbrRec rec;
    set_instr(&rec, 1, "EURUSD.SPOTFWD", "EURUSD.SPOTFWD", "CURRENCY", "SPOTFWD",
              "EUR", "USD", 1, 10000, 1000000, 1, 4, 1, 10);
    const int len = dbr_instr_len(&rec);
    char buf[len];
    dbr_write_instr(buf, &rec);
    memset(&rec, 0, sizeof(rec));
    dbr_read_instr(buf, &rec);

    check(rec.id == 1);
    check(sequal(rec.mnem, "EURUSD.SPOTFWD", DBR_MNEM_MAX));
    check(sequal(rec.instr.display, "EURUSD.SPOTFWD", DBR_DISPLAY_MAX));
    check(sequal(rec.instr.asset_type, "CURRENCY", DBR_MNEM_MAX));
    check(sequal(rec.instr.instr_type, "SPOTFWD", DBR_MNEM_MAX));
    check(sequal(rec.instr.asset, "EUR", DBR_MNEM_MAX));
    check(sequal(rec.instr.ccy, "USD", DBR_MNEM_MAX));
    check(rec.instr.tick_numer == 1);
    check(rec.instr.tick_denom == 10000);
    check(rec.instr.lot_numer == 1000000);
    check(rec.instr.lot_denom == 1);
    check(rec.instr.pip_dp == 4);
    check(rec.instr.min_lots == 1);
    check(rec.instr.max_lots == 10);
}
