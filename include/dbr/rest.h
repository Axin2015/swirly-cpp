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
#ifndef DBR_REST_H
#define DBR_REST_H

#include <dbr/defs.h>
#include <dbr/types.h>

/**
 * @addtogroup Rest
 * @{
 */

enum DbrParam {
    DBR_ID_PARAM     = 1 << 16,
    DBR_MNEM_PARAM  =  1 << 17,
    DBR_MARKET_PARAM = 1 << 18,
    DBR_TRADER_PARAM = 1 << 19,
    DBR_ACCNT_PARAM  = 1 << 20,
    DBR_REF_PARAM    = 1 << 21,
    DBR_ACTION_PARAM = 1 << 22,
    DBR_TICKS_PARAM  = 1 << 23,
    DBR_LOTS_PARAM    = 1 << 24
};

#define DBR_PARAM_MASK 0xFFFF0000

struct DbrRest {
    int cs;
    union {
        struct {
            int sign;
            int fcount;
            size_t idigits;
            size_t fdigits;
        } num;
        struct {
            char* buf;
            size_t count;
            size_t max;
        } str;
    };
    unsigned func;
    DbrIden id;
    DbrMnem mnem;
    DbrMnem market;
    DbrMnem trader;
    DbrMnem accnt;
    DbrRef ref;
    int action;
    DbrTicks ticks;
    DbrLots lots;
};

DBR_API void
dbr_rest_init(struct DbrRest* rest);

DBR_API int
dbr_rest_rurl(struct DbrRest* rest, const char* buf, size_t size);

DBR_API int
dbr_rest_json(struct DbrRest* rest, const char* buf, size_t size);

/** @} */

#endif // DBR_REST_H
