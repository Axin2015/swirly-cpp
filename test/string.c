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
#include "model.h"
#include "test.h"

#include <dbr/conv.h>
#include <dbr/core.h>
#include <dbr/string.h>

#include <limits.h>

static struct DbrRec*
get_rec_mnem(DbrEnv env, int type, const char* mnem)
{
    struct DbrSlNode* node = dbr_rec_find_mnem(env, type, mnem);
    check(node != NULL);
    return dbr_rec_entry(node);
}

/**
 * @test Int length.
 */

void
intlen(void)
{
    check(dbr_intlen(0) == 1);
    check(dbr_intlen(1) == 1);
    check(dbr_intlen(-1) == 2);
    check(dbr_intlen(12) == 2);
    check(dbr_intlen(123) == 3);
    check(dbr_intlen(1234) == 4);
    check(dbr_intlen(12345) == 5);
    check(dbr_intlen(-12345) == 6);
    check(dbr_intlen(123456) == 6);
    check(dbr_intlen(1234567) == 7);
    check(dbr_intlen(12345678) == 8);
    check(dbr_intlen(123456789) == 9);
    check(dbr_intlen(1234567890) == 10);
    check(dbr_intlen(-1234567890) == 11);
    check(dbr_intlen(INT_MIN) == 11);
    check(dbr_intlen(INT_MAX) == 10);
}

/**
 * @test Int length.
 */

void
longlen(void)
{
    check(dbr_longlen(0L) == 1);
    check(dbr_longlen(1L) == 1);
    check(dbr_longlen(-1L) == 2);
    check(dbr_longlen(12L) == 2);
    check(dbr_longlen(123L) == 3);
    check(dbr_longlen(1234L) == 4);
    check(dbr_longlen(12345L) == 5);
    check(dbr_longlen(-12345L) == 6);
    check(dbr_longlen(123456L) == 6);
    check(dbr_longlen(1234567L) == 7);
    check(dbr_longlen(12345678L) == 8);
    check(dbr_longlen(123456789L) == 9);
    check(dbr_longlen(1234567890L) == 10);
    check(dbr_longlen(-1234567890L) == 11);
    check(dbr_longlen(12345678901L) == 11);
    check(dbr_longlen(123456789012L) == 12);
    check(dbr_longlen(1234567890123L) == 13);
    check(dbr_longlen(12345678901234L) == 14);
    check(dbr_longlen(123456789012345L) == 15);
    check(dbr_longlen(-123456789012345L) == 16);
    check(dbr_longlen(1234567890123456L) == 16);
    check(dbr_longlen(12345678901234567L) == 17);
    check(dbr_longlen(123456789012345678L) == 18);
    check(dbr_longlen(1234567890123456789L) == 19);
    check(dbr_longlen(LONG_MIN) == 20);
    check(dbr_longlen(LONG_MAX) == 19);
}

/**
 * @test Format Action.
 */

void
format_action(void)
{
    char buf[256];

    *dbr_format(buf, "<%A>", DBR_BUY) = '\0';
    check(strcmp(buf, "<BUY>") == 0);

    *dbr_format(buf, "<%A>", DBR_SELL) = '\0';
    check(strcmp(buf, "<SELL>") == 0);
}

/**
 * @test Format Role.
 */

void
format_role(void)
{
    char buf[256];

    *dbr_format(buf, "<%R>", DBR_MAKER) = '\0';
    check(strcmp(buf, "<MAKER>") == 0);

    *dbr_format(buf, "<%R>", DBR_TAKER) = '\0';
    check(strcmp(buf, "<TAKER>") == 0);
}

/**
 * @test Format Status.
 */

void
format_status(void)
{
    char buf[256];

    *dbr_format(buf, "<%S>", DBR_NEW) = '\0';
    check(strcmp(buf, "<NEW>") == 0);

    *dbr_format(buf, "<%S>", DBR_REVISED) = '\0';
    check(strcmp(buf, "<REVISED>") == 0);

    *dbr_format(buf, "<%S>", DBR_CANCELLED) = '\0';
    check(strcmp(buf, "<CANCELLED>") == 0);

    *dbr_format(buf, "<%S>", DBR_REJECTED) = '\0';
    check(strcmp(buf, "<REJECTED>") == 0);

    *dbr_format(buf, "<%S>", DBR_PARTIAL) = '\0';
    check(strcmp(buf, "<PARTIAL>") == 0);

    *dbr_format(buf, "<%S>", DBR_FILLED) = '\0';
    check(strcmp(buf, "<FILLED>") == 0);
}

/**
 * @test Format Int.
 */

void
format_int(void)
{
    char buf[256];

    *dbr_format(buf, "<%d>", 0) = '\0';
    check(strcmp(buf, "<0>") == 0);

    *dbr_format(buf, "<%d>", -1) = '\0';
    check(strcmp(buf, "<-1>") == 0);

    *dbr_format(buf, "<%i>", INT_MAX) = '\0';
    check(strcmp(buf, "<2147483647>") == 0);

    *dbr_format(buf, "<%i>", INT_MIN) = '\0';
    check(strcmp(buf, "<-2147483648>") == 0);
}

/**
 * @test Format Long.
 */

void
format_long(void)
{
    char buf[256];

    *dbr_format(buf, "<%l>", 0L) = '\0';
    check(strcmp(buf, "<0>") == 0);

    *dbr_format(buf, "<%l>", -1L) = '\0';
    check(strcmp(buf, "<-1>") == 0);

    *dbr_format(buf, "<%l>", LONG_MAX) = '\0';
    check(strcmp(buf, "<9223372036854775807>") == 0);

    *dbr_format(buf, "<%l>", LONG_MIN) = '\0';
    check(strcmp(buf, "<-9223372036854775808>") == 0);
}

/**
 * @test Format Escape.
 */

void
format_escape(void)
{
    char buf[256];

    *dbr_format(buf, "%%") = '\0';
    check(strcmp(buf, "%") == 0);
}

/**
 * @test Instr copy.
 */

void
instrcpy(void)
{
    DbrCtx ctx = dbr_ctx_create(1);
    DbrModel model = model_create(ctx);
    DbrEnv env = dbr_env_create(ctx, model);

    struct DbrRec* irec = get_rec_mnem(env, DBR_INSTR, "EURUSD.SPOTFWD");
    check(irec != NULL);

    const size_t len = dbr_instrlen(irec);
    char buf[len + 1];
    char* begin = buf;
    char* end = dbr_instrcpy(begin, irec);
    *end = '\0';
    check(len == end - begin);
    check(strcmp(buf, "{\"mnem\":\"EURUSD.SPOTFWD\",\"display\":\"EURUSD.SPOTFWD\",\"asset_type\":\"CURRENCY\",\"instr_type\":\"SPOTFWD\",\"asset\":\"EUR\",\"ccy\":\"USD\",\"tick_numer\":0,\"tick_denom\":0,\"lot_numer\":0,\"lot_denom\":0,\"price_dp\":4,\"pip_dp\":4,\"qty_dp\":0,\"min_lots\":1,\"max_lots\":10}") == 0);

    dbr_env_destroy(env);
    model_destroy(model);
    dbr_ctx_destroy(ctx);
}
