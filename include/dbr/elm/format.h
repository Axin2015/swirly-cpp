/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_FORMAT_H
#define DBR_ASH_FORMAT_H

#include <dbr/ash/defs.h>

#include <stdarg.h>

/**
 * @addtogroup Format
 * @{
 */

struct DbrPriceString {
    /**
     * 10 decimal digits + 3 nulls + dp + sign + pad
     */
    char buf[16];
    char* small;
    char* pips;
    char* big;
};

DBR_API void
dbr_format_price(double price, int price_dp, int pip_dp, struct DbrPriceString* ps);

DBR_API char*
dbr_vformat(char* buf, const char* format, va_list args);

DBR_API char*
dbr_format(char* buf, const char* format, ...);

/** @} */

#endif // DBR_ASH_FORMAT_H
