/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_UTIL_H
#define DBR_ASH_UTIL_H

#include <dbr/ash/defs.h>

#include <stddef.h> // size_t

/**
 * @addtogroup Util
 * @{
 */

DBR_API int
dbr_int_dig(int i);

DBR_API int
dbr_long_dig(long l);

static inline size_t
dbr_int_len(int i)
{
    return dbr_int_dig(i) + (i < 0 ? 1 : 0);
}

static inline size_t
dbr_long_len(long l)
{
    return dbr_long_dig(l) + (l < 0 ? 1 : 0);
}

/**
 * Milliseconds since Unix epoch.
 */

DBR_API long
dbr_millis();

/** @} */

#endif // DBR_ASH_UTIL_H
