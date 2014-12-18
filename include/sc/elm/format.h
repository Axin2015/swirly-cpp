/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_FORMAT_H
#define SC_ASH_FORMAT_H

#include <sc/ash/defs.h>

#include <stdarg.h>

/**
 * @addtogroup Format
 * @{
 */

struct ScPriceString {
    /**
     * 10 decimal digits + 3 nulls + dp + sign + pad
     */
    char buf[16];
    char* small;
    char* pips;
    char* big;
};

SC_API void
sc_format_price(double price, int price_dp, int pip_dp, struct ScPriceString* ps);

SC_API char*
sc_vformat(char* buf, const char* format, va_list args);

SC_API char*
sc_format(char* buf, const char* format, ...);

/** @} */

#endif // SC_ASH_FORMAT_H
