/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
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
#include "format.h"

#include <dbr/elm/types.h>
#include <dbr/ash/util.h>

#include <math.h>
#include <stdlib.h> // abort()
#include <string.h>

static const char DIGITS[] = "0123456789";

static char*
printi(char* buf, int i)
{
    if (i == 0) {
        *buf++ = '0';
        goto done;
    }

    unsigned int ui;
    if (i < 0) {
        ui = -i;
        *buf++ = '-';
    } else
        ui = i;

    char* cp = (buf += dbr_int_dig(i));
    do {
        *--cp = DIGITS[ui % 10];
    } while ((ui /= 10));
 done:
    return buf;
}

static char*
printl(char* buf, long l)
{
    if (l == 0) {
        *buf++ = '0';
        goto done;
    }

    unsigned long ul;
    if (l < 0) {
        ul = -l;
        *buf++ = '-';
    } else
        ul = l;

    char* cp = (buf += dbr_long_dig(l));
    do {
        *--cp = DIGITS[ul % 10];
    } while ((ul /= 10));
 done:
    return buf;
}

DBR_EXTERN const char*
elm_action_str(int action)
{
    const char* sym;
    switch (action) {
    case DBR_ACTION_BUY:
        sym = "BUY";
        break;
    case DBR_ACTION_SELL:
        sym = "SELL";
        break;
    default:
        abort();
    }
    return sym;
}

DBR_EXTERN size_t
elm_action_len(int action)
{
    size_t len;
    switch (action) {
    case DBR_ACTION_BUY:
        len = sizeof("BUY") - 1;
        break;
    case DBR_ACTION_SELL:
        len = sizeof("SELL") - 1;
        break;
    default:
        abort();
    }
    return len;
}

DBR_EXTERN const char*
elm_role_str(int role)
{
    const char* sym;
    switch (role) {
    case DBR_ROLE_MAKER:
        sym = "MAKER";
        break;
    case DBR_ROLE_TAKER:
        sym = "TAKER";
        break;
    default:
        abort();
    }
    return sym;
}

DBR_EXTERN size_t
elm_role_len(int role)
{
    size_t len;
    switch (role) {
    case DBR_ROLE_MAKER:
        len = sizeof("MAKER") - 1;
        break;
    case DBR_ROLE_TAKER:
        len = sizeof("TAKER") - 1;
        break;
    default:
        abort();
    }
    return len;
}

DBR_EXTERN const char*
elm_state_str(int state)
{
    const char* sym;
    switch (state) {
    case DBR_STATE_NEW:
        sym = "NEW";
        break;
    case DBR_STATE_REVISE:
        sym = "REVISE";
        break;
    case DBR_STATE_CANCEL:
        sym = "CANCEL";
        break;
    case DBR_STATE_TRADE:
        sym = "TRADE";
        break;
    default:
        abort();
    }
    return sym;
}

DBR_EXTERN size_t
elm_state_len(int state)
{
    size_t len;
    switch (state) {
    case DBR_STATE_NEW:
        len = sizeof("NEW") - 1;
        break;
    case DBR_STATE_REVISE:
        len = sizeof("REVISE") - 1;
        break;
    case DBR_STATE_CANCEL:
        len = sizeof("CANCEL") - 1;
        break;
    case DBR_STATE_TRADE:
        len = sizeof("TRADE") - 1;
        break;
    default:
        abort();
    }
    return len;
}

DBR_API void
dbr_price_format(double price, int price_dp, int pip_dp, struct DbrPriceString* ps)
{
    static const char DIGITS[] = "0123456789";

    // Digits in price as integer.
    int digits = fabs(price) * pow(10, price_dp) + 0.5;
    char* cp = ps->buf + sizeof(ps->buf) - 1;

    *cp-- = '\0';
    while (price_dp > pip_dp) {
        *cp-- = DIGITS[digits % 10];
        digits /= 10;
        if (--price_dp == 0)
            *cp-- = '.';
    }
    ps->small = cp + 1;

    // Lower-order pip digit.
    *cp-- = '\0';
    *cp-- = DIGITS[digits % 10];
    digits /= 10;

    if (--price_dp == 0)
        *cp-- = '.';

    // Higher-order pip digit.
    *cp-- = DIGITS[digits % 10];
    digits /= 10;
    ps->pips = cp + 1;

    *cp-- = '\0';
    while (digits > 0 || price_dp > 0) {
        if (--price_dp == 0)
            *cp-- = '.';
        *cp-- = DIGITS[digits % 10];
        digits /= 10;
    }
    // Signedness.
    if (price < 0)
        *cp-- = '-';

    ps->big = cp + 1;
}

DBR_API char*
dbr_vformat(char* buf, const char* format, va_list args)
{
    assert(buf);
    assert(format);

    int esc = 0;
    for (const char* cp = format; *cp != '\0'; ++cp) {
        const char ch = *cp;
        if (esc) {
            int i;
            long l;
            const char* s;
            switch (ch) {
            case 'A':
                i = va_arg(args, int);
                buf = stpcpy(buf, elm_action_str(i));
                break;
            case 'R':
                i = va_arg(args, int);
                buf = stpcpy(buf, elm_role_str(i));
                break;
            case 'S':
                i = va_arg(args, int);
                buf = stpcpy(buf, elm_state_str(i));
                break;
            case 'd':
            case 'i':
                i = va_arg(args, int);
                buf = printi(buf, i);
                break;
            case 'j':
                i = va_arg(args, int);
                buf = printi(buf, dbr_jd_to_iso(i));
                break;
            case 'l':
                l = va_arg(args, long);
                buf = printl(buf, l);
                break;
            case 'm':
                i = DBR_MNEM_MAX;
                s = va_arg(args, const char*);
                // stpncpy() zero pads so cannot use.
                for (; *s != '\0' && i > 0; ++s, --i)
                    *buf++ = *s;
                break;
            case 's':
                i = va_arg(args, int);
                s = va_arg(args, const char*);
                // stpncpy() zero pads so cannot use.
                for (; *s != '\0' && i > 0; ++s, --i)
                    *buf++ = *s;
                break;
            default:
                *buf++ = ch;
                break;
            }
            esc = 0;
        } else if (ch == '%')
            esc = 1;
        else
            *buf++ = ch;
    }
    return buf;
}

DBR_API char*
dbr_format(char* buf, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    buf = dbr_vformat(buf, format, args);
    va_end(args);
    return buf;
}
