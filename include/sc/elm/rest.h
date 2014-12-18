/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_REST_H
#define SC_ASH_REST_H

#include <sc/elm/types.h>

#include <sc/ash/defs.h>

/**
 * @addtogroup Rest
 * @{
 */

enum {
    SC_METHOD_MASK = 0xFF000000,
    SC_RESRC_MASK  = 0x00FF0000,
    SC_PARAM_MASK  = 0x0000FFFF
};

enum ScMethod {
    SC_METHOD_DELETE = 1 << 24,
    SC_METHOD_GET    = 2 << 24,
    SC_METHOD_HEAD   = 3 << 24,
    SC_METHOD_POST   = 4 << 24,
    SC_METHOD_PUT    = 5 << 24,
};

enum ScResrc {
    SC_RESRC_LOGON   =  1 << 16,
    SC_RESRC_LOGOFF  =  2 << 16,
    SC_RESRC_ACCNT   =  3 << 16,
    SC_RESRC_CONTR   =  4 << 16,
    SC_RESRC_TRADER  =  5 << 16,
    SC_RESRC_GIVEUP  =  6 << 16,
    SC_RESRC_ORDER   =  7 << 16,
    SC_RESRC_TRADE   =  8 << 16,
    SC_RESRC_POSN    =  9 << 16,
    SC_RESRC_VIEW    = 10 << 16
};

enum ScParam {
    SC_PARAM_ID         = 1 << 0,
    SC_PARAM_ACCNT      = 1 << 1,
    SC_PARAM_GIVEUP     = 1 << 2,
    SC_PARAM_CONTR      = 1 << 3,
    SC_PARAM_SETTL_DATE = 1 << 4,
    SC_PARAM_REF        = 1 << 5,
    SC_PARAM_ACTION     = 1 << 6,
    SC_PARAM_TICKS      = 1 << 7,
    SC_PARAM_LOTS       = 1 << 8,
    SC_PARAM_MIN_LOTS   = 1 << 9
};

struct ScRest {
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
    ScIden id;
    ScMnem accnt;
    ScMnem giveup;
    ScMnem contr;
    ScIsoDate settl_date;
    ScRef ref;
    int action;
    ScTicks ticks;
    ScLots lots;
    ScLots min_lots;
};

static inline void
sc_rest_clear_method(struct ScRest* rest)
{
    rest->fields = (rest->fields & ~SC_METHOD_MASK);
}

static inline void
sc_rest_set_method(struct ScRest* rest, unsigned val)
{
    rest->fields = (rest->fields & ~SC_METHOD_MASK) | val;
}

static inline unsigned
sc_rest_get_method(const struct ScRest* rest)
{
    return rest->fields & SC_METHOD_MASK;
}

static inline void
sc_rest_clear_resrc(struct ScRest* rest)
{
    rest->fields = (rest->fields & ~SC_RESRC_MASK);
}

static inline void
sc_rest_set_resrc(struct ScRest* rest, unsigned val)
{
    rest->fields = (rest->fields & ~SC_RESRC_MASK) | val;
}

static inline unsigned
sc_rest_get_resrc(const struct ScRest* rest)
{
    return rest->fields & SC_RESRC_MASK;
}

static inline void
sc_rest_clear_param(struct ScRest* rest)
{
    rest->fields = (rest->fields & ~SC_PARAM_MASK);
}

static inline void
sc_rest_set_param(struct ScRest* rest, unsigned bs)
{
    rest->fields |= bs;
}

static inline unsigned
sc_rest_get_param(const struct ScRest* rest)
{
    return rest->fields & SC_PARAM_MASK;
}

SC_API void
sc_rest_init(struct ScRest* rest);

SC_API int
sc_rest_rurl(struct ScRest* rest, const char* buf, size_t size);

SC_API int
sc_rest_json(struct ScRest* rest, const char* buf, size_t size);

/** @} */

#endif // SC_ASH_REST_H
