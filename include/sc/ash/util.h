/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_UTIL_H
#define SC_ASH_UTIL_H

#include <sc/ash/defs.h>

#include <stddef.h> // size_t

/**
 * @addtogroup Util
 * @{
 */

SC_API int
sc_int_dig(int i);

SC_API int
sc_long_dig(long l);

static inline size_t
sc_int_len(int i)
{
    return sc_int_dig(i) + (i < 0 ? 1 : 0);
}

static inline size_t
sc_long_len(long l)
{
    return sc_long_dig(l) + (l < 0 ? 1 : 0);
}

/**
 * Milliseconds since Unix epoch.
 */

SC_API long
sc_millis();

/** @} */

#endif // SC_ASH_UTIL_H
