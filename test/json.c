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

#include <dbr/rest.h>

static int
parse(struct DbrRest* rest, const char* buf)
{
    return dbr_rest_json(rest, buf, strlen(buf));
}

/**
 * @test Json zero.
 */

void
json_zero(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "{}") == 1);
    check(rest.func == 0);

    dbr_rest_init(&rest);
    check(parse(&rest, " { } ") == 1);
    check(rest.func == 0);

    dbr_rest_init(&rest);
    check(parse(&rest, " { ") == 0);
    check(parse(&rest, " } ") == 1);
    check(rest.func == 0);
}

/**
 * @test Json one.
 */

void
json_one(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Id\":1}") == 1);
    check(rest.func == DBR_ID_PARAM);
    check(rest.id == 1);

    dbr_rest_init(&rest);
    check(parse(&rest, " { \"Ticks\" : 2 } ") == 1);
    check(rest.func == DBR_TICKS_PARAM);
    check(rest.ticks == 2);

    dbr_rest_init(&rest);
    check(parse(&rest, " { \"") == 0);
    check(parse(&rest, "Lots\"") == 0);
    check(parse(&rest, " :") == 0);
    check(parse(&rest, " 3 } ") == 1);
    check(rest.func == DBR_LOTS_PARAM);
    check(rest.lots == 3);
}

/**
 * @test Json two.
 */

void
json_two(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Id\":1,\"Ref\":\"apple\"}") == 1);
    check(rest.func == (DBR_ID_PARAM | DBR_REF_PARAM));
    check(rest.id == 1);
    check(strcmp(rest.ref, "apple") == 0);

    dbr_rest_init(&rest);
    check(parse(&rest, " { \"Id\" : 2 , \"Market\" : \"orange\" } ") == 1);
    check(rest.func == (DBR_ID_PARAM | DBR_MARKET_PARAM));
    check(rest.id == 2);
    check(strcmp(rest.market, "orange") == 0);

    dbr_rest_init(&rest);
    check(parse(&rest, " { \"I") == 0);
    check(parse(&rest, "d\" :") == 0);
    check(parse(&rest, " 3 , \"") == 0);
    check(parse(&rest, "Accnt\" : \"ba") == 0);
    check(parse(&rest, "nana\" ") == 0);
    check(parse(&rest, "} ") == 1);
    check(rest.func == (DBR_ID_PARAM | DBR_ACCNT_PARAM));
    check(rest.id == 3);
    check(strcmp(rest.accnt, "banana") == 0);
}

/**
 * @test Json long.
 */

void
json_long(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Id\":101}") == 1);
    check(rest.id == 101);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Id\":+101}") == 1);
    check(rest.id == 101);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Id\":-202}") == 1);
    check(rest.id == -202);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Id\":+}") == -1);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Id\":-}") == -1);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Id\":101.0}") == -1);
}

#if 0
/**
 * @test Json double.
 */

void
json_double(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Price\":1.01}") == 1);
    check(fequal(rest.price, 1.01));

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Price\":+1.01}") == 1);
    check(fequal(rest.price, 1.01));

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Price\":-2.02}") == 1);
    check(fequal(rest.price, -2.02));

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Price\":-303.}") == 1);
    check(fequal(rest.price, -303.));

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Price\":-.303}") == 1);
    check(fequal(rest.price, -.303));

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Price\":+404.}") == 1);
    check(fequal(rest.price, 404.));

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Price\":+.404}") == 1);
    check(fequal(rest.price, .404));

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Price\":+}") == -1);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Price\":-}") == -1);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Price\":.}") == -1);
}
#endif

/**
 * @test Json method.
 */

void
json_method(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Method\":\"Delete\"}") == 1);
    check(rest.func == DBR_DELETE);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Method\":\"Get\"}") == 1);
    check(rest.func == DBR_GET);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Method\":\"Head\"}") == 1);
    check(rest.func == DBR_HEAD);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Method\":\"Post\"}") == 1);
    check(rest.func == DBR_POST);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Method\":\"Put\"}") == 1);
    check(rest.func == DBR_PUT);
}

/**
 * @test Json entity.
 */

void
json_entity(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Entity\":\"Instr\"}") == 1);
    check(rest.func == DBR_INSTR);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Entity\":\"Market\"}") == 1);
    check(rest.func == DBR_MARKET);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Entity\":\"Trader\"}") == 1);
    check(rest.func == DBR_TRADER);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Entity\":\"Accnt\"}") == 1);
    check(rest.func == DBR_ACCNT);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Entity\":\"Order\"}") == 1);
    check(rest.func == DBR_ORDER);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Entity\":\"Trade\"}") == 1);
    check(rest.func == DBR_TRADE);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Entity\":\"Posn\"}") == 1);
    check(rest.func == DBR_POSN);
}

/**
 * @test Json action.
 */

void
json_action(void)
{
    struct DbrRest rest;

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Action\":\"Buy\"}") == 1);
    check(rest.action == DBR_BUY);

    dbr_rest_init(&rest);
    check(parse(&rest, "{\"Action\":\"Sell\"}") == 1);
    check(rest.action == DBR_SELL);
}
