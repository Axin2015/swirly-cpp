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

#include <dbr/proto.h>
#include <dbr/types.h>

static void
set_contr(DbrRec* rec, DbrIden id, const char* mnem, const char* display, const char* asset_type,
          const char* asset, const char* ccy, int tick_numer, int tick_denom, int lot_numer,
          int lot_denom, int pip_dp, DbrLots min_lots, DbrLots max_lots)
{
    rec->type = DBR_CONTR;
    rec->id = id;
    strncpy(rec->mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec->display, display, DBR_DISPLAY_MAX);
    strncpy(rec->contr.asset_type, asset_type, DBR_MNEM_MAX);
    strncpy(rec->contr.asset, asset, DBR_MNEM_MAX);
    strncpy(rec->contr.ccy, ccy, DBR_MNEM_MAX);
    rec->contr.tick_numer = tick_numer;
    rec->contr.tick_denom = tick_denom;
    rec->contr.lot_numer = lot_numer;
    rec->contr.lot_denom = lot_denom;
    rec->contr.pip_dp = pip_dp;
    rec->contr.min_lots = min_lots;
    rec->contr.max_lots = max_lots;
}

TEST_CASE(proto_contr)
{
    DbrRec rec;
    set_contr(&rec, 1, "EURUSD", "EURUSD", "CURRENCY", "EUR", "USD",
              1, 10000, 1000000, 1, 4, 1, 10);
    const size_t n = dbr_contr_len(&rec);
    char buf[n];
    dbr_write_contr(buf, &rec);
    memset(&rec, 0, sizeof(rec));
    dbr_read_contr(buf, &rec);

    check(rec.id == 1);
    check(sequal(rec.mnem, "EURUSD", DBR_MNEM_MAX));
    check(sequal(rec.display, "EURUSD", DBR_DISPLAY_MAX));
    check(sequal(rec.contr.asset_type, "CURRENCY", DBR_MNEM_MAX));
    check(sequal(rec.contr.asset, "EUR", DBR_MNEM_MAX));
    check(sequal(rec.contr.ccy, "USD", DBR_MNEM_MAX));
    check(rec.contr.tick_numer == 1);
    check(rec.contr.tick_denom == 10000);
    check(rec.contr.lot_numer == 1000000);
    check(rec.contr.lot_denom == 1);
    check(rec.contr.pip_dp == 4);
    check(rec.contr.min_lots == 1);
    check(rec.contr.max_lots == 10);
}
