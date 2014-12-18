/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_STRING_H
#define DBR_ASH_STRING_H

#include <ctype.h>  // isspace()

/**
 * @addtogroup String
 * @{
 */

static inline const char*
dbr_ltrim(const char* begin, const char* end)
{
    // Skip leading whitespace.
    while (begin != end && isspace(*begin))
        ++begin;
    return begin;
}

static inline const char*
dbr_rtrim(const char* begin, const char* end)
{
    // Skip trailing whitespace.
    while (begin != end && isspace(*(end - 1)))
        --end;
    return end;
}

/** @} */

#endif // DBR_ASH_STRING_H
