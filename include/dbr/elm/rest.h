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
    DBR_RESRC_TRADER  =  5 << 16,
    DBR_RESRC_GIVEUP  =  6 << 16,
    DBR_RESRC_ORDER   =  7 << 16,
    DBR_RESRC_TRADE   =  8 << 16,
    DBR_RESRC_POSN    =  9 << 16,
    DBR_RESRC_MARKET  = 10 << 16
};

enum DbrParam {
    DBR_PARAM_ID         = 1 << 0,
    DBR_PARAM_ACCNT      = 1 << 1,
    DBR_PARAM_GIVEUP     = 1 << 2,
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
    DbrMnem giveup;
    DbrMnem contr;
    DbrIsoDate settl_date;
    DbrRef ref;
    int action;
    DbrTicks ticks;
    DbrLots lots;
    DbrLots min_lots;
};

static inline void
dbr_rest_clear_method(struct DbrRest* rest)
{
    rest->fields = (rest->fields & ~DBR_METHOD_MASK);
}

static inline void
dbr_rest_set_method(struct DbrRest* rest, unsigned val)
{
    rest->fields = (rest->fields & ~DBR_METHOD_MASK) | val;
}

static inline unsigned
dbr_rest_get_method(const struct DbrRest* rest)
{
    return rest->fields & DBR_METHOD_MASK;
}

static inline void
dbr_rest_clear_resrc(struct DbrRest* rest)
{
    rest->fields = (rest->fields & ~DBR_RESRC_MASK);
}

static inline void
dbr_rest_set_resrc(struct DbrRest* rest, unsigned val)
{
    rest->fields = (rest->fields & ~DBR_RESRC_MASK) | val;
}

static inline unsigned
dbr_rest_get_resrc(const struct DbrRest* rest)
{
    return rest->fields & DBR_RESRC_MASK;
}

static inline void
dbr_rest_clear_param(struct DbrRest* rest)
{
    rest->fields = (rest->fields & ~DBR_PARAM_MASK);
}

static inline void
dbr_rest_set_param(struct DbrRest* rest, unsigned bs)
{
    rest->fields |= bs;
}

static inline unsigned
dbr_rest_get_param(const struct DbrRest* rest)
{
    return rest->fields & DBR_PARAM_MASK;
}

DBR_API void
dbr_rest_init(struct DbrRest* rest);

DBR_API int
dbr_rest_rurl(struct DbrRest* rest, const char* buf, size_t size);

DBR_API int
dbr_rest_json(struct DbrRest* rest, const char* buf, size_t size);

/** @} */

#endif // DBR_ASH_REST_H
