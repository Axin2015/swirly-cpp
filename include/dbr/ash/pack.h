/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef DBR_ASH_PACK_H
#define DBR_ASH_PACK_H

#include <dbr/ash/defs.h>

#include <stdarg.h>
#include <stddef.h> // size_t

/**
 * @addtogroup Pack
 * @{
 */

DBR_API size_t
dbr_pack_leni(int i);

DBR_API char*
dbr_packi(char* buf, int i);

/**
 * @return NULL on error.
 */

DBR_API const char*
dbr_unpacki(const char* buf, int* i);

DBR_API size_t
dbr_pack_lenl(long l);

DBR_API char*
dbr_packl(char* buf, long l);

/**
 * @return NULL on error.
 */

DBR_API const char*
dbr_unpackl(const char* buf, long* l);

static inline size_t
dbr_pack_lenz(size_t z)
{
    return dbr_pack_lenl(z);
}

static inline char*
dbr_packz(char* buf, size_t z)
{
    return dbr_packl(buf, z);
}

/**
 * @return NULL on error.
 */

static inline const char*
dbr_unpackz(const char* buf, size_t* z)
{
    long n;
    if ((buf = dbr_unpackl(buf, &n)))
        *z = n;
    return buf;
}

DBR_API size_t
dbr_pack_lens(const char* s, size_t m);

DBR_API char*
dbr_packs(char* buf, const char* s, size_t m);

/**
 * @return NULL on error.
 */

DBR_API const char*
dbr_unpacks(const char* buf, char* s, size_t m);

DBR_API size_t
dbr_pack_lenf(const char* format, ...);

DBR_API char*
dbr_packf(char* buf, const char* format, ...);

/**
 * @return NULL on error.
 */

DBR_API const char*
dbr_unpackf(const char* buf, const char* format, ...);

DBR_API size_t
dbr_vpack_lenf(const char* format, va_list args);

DBR_API char*
dbr_vpackf(char* buf, const char* format, va_list args);

/**
 * @return NULL on error.
 */

DBR_API const char*
dbr_vunpackf(const char* buf, const char* format, va_list args);

/** @} */

#endif // DBR_ASH_PACK_H
