/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "test.hpp"

#include <sc/elm/format.h>
#include <sc/elm/types.h>

#include <limits.h>

TEST_CASE(format_action)
{
    char buf[256];

    *sc_format(buf, "<%A>", SC_ACTION_BUY) = '\0';
    check(strcmp(buf, "<BUY>") == 0);

    *sc_format(buf, "<%A>", SC_ACTION_SELL) = '\0';
    check(strcmp(buf, "<SELL>") == 0);
}

TEST_CASE(format_role)
{
    char buf[256];

    *sc_format(buf, "<%R>", SC_ROLE_MAKER) = '\0';
    check(strcmp(buf, "<MAKER>") == 0);

    *sc_format(buf, "<%R>", SC_ROLE_TAKER) = '\0';
    check(strcmp(buf, "<TAKER>") == 0);
}

TEST_CASE(format_state)
{
    char buf[256];

    *sc_format(buf, "<%S>", SC_STATE_NEW) = '\0';
    check(strcmp(buf, "<NEW>") == 0);

    *sc_format(buf, "<%S>", SC_STATE_REVISE) = '\0';
    check(strcmp(buf, "<REVISE>") == 0);

    *sc_format(buf, "<%S>", SC_STATE_CANCEL) = '\0';
    check(strcmp(buf, "<CANCEL>") == 0);

    *sc_format(buf, "<%S>", SC_STATE_TRADE) = '\0';
    check(strcmp(buf, "<TRADE>") == 0);
}

TEST_CASE(format_int)
{
    char buf[256];

    *sc_format(buf, "<%d>", 0) = '\0';
    check(strcmp(buf, "<0>") == 0);

    *sc_format(buf, "<%d>", -1) = '\0';
    check(strcmp(buf, "<-1>") == 0);

    *sc_format(buf, "<%i>", INT_MAX) = '\0';
    check(strcmp(buf, "<2147483647>") == 0);

    *sc_format(buf, "<%i>", INT_MIN) = '\0';
    check(strcmp(buf, "<-2147483648>") == 0);
}

TEST_CASE(format_long)
{
    char buf[256];

    *sc_format(buf, "<%l>", 0L) = '\0';
    check(strcmp(buf, "<0>") == 0);

    *sc_format(buf, "<%l>", -1L) = '\0';
    check(strcmp(buf, "<-1>") == 0);

    *sc_format(buf, "<%l>", LONG_MAX) = '\0';
    check(strcmp(buf, "<9223372036854775807>") == 0);

    *sc_format(buf, "<%l>", LONG_MIN) = '\0';
    check(strcmp(buf, "<-9223372036854775808>") == 0);
}

TEST_CASE(format_escape)
{
    char buf[256];

    *sc_format(buf, "%%") = '\0';
    check(strcmp(buf, "%") == 0);
}
