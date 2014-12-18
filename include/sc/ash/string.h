/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef SC_ASH_STRING_H
#define SC_ASH_STRING_H

#include <ctype.h>  // isspace()

/**
 * @addtogroup String
 * @{
 */

static inline const char*
sc_ltrim(const char* begin, const char* end)
{
    // Skip leading whitespace.
    while (begin != end && isspace(*begin))
        ++begin;
    return begin;
}

static inline const char*
sc_rtrim(const char* begin, const char* end)
{
    // Skip trailing whitespace.
    while (begin != end && isspace(*(end - 1)))
        --end;
    return end;
}

/** @} */

#endif // SC_ASH_STRING_H
