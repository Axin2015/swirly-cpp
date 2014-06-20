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
#ifndef DBR_ASH_REST_H
#define DBR_ASH_REST_H

#include <dbr/elm/types.h>

#include <dbr/ash/defs.h>

/**
 * @addtogroup Rest
 * @{
 */

enum {
    DBR_METHOD_MASK = 0xFF000000,
    DBR_RESRC_MASK  = 0x00FF0000,
    DBR_PARAM_MASK  = 0x0000FFFF
};

enum DbrMethod {
    DBR_METHOD_DELETE = 1 << 24,
    DBR_METHOD_GET    = 2 << 24,
    DBR_METHOD_HEAD   = 3 << 24,
    DBR_METHOD_POST   = 4 << 24,
    DBR_METHOD_PUT    = 5 << 24,
};

enum DbrResrc {
    DBR_RESRC_LOGON   =  1 << 16,
    DBR_RESRC_LOGOFF  =  2 << 16,
    DBR_RESRC_ACCNT   =  3 << 16,
    DBR_RESRC_CONTR   =  4 << 16,
    DBR_RESRC_USER    =  5 << 16,
    DBR_RESRC_GROUP   =  6 << 16,
    DBR_RESRC_ORDER   =  7 << 16,
    DBR_RESRC_TRADE   =  8 << 16,
    DBR_RESRC_POSN    =  9 << 16,
    DBR_RESRC_MARKET  = 10 << 16
};

enum DbrParam {
    DBR_PARAM_ID         = 1 << 0,
    DBR_PARAM_ACCNT      = 1 << 1,
    DBR_PARAM_GROUP      = 1 << 2,
    DBR_PARAM_CONTR      = 1 << 3,
    DBR_PARAM_SETTL_DATE = 1 << 4,
    DBR_PARAM_REF        = 1 << 5,
    DBR_PARAM_ACTION     = 1 << 6,
    DBR_PARAM_TICKS      = 1 << 7,
    DBR_PARAM_LOTS       = 1 << 8,
    DBR_PARAM_MIN_LOTS   = 1 << 9
};

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
    unsigned fields;
    DbrIden id;
    DbrMnem accnt;
    DbrMnem group;
    DbrMnem contr;
    DbrIsoDate settl_date;
    DbrRef ref;
    int action;
    DbrTicks ticks;
    DbrLots lots;
    DbrLots min_lots;
};

DBR_API void
dbr_rest_init(struct DbrRest* rest);

DBR_API int
dbr_rest_rurl(struct DbrRest* rest, const char* buf, size_t size);

DBR_API int
dbr_rest_json(struct DbrRest* rest, const char* buf, size_t size);

/** @} */

#endif // DBR_ASH_REST_H
