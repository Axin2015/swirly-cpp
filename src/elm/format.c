/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#include "format.h"

#include <sc/elm/types.h>

#include <sc/ash/util.h>

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

    char* cp = (buf += sc_int_dig(i));
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

    char* cp = (buf += sc_long_dig(l));
    do {
        *--cp = DIGITS[ul % 10];
    } while ((ul /= 10));
 done:
    return buf;
}

SC_EXTERN const char*
elm_action_str(int action)
{
    const char* sym;
    switch (action) {
    case SC_ACTION_BUY:
        sym = "BUY";
        break;
    case SC_ACTION_SELL:
        sym = "SELL";
        break;
    default:
        abort();
    }
    return sym;
}

SC_EXTERN size_t
elm_action_len(int action)
{
    size_t len;
    switch (action) {
    case SC_ACTION_BUY:
        len = sizeof("BUY") - 1;
        break;
    case SC_ACTION_SELL:
        len = sizeof("SELL") - 1;
        break;
    default:
        abort();
    }
    return len;
}

SC_EXTERN const char*
elm_role_str(int role)
{
    const char* sym;
    switch (role) {
    case SC_ROLE_MAKER:
        sym = "MAKER";
        break;
    case SC_ROLE_TAKER:
        sym = "TAKER";
        break;
    default:
        abort();
    }
    return sym;
}

SC_EXTERN size_t
elm_role_len(int role)
{
    size_t len;
    switch (role) {
    case SC_ROLE_MAKER:
        len = sizeof("MAKER") - 1;
        break;
    case SC_ROLE_TAKER:
        len = sizeof("TAKER") - 1;
        break;
    default:
        abort();
    }
    return len;
}

SC_EXTERN const char*
elm_state_str(int state)
{
    const char* sym;
    switch (state) {
    case SC_STATE_NEW:
        sym = "NEW";
        break;
    case SC_STATE_REVISE:
        sym = "REVISE";
        break;
    case SC_STATE_CANCEL:
        sym = "CANCEL";
        break;
    case SC_STATE_TRADE:
        sym = "TRADE";
        break;
    default:
        abort();
    }
    return sym;
}

SC_EXTERN size_t
elm_state_len(int state)
{
    size_t len;
    switch (state) {
    case SC_STATE_NEW:
        len = sizeof("NEW") - 1;
        break;
    case SC_STATE_REVISE:
        len = sizeof("REVISE") - 1;
        break;
    case SC_STATE_CANCEL:
        len = sizeof("CANCEL") - 1;
        break;
    case SC_STATE_TRADE:
        len = sizeof("TRADE") - 1;
        break;
    default:
        abort();
    }
    return len;
}

SC_API void
sc_price_format(double price, int price_dp, int pip_dp, struct ScPriceString* ps)
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

SC_API char*
sc_vformat(char* buf, const char* format, va_list args)
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
                buf = printi(buf, sc_jd_to_iso(i));
                break;
            case 'l':
                l = va_arg(args, long);
                buf = printl(buf, l);
                break;
            case 'm':
                i = SC_MNEM_MAX;
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

SC_API char*
sc_format(char* buf, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    buf = sc_vformat(buf, format, args);
    va_end(args);
    return buf;
}
