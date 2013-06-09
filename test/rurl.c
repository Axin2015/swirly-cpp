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

#include <dbr/rest.h>

static int
parse(struct DbrRest* rest, const char* buf)
{
    return dbr_rest_rurl(rest, buf, strlen(buf));
}

/**
 * @test Rurl instr.
 */

void
rurl_instr(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "/instr") == 1);
    check(rest.func == DBR_INSTR);

    dbr_rest_init(&rest);
    check(parse(&rest, "/instr/") == 1);
    check(rest.func == DBR_INSTR);

    dbr_rest_init(&rest);
    check(parse(&rest, "/instr/EURUSD.SPOTFWD") == 1);
    check(rest.func == (DBR_INSTR | DBR_MNEM_PARAM));
    check(sequal(rest.mnem, "EURUSD.SPOTFWD", DBR_MNEM_MAX));

    dbr_rest_init(&rest);
    check(parse(&rest, "/instr/EURUSD.SPOTFWD/") == 1);
    check(rest.func == (DBR_INSTR | DBR_MNEM_PARAM));
    check(sequal(rest.mnem, "EURUSD.SPOTFWD", DBR_MNEM_MAX));
}

/**
 * @test Rurl market.
 */

void
rurl_market(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "/market") == 1);
    check(rest.func == DBR_MARKET);

    dbr_rest_init(&rest);
    check(parse(&rest, "/market/") == 1);
    check(rest.func == DBR_MARKET);

    dbr_rest_init(&rest);
    check(parse(&rest, "/market/EURUSD") == 1);
    check(rest.func == (DBR_MARKET | DBR_MNEM_PARAM));
    check(sequal(rest.mnem, "EURUSD", DBR_MNEM_MAX));

    dbr_rest_init(&rest);
    check(parse(&rest, "/market/EURUSD/") == 1);
    check(rest.func == (DBR_MARKET | DBR_MNEM_PARAM));
    check(sequal(rest.mnem, "EURUSD", DBR_MNEM_MAX));
}

/**
 * @test Rurl accnt.
 */

void
rurl_accnt(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "/accnt") == 1);
    check(rest.func == DBR_ACCNT);

    dbr_rest_init(&rest);
    check(parse(&rest, "/accnt/") == 1);
    check(rest.func == DBR_ACCNT);

    dbr_rest_init(&rest);
    check(parse(&rest, "/accnt/DBRA") == 1);
    check(rest.func == (DBR_ACCNT | DBR_MNEM_PARAM));
    check(sequal(rest.mnem, "DBRA", DBR_MNEM_MAX));

    dbr_rest_init(&rest);
    check(parse(&rest, "/accnt/DBRA/") == 1);
    check(rest.func == (DBR_ACCNT | DBR_MNEM_PARAM));
    check(sequal(rest.mnem, "DBRA", DBR_MNEM_MAX));
}

/**
 * @test Rurl trader.
 */

void
rurl_trader(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "/trader") == 1);
    check(rest.func == DBR_TRADER);

    dbr_rest_init(&rest);
    check(parse(&rest, "/trader/") == 1);
    check(rest.func == DBR_TRADER);

    dbr_rest_init(&rest);
    check(parse(&rest, "/trader/WRAMIREZ") == 1);
    check(rest.func == (DBR_TRADER | DBR_MNEM_PARAM));
    check(sequal(rest.mnem, "WRAMIREZ", DBR_MNEM_MAX));

    dbr_rest_init(&rest);
    check(parse(&rest, "/trader/WRAMIREZ/") == 1);
    check(rest.func == (DBR_TRADER | DBR_MNEM_PARAM));
    check(sequal(rest.mnem, "WRAMIREZ", DBR_MNEM_MAX));
}

/**
 * @test Rurl order.
 */

void
rurl_order(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "/order/WRAMIREZ") == 1);
    check(rest.func == (DBR_ORDER | DBR_TRADER_PARAM));
    check(sequal(rest.trader, "WRAMIREZ", DBR_MNEM_MAX));

    dbr_rest_init(&rest);
    check(parse(&rest, "/order/WRAMIREZ/") == 1);
    check(rest.func == (DBR_ORDER | DBR_TRADER_PARAM));
    check(sequal(rest.trader, "WRAMIREZ", DBR_MNEM_MAX));

    dbr_rest_init(&rest);
    check(parse(&rest, "/order/WRAMIREZ/123") == 1);
    check(rest.func == (DBR_ORDER | DBR_TRADER_PARAM | DBR_ID_PARAM));
    check(sequal(rest.trader, "WRAMIREZ", DBR_MNEM_MAX));
    check(rest.id == 123);

    dbr_rest_init(&rest);
    check(parse(&rest, "/order/WRAMIREZ/123/") == 1);
    check(rest.func == (DBR_ORDER | DBR_TRADER_PARAM | DBR_ID_PARAM));
    check(sequal(rest.trader, "WRAMIREZ", DBR_MNEM_MAX));
    check(rest.id == 123);
}

/**
 * @test Rurl trade.
 */

void
rurl_trade(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "/trade/DBRA") == 1);
    check(rest.func == (DBR_TRADE | DBR_ACCNT_PARAM));
    check(sequal(rest.accnt, "DBRA", DBR_MNEM_MAX));

    dbr_rest_init(&rest);
    check(parse(&rest, "/trade/DBRA/") == 1);
    check(rest.func == (DBR_TRADE | DBR_ACCNT_PARAM));
    check(sequal(rest.accnt, "DBRA", DBR_MNEM_MAX));

    dbr_rest_init(&rest);
    check(parse(&rest, "/trade/DBRA/123") == 1);
    check(rest.func == (DBR_TRADE | DBR_ACCNT_PARAM | DBR_ID_PARAM));
    check(sequal(rest.accnt, "DBRA", DBR_MNEM_MAX));
    check(rest.id == 123);

    dbr_rest_init(&rest);
    check(parse(&rest, "/trade/DBRA/123/") == 1);
    check(rest.func == (DBR_TRADE | DBR_ACCNT_PARAM | DBR_ID_PARAM));
    check(sequal(rest.accnt, "DBRA", DBR_MNEM_MAX));
    check(rest.id == 123);
}

/**
 * @test Rurl posn.
 */

void
rurl_posn(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "/posn/DBRA") == 1);
    check(rest.func == (DBR_POSN | DBR_ACCNT_PARAM));
    check(sequal(rest.accnt, "DBRA", DBR_MNEM_MAX));

    dbr_rest_init(&rest);
    check(parse(&rest, "/posn/DBRA/") == 1);
    check(rest.func == (DBR_POSN | DBR_ACCNT_PARAM));
    check(sequal(rest.accnt, "DBRA", DBR_MNEM_MAX));
}
